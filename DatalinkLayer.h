#pragma once
#include "NetLayer.h"
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>


class DatalinkLayer : public NetLayer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;    
protected:
    std::string xorChecksum(std::string sdu);
};