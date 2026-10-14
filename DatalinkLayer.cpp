#include "DatalinkLayer.h"
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <iostream>
//STX|MAC_SRC=<id>;MAC_DST=<id>;LEN=<n>;DATA=<L3-PDU>;CRC=<hex>|ETX
void DatalinkLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    std::string xorSum = xorChecksum(sdu);
    int len = sdu.size();
    std::string pdu = 
    "STX|MAC_SRC="+packetInfo.mac_src+
    ";MAC_DST="+packetInfo.mac_dst+
    ";LEN="+std::to_string(len)+ 
    ";DATA="+sdu+
    ";CRC="+xorSum+"|ETX";    
    std::cout << "2: Datalink:              " << pdu << "\n";
    down_ -> encapsulate(packetInfo, pdu);
}
void DatalinkLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    const size_t etxPos = pdu.rfind("|ETX");
    const size_t crcPos = pdu.rfind(";CRC=");
    const size_t start = pdu.find("MAC_SRC=");
    if(pdu.substr(0, 4) != "STX|" || etxPos == std::string::npos
        || crcPos == std::string::npos || start == std::string::npos
        || pdu.substr(etxPos) != "|ETX" || crcPos >= etxPos){
        throwPacket("DatalinkLayer", "Malformed frame");
    }

    PacketInfo pduInfo;
    std::string header, sdu, mac_src, mac_dst, len, crc, frameHeader;

    frameHeader = pdu.substr(start, crcPos - start);
    crc = pdu.substr(crcPos+ 5, etxPos - (crcPos + 5));
    
    if(!splitPdu(frameHeader, header, sdu)){
        throwPacket("DatalinkLayer", "Frame has no payload separator");
    }
    mac_src = getHeader(header, "MAC_SRC");
    mac_dst = getHeader(header, "MAC_DST");

    if(mac_src != myMac || mac_dst != "FF"){
        throwPacket("DatalinkLayer", "Frame MAC addresses do not match");
    }

    len = getHeader(header, "LEN");
    std::string newcrc = xorChecksum(sdu);
    if(crc != newcrc){
        throwPacket("DatalinkLayer", "Frame checksum does not match");
    }
    pduInfo.mac_dst = mac_dst;
    pduInfo.mac_src = mac_src;

    up_ -> decapsulate(pduInfo, sdu);
}

std::string DatalinkLayer::xorChecksum(const std::string& sdu){
    uint8_t checksum = 0;
    for(unsigned char byte : sdu){
        checksum ^= byte;
    }
    std::ostringstream out;
    out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(checksum);
    return out.str();
}