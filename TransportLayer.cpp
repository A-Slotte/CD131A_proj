#include "TransportLayer.h"
#include <iostream>
#include <string>


void TransportLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    std::cout << packet <<"\n";
    std::vector<std::string> fragments;
    std::string msg = packetInfo.msgEncrypted;
    std::string sid = std::to_string(packetInfo.sid);
    
    if(checkMsgLength(msg)){
        fragments = fragMsg(msg, 20);
        size_t fragmentsLen = fragments.size();
        for(size_t i = 0;i < fragmentsLen; i++){
            std::string header = 
            "SPORT=" + packetInfo.src +
            ";DPORT=" + packetInfo.dest + 
            ";FRAG=" + std::to_string(i+1) +"/" +std::to_string(fragmentsLen) + 
            ";DATA=SID=" + std::to_string(packetInfo.sid) + 
            ";SEQ=" + std::to_string(packetInfo.seq) + 
            ";DATA=ENC=CAESAR:" + std::to_string(packetInfo.cryptkey) +
            ";DATA=" + fragments[i] +
            ":CRC" + std::to_string(fragments[i].length()) +
            "|ETX";
            std::cout << header << "\n";
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

    std::cout << "GETTED HEADER: " << msg;
}

void TransportLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){

}

bool TransportLayer::checkMsgLength(std::string msg){
    size_t maxSize = 20; //20 bytes
    if(msg.size() > 20){
        return true;
    }
    return false;
}

std::vector<std::string> TransportLayer::fragMsg(std::string& msg, size_t maxFragSize){
    std::vector<std::string> res;
    for(size_t i = 0; i < msg.size(); i += maxFragSize){
        size_t len = std::min(maxFragSize, msg.size() - i);
        res.push_back(msg.substr(i, len));
    };
    return res;
}

std::string TransportLayer::defragMsg(std::string& frag){
    return "asd";
} 

void TransportLayer::cleanStuckFrags(){

}