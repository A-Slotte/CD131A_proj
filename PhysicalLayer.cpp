#include "PhysicalLayer.h"
#include <string>
#include <bitset>
#include <iostream>


void PhysicalLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    std::string pdu;
    pdu.reserve(sdu.size() * 8);
    for(unsigned char c : sdu){
        pdu += std::bitset<8>(c).to_string();
    }
    std::cout << "1: Physical:              " << pdu << std::endl;
    decapsulate(packetInfo, pdu);
}
void PhysicalLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    try{
        if(pdu.size() % 8 != 0){
            throwPacket("PhysicalLayer", "Bit stream length is not a multiple of 8");
        }

        std::string sdu;
        PacketInfo pack;
        sdu.reserve(pdu.size() / 8);
        for(size_t i = 0; i < pdu.size(); i+=8){
            std::string byteStr = pdu.substr(i, 8);
            if(byteStr.find_first_not_of("01") != std::string::npos){
                throwPacket("PhysicalLayer", "Bit stream contains characters other than 0 and 1");
            }
            sdu += static_cast<char>(std::bitset<8>(byteStr).to_ulong());
        }
        std::cout << "1: Physical: " << sdu;
        up_ -> decapsulate(pack, sdu);
    }
    catch(const PacketDropped&){
    }
}