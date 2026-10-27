#pragma once
#include "Layer.h"
#include <string>

class NetworkLayer : public Layer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    void updateSrc(std::string newSrc){
        node = newSrc;
    }
protected:
    bool updateTtl(int ttl);
    bool checkDst(const std::string& dstIp);
    
    std::string node;
};