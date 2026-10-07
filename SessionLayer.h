#pragma once
#include "NetLayer.h"
#include <map>

class SessionLayer : public NetLayer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;
protected:

    struct sessionInfo{
        int sqn_;
        int sid_;
    };  
    void updateSession(const PacketInfo& packetInfo);
    bool validateSession(const PacketInfo& packetInfo, int SEQ);
    struct sessionInfo;
    std::map<std::string, sessionInfo> sessions; // key : DST_ADDRESS, val : sid, seq:
};