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
    packet+
    ";CRC="+xorSum+"|ETX";    
    std::cout << "2: Datalink:              " << pdu << "\n";
    down_ -> encapsulate(packetInfo, packet);
}
void DatalinkLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){

}

std::string DatalinkLayer::xorChecksum(std::string sdu){
    uint8_t checksum;
    for(unsigned char byte : sdu){
        checksum ^= byte;
    }
    std::ostringstream out;
    out << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(checksum);
    return out.str();
}