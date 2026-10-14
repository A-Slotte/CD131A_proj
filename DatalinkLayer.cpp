#include "DatalinkLayer.h"
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <iostream>
//STX|MAC_SRC=<id>;MAC_DST=<id>;LEN=<n>;DATA=<L3-PDU>;CRC=<hex>|ETX
void DatalinkLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    std::string xorSum = xorChecksum(packet);
    int len = packet.size();
    std::string pdu = 
    "STX|MAC_SRC="+packetInfo.mac_src+
    ";MAC_DST="+packetInfo.mac_dst+
    ";LEN="+std::to_string(len)+ 
    ";DATA="+packet+
    ";CRC="+xorSum+"|ETX";    
    std::cout << "2: Datalink:              " << pdu << "\n";
    down_ -> encapsulate(packetInfo, pdu);
}
void DatalinkLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){
    int etxPos = packet.rfind("|ETX");
    if(packet.substr(0, 4) != "STX|" || packet.substr(etxPos) != "|ETX"){
        return;
    }

    PacketInfo pduInfo;
    std::string header, pdu, mac_src, mac_dst, len, crc, s;
    size_t crcPos = packet.rfind(";CRC=");
    size_t start = packet.find("MAC_SRC=");

    s = packet.substr(start, crcPos - start);
    crc = packet.substr(crcPos+ 5, etxPos - (crcPos + 5));
    
    splitPdu(s, header, pdu);
    mac_src = getHeader(header, "MAC_SRC");
    mac_dst = getHeader(header, "MAC_DST");

    if(mac_src != "AA" || mac_dst != "FF"){
        return;
    }

    len = getHeader(header, "LEN");
    std::string newcrc = xorChecksum(pdu);
    if(crc != newcrc){
        return;
    }
    pduInfo.mac_dst = mac_dst;
    pduInfo.mac_src = mac_src;

    up_ -> decapsulate(pduInfo, pdu);
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