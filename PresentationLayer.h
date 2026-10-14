#pragma once
#include "Layer.h"
#include <string>

class PresentationLayer : public Layer {
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
private:
    
    std::string encrypt(const PacketInfo& data);
    std::string decrypt(const PacketInfo& data);
};