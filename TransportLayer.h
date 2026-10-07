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
        int totalFrags;
        std::vector<std::string> recvFrags;
        std::chrono::steady_clock::time_point firstSeen;
    };
    struct fragmentKey {
        int seq;
        std::string sid;

        //Operator på nyckelvärdena för jämförelse i RB-träd(map)
        bool operator<(const fragmentKey& other) const {
            if(sid != other.sid) {
                return sid < other.sid;
            };
            return seq < other.seq;
        };
    };
    bool checkMsgLength(std::string msg);
    std::vector<std::string> fragMsg(std::string& msg, size_t maxFragSize);
    std::string defragMsg(std::string& frag);
    void cleanStuckFrags();
    std::map<fragmentKey, fragmentBuffer> fragmentBuffers_;
};