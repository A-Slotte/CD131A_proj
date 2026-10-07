#pragma once
#include <string>
#include "PacketInfo.h"
#include <vector>
#include <sstream>

class NetLayer {
public:
    NetLayer() = default;
    void connect(NetLayer* up, NetLayer* down);
    std::vector<std::string> split(const std::string& is, char delim);
    std::string getHeader(const std::string& s, std::string targ);

    virtual ~NetLayer() = default;

    virtual void encapsulate(const PacketInfo& packetInfo, std::string packet) = 0;
    virtual void decapsulate(const PacketInfo& packetInfo, std::string packet) = 0;

protected:

    NetLayer* up_ = nullptr;
    NetLayer* down_ = nullptr;
};