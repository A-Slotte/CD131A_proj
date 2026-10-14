#pragma once
#include "Layer.h"
#include <string>

class PhysicalLayer : public Layer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
protected:
    void biterror(std::string pdu);
    

};