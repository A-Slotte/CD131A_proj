#include "PresentationLayer.h"
#include <iostream>
#include <string>

void PresentationLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    std::string encrypted = encrypt(packetInfo);
    std::string data = "ENC=CAESAR:"+std::to_string(packetInfo.cryptkey)+";DATA="+encrypted+";CRC="+std::to_string(encrypted.length())+"|ETX";
    std::cout << "Layer 6: " << data <<"\n";
    down_->encapsulate(packetInfo, data);
}

void PresentationLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){
    
}

std::string PresentationLayer::encrypt(const PacketInfo& packet){
    std::string result = "";
    int key = packet.cryptkey;
    for(int i = 0; i< packet.msg.length(); i++){
        if('a' <= packet.msg[i] && packet.msg[i] <= 'z'){
            result += 'a' + (packet.msg[i] - 'a' + key) % 26;
        }
        else if('A' <= packet.msg[i] && packet.msg[i] <= 'Z'){
            result += 'A' + (packet.msg[i] - 'A' + key) % 26;
        }
        else{result += packet.msg[i];};

    }
    return result;
}

std::string PresentationLayer::decrypt(const PacketInfo& packet){
    std::string result = "";
    int key = packet.cryptkey;
    for(int i = 0; i< packet.msg.length(); i++){
        if('a' <= packet.msg[i] && packet.msg[i] <= 'z'){
            result += 'a' + (packet.msg[i] - 'a' - key + 26) % 26;
        }
        else if('A' <= packet.msg[i] && packet.msg[i] <= 'Z'){
            result += 'A' + (packet.msg[i] - 'A' - key + 26) % 26;
        }
        else{result += packet.msg[i];};

    }
    return result;
}