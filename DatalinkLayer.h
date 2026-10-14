#pragma once
#include "Layer.h"
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>


class DatalinkLayer : public Layer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    void setMac(std::string newMac){
        myMac = newMac;
    }
protected:
    std::string xorChecksum(const std::string& sdu);
    std::string myMac;
};