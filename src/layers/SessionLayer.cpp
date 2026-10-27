#include "SessionLayer.h"
#include <string>
#include <iostream>


//SID=<sessions-id>;SEQ=<sekvensnummer>;DATA=<L6-PDU>

void SessionLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    auto newPacketInfo = packetInfo;
    {
        std::lock_guard<std::mutex> lock(sessionsMutex_);
        auto& session = getSession(packetInfo.dst);
        ++session.sendSeq_;
        newPacketInfo.seq = session.sendSeq_;
        newPacketInfo.sid = session.sid_;
    }

    std::string pdu = "SID=" + std::to_string(newPacketInfo.sid) + ";SEQ=" + std::to_string(newPacketInfo.seq) + ";DATA=" + sdu;
    printLayerStep("SessionLayer", pdu);
    down_ -> encapsulate(newPacketInfo, pdu);

    // Fragment får egna SEQ-nummer (seq, seq+1, ...), så nästa meddelande börjar efter dem
    const size_t segments = down_ -> segmentsInLastSend();
    if(segments > 1){
        std::lock_guard<std::mutex> lock(sessionsMutex_);
        getSession(packetInfo.dst).sendSeq_ += static_cast<int>(segments) - 1;
    }
};

void SessionLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    printLayerStep("SessionLayer", pdu);
    std::string header, sdu;
    if(!splitPdu(pdu, header, sdu)){
        throwPacket("SessionLayer", "Malformed session PDU");
    }

    int sid, seq;
    if(!parseNumber(getHeader(header, "SID"), sid) || !parseNumber(getHeader(header, "SEQ"), seq)){
        throwPacket("SessionLayer", "Missing or invalid SID/SEQ");
    }

    std::unique_lock<std::mutex> lock(sessionsMutex_);
    // En session identifieras av avsändaren och dess SID
    auto [entry, isNew] = receivedSeq_.try_emplace({packetInfo.src, sid}, 0);
    int& lastSeq = entry->second;
    if(isNew){
        std::cout << "SessionLayer: new session SID=" << sid << " from node " << packetInfo.src << std::endl;
    }
    if(seq <= lastSeq){
        throwPacket("SessionLayer", "Duplicate or old SEQ " + std::to_string(seq));
    }
    if(!isNew && seq > lastSeq + 1){
        std::cout << "SessionLayer: " << seq - lastSeq - 1
                  << " packet(s) from " << packetInfo.src << " missing" << std::endl;
    }
    lastSeq = seq + packetInfo.fragments - 1;   // synka om även efter en lucka; fragment har egna SEQ
    lock.unlock();

    auto newPacketInfo = packetInfo;
    newPacketInfo.sid = sid;
    newPacketInfo.seq = seq;
    up_ -> decapsulate(newPacketInfo, sdu);
};

SessionLayer::sessionInfo& SessionLayer::getSession(const std::string& peer){
    auto session = sessions.find(peer);
    if(session != sessions.end()){
        return session->second;
    }
    int nextSid = 1;
    for(const auto& entry : sessions){
        if(entry.second.sid_ >= nextSid){
            nextSid = entry.second.sid_ + 1;
        }
    }
    return sessions.emplace(peer, sessionInfo{nextSid, 0}).first->second;
};

bool SessionLayer::parseNumber(const std::string& text, int& out) const{
    if(text.empty() || text.size() > 9 || text.find_first_not_of("0123456789") != std::string::npos){
        return false;
    }
    out = std::stoi(text);
    return true;
}
