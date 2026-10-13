#pragma once
#include "NetLayer.h"
#include <string>

class PhysicalLayer : public NetLayer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;
protected:
    void biterror(std::string pdu);
    

};