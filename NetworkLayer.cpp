#include "NetworkLayer.h"
#include <iostream>

void NetworkLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){


    std::string pdu = "SRC=" + packetInfo.src + ";" + "DST=" + packetInfo.dst +";TTL=" + packetInfo.ttl + ";DATA=" + packet;
    std::cout << "3: Network:           " << pdu << "\n";
    down_ -> encapsulate(packetInfo, pdu);
    //Ska ttl minskas vid send?
}
void NetworkLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){
    std::string header, payload;
    splitPdu(packet, header, payload);
    std::string src = getHeader(header, "SRC");
    std::string dst = getHeader(header, "DST");
    int ttl = stoi(getHeader(header, "TTL"));

    //Ska det reroutas?
    if(checkDst(dst) && updateTtl(ttl)){
        up_ -> decapsulate(packetInfo, payload);
    }
    else{
    
        return;
    }
}

bool NetworkLayer::updateTtl(int ttl){
    if(ttl <= 0){
        //reroute
        return false;
    }
    else {
        ttl -= 1;
        return true;
    }
};

bool NetworkLayer::checkDst(const std::string& dst){
    if(ip == dst){
        return true;
    }
    return false;
};

