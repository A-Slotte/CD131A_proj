#include "NetworkLayer.h"
#include <iostream>

/**
 * @file NetworkLayer.cpp
 * @brief Implementation av L3 – Nätverk.
 */
void NetworkLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){

    // TTL:s startvärde, det minskas först när en nod vidarebefordrar paketet
    std::string pdu = "SRC=" + node + ";" + "DST=" + packetInfo.dst +";TTL=" + packetInfo.ttl + ";DATA=" + sdu;
    printLayerStep("NetworkLayer", pdu);
    down_ -> encapsulate(packetInfo, pdu);
}
void NetworkLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    printLayerStep("NetworkLayer", pdu);
    std::string header, sdu;
    if(!splitPdu(pdu, header, sdu)){
        throwPacket("NetworkLayer", "Malformed network PDU");
    }
    // Alla noder hör alla ramar (broadcast på L2), så paket till andra noder kastas här.
    std::string dst = getHeader(header, "DST");
    int ttl = stoi(getHeader(header, "TTL"));

    if(!checkDst(dst)){
        throwPacket("NetworkLayer", "Destination IP does not match this host");
    }
    if(!updateTtl(ttl)){
        throwPacket("NetworkLayer", "Packet TTL has expired");
    }

    // Avsändar- och mottagaradress skickas uppåt som parametrar
    PacketInfo newPacketInfo = packetInfo;
    newPacketInfo.src = getHeader(header, "SRC");
    newPacketInfo.dst = dst;
    up_ -> decapsulate(newPacketInfo, sdu);
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
