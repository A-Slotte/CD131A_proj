#include "PhysicalLayer.h"
#include <string>
#include <bitset>
#include <iostream>


void PhysicalLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    std::string bits;
    bits.reserve(packet.size() * 8);
    for(unsigned char c : packet){
        bits += std::bitset<8>(c).to_string();
    }
    std::cout << "1: Physical:              " << bits << std::endl;
    decapsulate(packetInfo, bits);
}
void PhysicalLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){
    std::string out;
    out.reserve(packet.size() / 8);
    for(size_t i = 0; i < packet.size(); i+=8){
        std::string byteStr = packet.substr(i, 8);
        if(byteStr.find_first_not_of("01") != std::string::npos){
            //hantera felet;
        }
        out += static_cast<char>(std::bitset<8>(byteStr).to_ulong());
    }
    std::cout << "1: Physical: " << out;

}