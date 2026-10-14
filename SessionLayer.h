#pragma once
#include "Layer.h"
#include <map>

class SessionLayer : public Layer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
protected:

    struct sessionInfo{
        int sqn_;
        int sid_;
    };
    void updateSession(const PacketInfo& packetInfo);
    bool validateSession(const PacketInfo& packetInfo);
    std::map<std::string, sessionInfo> sessions; // key: destination address
};