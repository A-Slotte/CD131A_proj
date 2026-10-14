#include "SessionLayer.h"
#include <string>
#include <iostream>


//SID=<sessions-id>;SEQ=<sekvensnummer>;DATA=<L6-PDU>

void SessionLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    updateSession(packetInfo);

    const auto& session = sessions.at(packetInfo.dst);
    auto newPacketInfo = packetInfo;
    newPacketInfo.seq = session.sqn_;
    newPacketInfo.sid = session.sid_;


    std::string pdu = "SID=" + std::to_string(session.sid_) + ";SEQ=" + std::to_string(session.sqn_) + ";DATA=" + sdu + "\n";
    std::cout << "5: Session:           " << pdu;
    down_ -> encapsulate(newPacketInfo, pdu);
};

void SessionLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    if(!validateSession(packetInfo)){
        throwPacket("Sessionlayer: ", "Session seq is not valid");
    }
    updateSession(packetInfo);
    std::string header, sdu;
    if(!splitPdu(pdu, header, sdu)){
        throwPacket("SessionLayer", "Malformed session PDU");
    }
    up_ -> decapsulate(packetInfo, sdu);
};

void SessionLayer::updateSession(const PacketInfo& packetInfo){
    auto session = sessions.find(packetInfo.dst);
    if(session == sessions.end()){
        int nextSid = 1;
        for(const auto& entry : sessions){
            if(entry.second.sid_ >= nextSid){
                nextSid = entry.second.sid_ + 1;
            }
        }
        sessions.emplace(packetInfo.dst, sessionInfo{1, nextSid});
    }
    else {
        ++session->second.sqn_;
    }
};

bool SessionLayer::validateSession(const PacketInfo& packetInfo){
    const auto session = sessions.find(packetInfo.dst);
    if(session == sessions.end()){
        return true;
    }

    return session->second.sqn_ == packetInfo.seq - 1;
}
