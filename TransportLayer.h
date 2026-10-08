#pragma once
#include "NetLayer.h"
#include <vector>
#include <string>
#include <map>
#include <chrono>

class TransportLayer : public NetLayer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;
protected:
    struct fragmentBuffer {
        size_t totalFrags;
        std::vector<std::string> recvFrags;
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
    bool checkMsgLength(std::string msg);
    std::vector<std::string> fragMsg(std::string& msg, size_t maxFragSize);
    std::string defragMsg(std::string& frag);
    void cleanStuckFrags();
    std::map<fragmentKey, fragmentBuffer> fragmentBuffers_;
};