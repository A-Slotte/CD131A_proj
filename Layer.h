#pragma once
#include <string>
#include "PacketInfo.h"
#include <vector>
#include <sstream>

class PacketDropped final {};

class Layer {
public:
    Layer() = default;
    void connect(Layer* up, Layer* down);

    std::vector<std::string> split(const std::string& is, char delim);
    std::string getHeader(const std::string& s, std::string targ);

    bool splitPdu(const std::string& pdu, std::string& header, std::string& payload);
    [[noreturn]] void throwPacket(const std::string& layer, const std::string& reason) const;

    virtual ~Layer() = default;
    
    virtual void encapsulate(const PacketInfo& packetInfo, std::string sdu) = 0;
    virtual void decapsulate(const PacketInfo& packetInfo, std::string pdu) = 0;

protected:
    Layer* up_ = nullptr;
    Layer* down_ = nullptr;
    std::string printDecor1 = "Layer                      PDU\n-----------------           ---------------------------------------------------------------\n";
};