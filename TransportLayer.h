#pragma once
#include "Layer.h"
#include <vector>
#include <string>
#include <map>
#include <chrono>

class TransportLayer : public Layer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    void setPort(std::string port){
        port_ = port;
    }
protected:
    struct fragmentBuffer {
        size_t totalFrags;
        std::vector<std::string> recvFrags;
        std::vector<bool> receivedFrags;
        size_t receivedCount = 0;
        std::chrono::steady_clock::time_point firstSeen;
    };
    struct fragmentKey {
        std::string seq;
        std::string sid;

        fragmentKey() = default;
        fragmentKey(const std::string& s, const std::string& si) : seq(s), sid(si) {}

        //Operator på nyckelvärdena för jämförelse i RB-träd(map)
        bool operator<(const fragmentKey& other) const {
            if(sid != other.sid) {
                return sid < other.sid;
            };
            return seq < other.seq;
        };

        bool operator==(const fragmentKey& other) const {
            return seq == other.seq && sid == other.sid;
        };
    };
    bool checkSduLength(std::string msg);
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
        const std::string& seq,
        const std::string& sid
    );
    void cleanStuckFrags();
    bool checkDport(std::string);

    std::map<fragmentKey, fragmentBuffer> fragmentBuffers_;
    std::string port_;
};