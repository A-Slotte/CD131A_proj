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
  * Motsvarar TCP/UDP i verkligheten.
  */
class TransportLayer : public Layer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    void setPort(std::string port){
        port_ = port;
    }
    size_t segmentsInLastSend() const override { return lastSegmentCount_; }
protected:
    struct fragmentBuffer {
        size_t totalFrags;
        std::string presentationHeader;   // ENC=...; från fragmenten
        std::vector<std::string> recvFrags;
        std::vector<bool> receivedFrags;
        size_t receivedCount = 0;
        std::chrono::steady_clock::time_point firstSeen;
    };
    struct fragmentKey {
        std::string src;   // avsändarnod, annars krockar SID/SEQ mellan olika noder
        std::string seq;
        std::string sid;

        fragmentKey() = default;
        fragmentKey(const std::string& sr, const std::string& s, const std::string& si) : src(sr), seq(s), sid(si) {}

        //Operator på nyckelvärdena för jämförelse i RB-träd(map)
        bool operator<(const fragmentKey& other) const {
            return std::tie(src, sid, seq) < std::tie(other.src, other.sid, other.seq);
        };

        bool operator==(const fragmentKey& other) const {
            return src == other.src && seq == other.seq && sid == other.sid;
        };
    };
    std::vector<std::string> fragmentSdu(std::string& msg, size_t maxFragSize);
    bool splitIncomingPacket(
        const std::string& pdu,
        std::string& transportHeader,
        std::string& payload,
        std::string& sessionPayload
    );
    void decapsulateFragments(
        PacketInfo& packetInfo,
        const std::string& payload,
        const std::string& sessionPayload,
        size_t fragmentNumber,
        size_t totalFrags
    );
    std::string defragPdu(
        const std::string& frag,
        size_t fragmentNumber,
        size_t totalFrags,
        const std::string& src,
        const std::string& seq,
        const std::string& sid
    );
    void cleanStuckFrags();
    bool checkDport(std::string);

    std::map<fragmentKey, fragmentBuffer> fragmentBuffers_;
    std::string port_;
    static constexpr size_t maxFragmentSize = 50;   // max tecken chiffertext per fragment
    size_t lastSegmentCount_ = 1;
};