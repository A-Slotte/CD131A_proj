#include "NetworkLayer.h"
#include <iostream>

void NetworkLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){


    std::string pdu = "SRC=" + node + ";" + "DST=" + packetInfo.dst +";TTL=" + packetInfo.ttl + ";DATA=" + sdu;
    printLayerStep("NetworkLayer", pdu);
    down_ -> encapsulate(packetInfo, pdu);
    //Ska ttl minskas vid send?
}
void NetworkLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    std::string header, sdu;
    if(!splitPdu(pdu, header, sdu)){
        throwPacket("NetworkLayer", "Malformed network PDU");
    }

    std::string dst = getHeader(header, "DST");
    int ttl = stoi(getHeader(header, "TTL"));

    if(!checkDst(dst)){
        throwPacket("NetworkLayer", "Destination IP does not match this host");
    }
    if(!updateTtl(ttl)){
        throwPacket("NetworkLayer", "Packet TTL has expired");
    }

    up_ -> decapsulate(packetInfo, sdu);
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
    if(node == dst){
        return true;
    }
    return false;
};
