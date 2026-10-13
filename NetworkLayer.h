#pragma once
#include "NetLayer.h"
#include <string>

class NetworkLayer : public NetLayer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;
protected:
    bool updateTtl(int ttl);
    bool checkDst(const std::string& dstIp);
    void updateIp(std::string newIp){
        ip = newIp;
    }
    std::string ip;
};