#include "TransportLayer.h"
#include <iostream>
#include <string>
#include <utility>

void TransportLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    std::vector<std::string> fragments;
    std::string sid = std::to_string(packetInfo.sid);
    std::string seq = std::to_string(packetInfo.seq);
    std::string header, payload;
    splitPdu(packet, header, payload);
    if(checkSduLength(payload)){
        fragments = fragmentSdu(payload, 20);
        size_t fragmentsLen = fragments.size();
        for(size_t i = 0;i < fragmentsLen; i++){
            std::string header = 
            "SPORT=" + packetInfo.src +
            ";DPORT=" + packetInfo.dest + 
            ";FRAG=" + std::to_string(i+1) +"/" +std::to_string(fragmentsLen) + 
            ";DATA=SID=" + std::to_string(packetInfo.sid) + 
            ";SEQ=" + std::to_string(packetInfo.seq) +";" +"DATA="+fragments[i] ;
            std::cout << "Layer 4: " << header << "\n";
        }
    }
    else {
        std::string header = 
        "SPORT=" + packetInfo.src +
        ";DPORT=" +packetInfo.dest + 
        ";FRAG="+ std::to_string(1) +"/" +std::to_string(1) + 
        ";DATA="+ packet;
        std::cout << header << "\n";
    }
}

void TransportLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){
    std::string header, payload;
    std::string sessionHeader, sessionPayload;
    try {
        if (!splitPdu(packet, header, payload)) {
            throw packet;
        }
    } catch (const std::string& malformedPacket) {
        std::cout << "Incorrect packet: " << malformedPacket << '\n';
        return;
    }
    try {
        if (!splitPdu(packet, sessionHeader, sessionPayload)) {
            throw packet;
        }
    } catch (const std::string& malformedPacket) {
        std::cout << "Incorrect packet: " << malformedPacket << '\n';
        return;
    }
    
    std::string fragmentHeader = getHeader(header, "FRAG");
    size_t slashPos = fragmentHeader.find('/');
    size_t totalFrags = std::stoi(fragmentHeader.substr(slashPos+1));
    
    std::string sid = getHeader(payload, "SID");
    std::string seq = getHeader(payload, "SEQ");

    if(totalFrags == 1){

        //up_ -> decapsulate();
    }
    else{
        fragmentKey key;
        key.seq = seq;
        key.sid = sid;
        auto it = fragmentBuffers_.find(key);
        if(it == fragmentBuffers_.end()){
            fragmentBuffer buffer;
            buffer.firstSeen = std::chrono::steady_clock::now();
            buffer.totalFrags = totalFrags;
            buffer.recvFrags.push_back(packet); // tillfälligt packet
            fragmentBuffers_.emplace(key, std::move(buffer));
        }
        else{
            auto& buffer = it->second;
            buffer.firstSeen = std::chrono::steady_clock::now();
            buffer.recvFrags.push_back(packet);
            if(buffer.totalFrags == buffer.recvFrags.size()){
                std::string message;
                for(size_t i = 0; i < buffer.recvFrags.size(); i++){
                    message += buffer.recvFrags[i];
                };
                std::cout << "Assembled fragments: " << message;
            }
        }
    }
}

bool TransportLayer::checkSduLength(std::string msg){
    size_t maxSize = 20; //20 bytes
    if(msg.size() > 20){
        return true;
    }
    return false;
}

std::vector<std::string> TransportLayer::fragmentSdu(std::string& packet, size_t maxFragSize){
    std::vector<std::string> res;
    for(size_t i = 0; i < packet.size(); i += maxFragSize){
        size_t len = std::min(maxFragSize, packet.size() - i);
        res.push_back(packet.substr(i, len));
    };
    return res;
}

std::string TransportLayer::defragPdu(std::string& frag){
    return "asd";
} 

void TransportLayer::cleanStuckFrags(){

}