#pragma once
#include "NetLayer.h"
#include <string>

class NetworkLayer : public NetLayer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void updateIp(std::string newIp){
        ip = newIp;
    }
protected:
    bool updateTtl(int ttl);
    bool checkDst(const std::string& dstIp);
    
    std::string ip;
};