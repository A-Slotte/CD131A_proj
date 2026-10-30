#pragma once
#include "Layer.h"
#include <map>
#include <mutex>
#include <utility>

/**
 * @file SessionLayer.h
 * @brief L5 - Sessionlagret.
*/

/**
 * @brief L5 - Session: håller ihop en konversation mellan två noder.
 * 
 * Motsvarar i sessionslagret i OSI, med verkliga exempel som RCP, NetBIOS och SIP(Session Initiation Protocol).
 * I TCP/IP finns inget eget sessionslager utan är utsprid över t.ex. TLS, HTTP-cookies 
 * och TCP-anslutningen.
 * 
 * Varje nod får en egen session med ett SID. SEQ räknas up för varje skickat paket och används
 * som synkronisering för att upptäcka saknade, dubbleter eller gamla meddelanden.
 * 
 * PDU-format: @c SID=<sessions-id>;SEQ=<sekvensnummer>;DATA=<L6-PDU>
 * 
 *Sändning och mottagning körs i olika trådar (användaren respektive physicalLayers mottagartråd), därför skyddas sessiondatan med en mutex(Delad variabel mellan trådarna). 
 */
class SessionLayer : public Layer{
public:
     /**
     * @brief Lägger till SID och SEQ och skickar PDU:n till L4.
     *
     * Skapar en ny session första gången en mottagare används. Om L4 delar
     * meddelandet i flera fragment får varje fragment ett eget SEQ, så nästa
     * meddelande börjar efter det sista fragmentets SEQ.
     *
     * @param packetInfo 
     * Information om meddelandet; @c dst avgör vilken session som används.
     * @param sdu från L6.      
     * 
     */
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;

    /**
     * @brief Kontrollerar SID/SEQ i en mottagen PDU och skickar L6-PDU:n uppåt.
     *
     * En session identifieras av avsändarens adress och dess SID. Paketet kastas
     * om SEQ är lika med eller lägre än det senast mottagna (dubblett eller gammalt).
     * Vid en lucka skrivs antalet saknade paket ut och sessionen synkas om.
     *
     * @param packetInfo Information från lagren under; @c src och @c fragments används.
     * @param pdu        L5-PDU:n från L4.
     * @throws PacketDropped om PDU:n är felformaterad eller SEQ inte är nytt.
     */
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
private:

    /// En egen session mot en mottagare.
    struct sessionInfo{
        int sid_; /// Sessions-id som skickas i SID-fältet.
        int sendSeq_;   /// senaste skickade SEQ till motparten
    };

    sessionInfo& getSession(const std::string& peer);
    bool parseNumber(const std::string& text, int& out) const;

    // egna sessioner, key: mottagarens adress
    std::map<std::string, sessionInfo> sessions; 
    
    // senaste mottagna SEQ, key: (avsändare, avsändarens SID)
    std::map<std::pair<std::string, int>, int> receivedSeq_; 
    
    /// Skyddar sessions och receivedSeq_ (sändning och mottagning körs i olika trådar).
    std::mutex sessionsMutex_;    

};
