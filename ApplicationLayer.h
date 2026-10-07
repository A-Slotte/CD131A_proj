#pragma once
#include <iostream>
#include <string>
#include "NetLayer.h"
#include <map>

class ApplicationLayer : public NetLayer {
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void run();
    void handleCommand(const std::string& input);
private:
    std::string divider = "========================================";
};