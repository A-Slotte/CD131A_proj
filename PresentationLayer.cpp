#include "PresentationLayer.h"
#include <iostream>
#include <string>
#include <stdexcept>

void PresentationLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    auto newPacketInfo = packetInfo;
    newPacketInfo.msg = sdu;
    std::string encrypted = encrypt(newPacketInfo);
    newPacketInfo.msgEncrypted = encrypted;
    std::string pdu = "ENC=CAESAR:"+std::to_string(packetInfo.cryptkey)+";DATA="+encrypted;

    printLayerStep("PresentationLayer", pdu);
    down_->encapsulate(newPacketInfo, pdu);
}

void PresentationLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    printLayerStep("PresentationLayer", pdu);
    std::string header;
    std::string sdu;
    if(!splitPdu(pdu, header, sdu)){
        throwPacket("PresentationLayer", "Malformed presentation PDU");
    }

    const std::string encryption = getHeader(header, "ENC");
    const char cipherPrefix[] = "CAESAR:";
    if(encryption.rfind(cipherPrefix, 0) != 0){
        throwPacket("PresentationLayer", "Unsupported or missing encryption method");
    }

    const std::string keyText = encryption.substr(sizeof(cipherPrefix) - 1);
    if(keyText.empty() || keyText.find_first_not_of("0123456789") != std::string::npos){
        throwPacket("PresentationLayer", "Invalid Caesar cipher key");
    }

    PacketInfo decryptedPacketInfo = packetInfo;

    decryptedPacketInfo.cryptkey = std::stoi(keyText);
    

    decryptedPacketInfo.msgEncrypted = sdu;
    decryptedPacketInfo.msg = sdu;
    decryptedPacketInfo.msg = decrypt(decryptedPacketInfo);
    const std::string decryptedSdu = decryptedPacketInfo.msg;
    up_->decapsulate(decryptedPacketInfo, decryptedSdu);
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