#include "SessionLayer.h"
#include <string>
#include <iostream>


//SID=<sessions-id>;SEQ=<sekvensnummer>;DATA=<L6-PDU>

void SessionLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    updateSession(packetInfo);
    auto session = sessions[packetInfo.dst]; 
    auto newPacketInfo = packetInfo;
    newPacketInfo.seq = session.sqn_;
    newPacketInfo.sid = session.sid_;


    std::string data = "SID=" + std::to_string(session.sid_) + ";SEQ=" + std::to_string(session.sqn_) + ";DATA=" + packet + "\n";
    std::cout << "5: Session:           " << data;
    down_ -> encapsulate(newPacketInfo, data);
};

void SessionLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){

};

// kolla va fan if() på rad 23 gör.
void SessionLayer::updateSession(const PacketInfo& packetInfo){
    if(!sessions.count(packetInfo.dst)){
        int a = 1;
        for(const auto& [address, sInfo] : sessions ){
            if(sInfo.sid_ > a ){// <------- denna
                a = sInfo.sid_;
            }
        }
        sessions[packetInfo.dst] = sessionInfo{a, 1};
    }
    else {
        auto sInfo = sessions[packetInfo.dst];
        sInfo.sqn_ += 1;
        sessions[packetInfo.dst] = sInfo;
    }
};

bool SessionLayer::validateSession(const PacketInfo& packetInfo, int SEQ){
    if(sessions.count(packetInfo.dst)){
        if(sessions[packetInfo.dst].sqn_ == SEQ){
            return true;
        }
        else {
            return false;
        }
    }
    return true;
};
