#include "SessionLayer.h"
#include <string>
#include <iostream>

/**
 * @file SessionLayer.cpp
 * @brief Implementation av L5 – Session.
 */

void SessionLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    auto newPacketInfo = packetInfo;

    {
        //Låser trådem, endast en tråd kan komma åt och uppdatera sessions.
        //hålls inte medan paketet skickas ner i stacken. 
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

    // Ifall send tråden är inne i låsta blocket väntar mottagar tråden här.
    std::unique_lock<std::mutex> lock(sessionsMutex_);
    
    // En session identifieras av avsändaren och dess SID, isNew är sann om det är en ny post och falsk om det är en befintlig post.
    // try_emplace skapar posten med 0 om sessionen är ny, annars hämtas den befintliga.
    auto [entry, isNew] = receivedSeq_.try_emplace({packetInfo.src, sid}, 0);
    int& lastSeq = entry->second;
    if(isNew){
        std::cout << "SessionLayer: new session SID=" << sid << " from node " << packetInfo.src << std::endl;
    }
    // Lika eller lägre SEQ = dubblett eller ett gammalt paket som kommit för sent
    if(seq <= lastSeq){
        throwPacket("SessionLayer", "Duplicate or old SEQ " + std::to_string(seq));
    }
    // Lucka i SEQ = paket har tappats 
    // Första paketet i en ny session räknas inte som lucka.
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
    

/**
     * @brief Hämtar sessionen mot en mottagare, eller skapar en ny.
     *
     * En ny session får nästa lediga SID och börjar med SEQ 0
     * (första meddelandet får därmed SEQ 1). Anroparen måste hålla sessionsMutex_.
     *
     * @param peer Mottagarens nodadress.
     * @return Referens till sessionen; giltig så länge objektet lever.
     */
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

/**
     * @brief Tolkar ett SID- eller SEQ-värde som ett positivt heltal.
     *
     * Kräver bara siffror och högst 9 tecken, så att std::stoi aldrig kan
     * kasta ett undantag eller svämma över.
     *
     * @param text Värdet från headern.
     * @param out  Får talet om tolkningen lyckas.
     * @return true om @p text var ett giltigt tal.
     */
bool SessionLayer::parseNumber(const std::string& text, int& out) const{
    if(text.empty() || text.size() > 9 || text.find_first_not_of("0123456789") != std::string::npos){
        return false;
    }
    out = std::stoi(text);
    return true;
}
