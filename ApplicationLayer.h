#pragma once
#include <iostream>
#include <string>
#include "Layer.h"
#include <map>

class ApplicationLayer : public Layer {
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    void run();
    void handleCommand(const std::string& input);
private:
    std::string divider = "========================================";
};