#pragma once
#include "Layer.h"
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <tuple>

/**
 * @page TransportLayer.h
 * @brief L4 - Transportlagret.
 */

 /**
  * @brief L4 - Transport: identifierar paket adresserade till chat applikationen.
  * Hanterar fragmentering av paket.
  * 
  * Motsvarar TCP/UDP i verkligheten. Använder portnummer portnummer SPORT och DPORT för att identifiera vilken tjänst datan ska till.
  * Vår chatapplikation i implementationen så koden kontrollerar endast att den är addreserad till vår port. Fragmenterar data som sänds ut
  * ifall den är för lång. Bygger ihop inkommande fragment, under tiden lagras fragment i en buffer fragmentBuffers_ nyckeln är SEQ, SID och src noden(avsändaren).
  * Varje buffer får en tidsstämpel och rensas efter en viss tid om resterande meddelanden inte kommer. Fragmenten sätts ihop i rätt ordning oavsett vilken ordning. 
  * 
  * PDU-format: @c SPORT=<port>;DPORT=<port>;FRAG=<i>/<n>;DATA=<L5-PDU>
  */
class TransportLayer : public Layer{
public:
 /**
     * @brief Lägger till portar och FRAG och skickar ett eller flera fragments till L3.
     *
     * Fragment i får SEQ = packetInfo.seq + (i - 1).
     *
     * @param packetInfo Information om meddelandet; @c DPORT, @c sid och @c seq används.
     * @param sdu L5 PDU:n
     */
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    /**
     * @brief Kontrollerar DPORT och FRAG och skickar L5-PDU:n uppåt.
     *
     * Paket utan fragment skickas direkt till L5 ifall DPORT stämmer.
     *
     * @param packetInfo Information från lagren under; @c src används för fragmentnyckeln.
     * @param pdu L4-PDU:n
     * @throws PacketDropped om PDU:n är felformaterad eller DPORT inte är denna nods port.
     */
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;

    /// Sätter nodens logiska port (används som SPORT och jämförs mot DPORT).
    void setPort(std::string port){port_ = port;}

    /// Överskuggar segmentsInLastSend i basklass. Räknar ut fragmentens nästa seq. 
    size_t segmentsInLastSend() const override { return lastSegmentCount_; }

private:
    /// Fragment som tagits emot för ett meddelande som ännu inte är komplett.
    struct fragmentBuffer {
        size_t totalFrags;  ///< n i FRAG=i/n.
        std::string presentationHeader;   ///< ENC=...; från fragmenten.
        std::vector<std::string> recvFrags; ///< Chiffertextbit per fragment, index i-1.
        std::vector<bool> receivedFrags; ///< Om fragment i-1 har kommit, skydd mot dubbleter 
        size_t receivedCount = 0;
        std::chrono::steady_clock::time_point firstSeen;  ///< När första fragmentet kom
    };

    /// Identifierar vilket meddelande ett fragment tillhör.
    struct fragmentKey {
        std::string src;  
        std::string seq; 
        std::string sid;

        fragmentKey() = default;
        fragmentKey(const std::string& sr, const std::string& s, const std::string& si) : src(sr), seq(s), sid(si) {}

        //Operator på nyckelvärdena för jämförelse i RB-träd(map) Bestämmer vad som jämförs, 
        // Ordning -> src -> sid -> seq.
        bool operator<(const fragmentKey& other) const {
            if(src != other.src){
                return src < other.src;
            }
            if(sid != other.sid){
                return sid < other.sid;
            }
            return seq < other.seq;
        }
    };
    
    /**
     * @brief Delar en sträng i bitar om högst maxFragSize tecken.
     * @return Bitarna i ordning; den sista kan vara kortare.
     */
    std::vector<std::string> fragmentSdu(std::string& msg, size_t maxFragSize);

     /**
     * @brief Delar ett mottaget segment i L4-header, L5-PDU och L6-PDU.
     * @param pdu             Hela L4-PDU:n.
     * @param transportHeader Får SPORT/DPORT/FRAG-delen.
     * @param payload         Får L5-PDU:n (SID=...;SEQ=...;DATA=...).
     * @param sessionPayload  Får L6-PDU:n (ENC=...;DATA=...).
     * @return false om någon av headersen saknar "DATA=".
     */

    bool splitIncomingPacket(
        const std::string& pdu,
        std::string& transportHeader,
        std::string& payload,
        std::string& sessionPayload
    );

   /**
     * @brief Hanterar ett fragment: buffrar det och skickar upp meddelandet när alla delar kommit.
     * @param packetInfo     Information om segmentet; får seq/sid/fragments satta innan den skickas upp.
     * @param payload        Fragmentets L5-PDU (för SID/SEQ).
     * @param sessionPayload Fragmentets L6-PDU (ENC-header + chiffertextbit).
     * @param fragmentNumber i i FRAG=i/n.
     * @param totalFrags     n i FRAG=i/n.
     */
    void decapsulateFragments(PacketInfo& packetInfo, const std::string& payload, const std::string& sessionPayload, size_t fragmentNumber, size_t totalFrags);

    /**
     * @brief Lägger ett fragment i rätt buffer och sätter ihop meddelandet när det är komplett.
     * @return Den sammansatta L5-PDU:n, eller tom sträng om fragment fortfarande saknas.
     */
    std::string defragPdu(const std::string& frag, size_t fragmentNumber, size_t totalFrags, const std::string& src, const std::string& seq, const std::string& sid);

    /// Tar bort buffrar för meddelanden som inte blivit kompletta inom 30 s (ett fragment har tappats).
    void cleanStuckFrags();

    /// true om porten är denna nods port.
    bool checkDport(std::string);

    std::map<fragmentKey, fragmentBuffer> fragmentBuffers_; /// Fragment av meddelanden.
    std::string port_;
    static constexpr size_t maxFragmentSize = 50;   // max tecken chiffertext per fragment
    size_t lastSegmentCount_ = 1;
};