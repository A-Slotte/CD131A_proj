#include "TransportLayer.h"
#include <iostream>
#include <string>


void TransportLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    std::string msg = getHeader(packet, "DATA");
    std::cout << "GETTED HEADER: " << msg;
}

void TransportLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){

}

std::vector<std::string> TransportLayer::fragPacket(std::string& frag){

}

std::string TransportLayer::defragPacket(std::string& frag){

} 

void TransportLayer::cleanFrags(){

}