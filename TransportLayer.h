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
        int recvFrags;
        std::chrono::steady_clock::time_point firstSeen;
    };
    struct fragmentKey {
        int seq;
        std::string sin;

        //Operator på nyckelvärdena för jämförelse i RB-träd(map)
        bool operator<(const fragmentKey& other) const {
            if(sin != other.sin) {
                return sin < other.sin;
            };
            return seq < other.seq;
        };
    };

    std::vector<std::string> fragPacket(std::string& packet);
    std::string defragPacket(std::string& frag);
    void cleanFrags();
    std::map<fragmentKey, fragmentBuffer> fragmentBuffers_;
};