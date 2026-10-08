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
            decapsulate(packetInfo, fragments[i]);
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
    int headerIndex = 1;
    int headerIndexFrag = 1;
    int targetIndexFrag = 1;
    int targetIndex = 2;
    
    std::string fragHeader = getHeader(packet, "FRAG", headerIndexFrag, targetIndexFrag);
    size_t pos = fragHeader.find('/');
    int totalFrags = std::stoi(fragHeader.substr(pos + 1));


    if(totalFrags == 1){

        //up_ -> decapsulate();
    }
    else{
        fragmentKey key;
        key.sid = getHeader(packet, "SID", headerIndex, targetIndex);
        key.seq = getHeader(packet, "SEQ", headerIndex, targetIndex);
        auto it = fragmentBuffers_.find(key);
        if(it == fragmentBuffers_.end()){
            fragmentBuffer buffer;
            buffer.firstSeen = std::chrono::steady_clock::now();
            buffer.totalFrags = totalFrags;
            buffer.recvFrags.push_back(packet); // tillfälligt packet
        }
        else{
            fragmentBuffers_[key].firstSeen = std::chrono::steady_clock::now();
            fragmentBuffers_[key].recvFrags.push_back(packet);
            fragmentBuffers_[key].totalFrags += 1;
            if(fragmentBuffers_[key].totalFrags == fragmentBuffers_[key].recvFrags.size()){
                std::string message;
                for(size_t i = 0; i < fragmentBuffers_[key].recvFrags.size(); i++){
                    message += fragmentBuffers_[key].recvFrags[i];
                };
                std::cout << "Assembled fragments: " << message;
            }
        }

    }
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