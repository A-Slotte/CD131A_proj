#include "TransportLayer.h"
#include <iostream>
#include <string>
#include <utility>

void TransportLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    std::vector<std::string> fragments;
    std::string sid = std::to_string(packetInfo.sid);
    std::string seq = std::to_string(packetInfo.seq);
    std::string header, payload;
    splitPdu(sdu, header, payload);
    if(checkSduLength(payload)){
        fragments = fragmentSdu(payload, 20);
        size_t fragmentsLen = fragments.size();
        for(size_t i = 0;i < fragmentsLen; i++){
            std::string pdu =
            "SPORT=" + port_ +
            ";DPORT=" + packetInfo.DPORT + 
            ";FRAG=" + std::to_string(i+1) +"/" +std::to_string(fragmentsLen) + 
            ";DATA=SID=" + std::to_string(packetInfo.sid) + 
            ";SEQ=" + std::to_string(packetInfo.seq) +";" +"DATA="+fragments[i] ;
            printLayerStep("TransportLayer", pdu);
            down_ -> encapsulate(packetInfo, pdu);
        }
    }
    else {
        std::string pdu =
        "SPORT=" + port_ +
        ";DPORT=" +packetInfo.DPORT + 
        ";FRAG="+ std::to_string(1) +"/" +std::to_string(1) + 
        ";DATA="+ sdu;
        printLayerStep("TransportLayer", pdu);
        down_ -> encapsulate(packetInfo, pdu);
    }
}

void TransportLayer::decapsulate(const PacketInfo& packetInfo, std::string pdu){
    printLayerStep("TransportLayer", pdu);
    cleanStuckFrags();

    std::string transportHeader, payload, sessionPayload;
    if(!splitIncomingPacket(pdu, transportHeader, payload, sessionPayload)){
        throwPacket("TransportLayer", "Malformed transport or session PDU");
    }

    PacketInfo newPacketInfo = packetInfo;
    const std::string destinationPort = getHeader(transportHeader, "DPORT");
    if(!checkDport(destinationPort)){
        throwPacket("TransportLayer", "Destination port does not match this layer");
    }

    newPacketInfo.DPORT = destinationPort;
    newPacketInfo.SPORT = getHeader(transportHeader, "SPORT");

    const std::string fragmentHeader = getHeader(transportHeader, "FRAG");
    const size_t slashPos = fragmentHeader.find('/');
    if(slashPos == std::string::npos){
        throwPacket("TransportLayer", "Malformed fragment header");
    }
    const size_t fragmentNumber = std::stoi(fragmentHeader.substr(0, slashPos));
    const size_t totalFrags = std::stoi(fragmentHeader.substr(slashPos + 1));
    if(fragmentNumber == 0 || fragmentNumber > totalFrags){
        throwPacket("TransportLayer", "Fragment number is outside the declared range");
    }
    if(totalFrags == 1){
        up_ -> decapsulate(newPacketInfo, payload);
        return;
    }

    decapsulateFragments(newPacketInfo, payload, sessionPayload, fragmentNumber, totalFrags);
}

bool TransportLayer::splitIncomingPacket(
    const std::string& pdu,
    std::string& transportHeader,
    std::string& payload,
    std::string& sessionPayload
){
    std::string sessionHeader;
    return splitPdu(pdu, transportHeader, payload)
        && splitPdu(payload, sessionHeader, sessionPayload);
}

void TransportLayer::decapsulateFragments(
    PacketInfo& packetInfo,
    const std::string& payload,
    const std::string& sessionPayload,
    size_t fragmentNumber,
    size_t totalFrags
){
    const std::string sid = getHeader(payload, "SID");
    const std::string seq = getHeader(payload, "SEQ");
    const std::string message = defragPdu(sessionPayload, fragmentNumber, totalFrags, packetInfo.src, seq, sid);
    if(message.empty()){
        return;
    }

    packetInfo.seq = std::stoi(seq);
    packetInfo.sid = std::stoi(sid);
    up_ -> decapsulate(packetInfo, message);
}

bool TransportLayer::checkDport(std::string port){
    if(port_ == port){
        return true;
    }
    return false;
}

bool TransportLayer::checkSduLength(std::string msg){
    size_t maxSize = 20; //20 bytes
    if(msg.size() > 20){
        return true;
    }
    return false;
}

std::vector<std::string> TransportLayer::fragmentSdu(std::string& packet, size_t maxFragSize){
    std::vector<std::string> res;
    for(size_t i = 0; i < packet.size(); i += maxFragSize){
        size_t len = std::min(maxFragSize, packet.size() - i);
        res.push_back(packet.substr(i, len));
    };
    return res;
}

std::string TransportLayer::defragPdu(
    const std::string& fragment,
    size_t fragmentNumber,
    size_t totalFrags,
    const std::string& src,
    const std::string& seq,
    const std::string& sid
){
    fragmentKey key(src, seq, sid);
    auto it = fragmentBuffers_.find(key);
    if(it == fragmentBuffers_.end()){
        fragmentBuffer buffer;
        buffer.firstSeen = std::chrono::steady_clock::now();
        buffer.totalFrags = totalFrags;
        buffer.recvFrags.resize(totalFrags);
        buffer.receivedFrags.resize(totalFrags, false);
        buffer.recvFrags[fragmentNumber - 1] = fragment;
        buffer.receivedFrags[fragmentNumber - 1] = true;
        buffer.receivedCount = 1;
        fragmentBuffers_.emplace(key, std::move(buffer));
        return "";
    }
    else{
        auto& buffer = it->second;
        if(buffer.totalFrags != totalFrags){
            throwPacket("TransportLayer", "Fragment count changed within the same message");
        }
        const size_t fragmentIndex = fragmentNumber - 1;
        if(!buffer.receivedFrags[fragmentIndex]){
            buffer.recvFrags[fragmentIndex] = fragment;
            buffer.receivedFrags[fragmentIndex] = true;
            ++buffer.receivedCount;
        }
        if(buffer.receivedCount == buffer.totalFrags){
            std::string message = "SID="+sid+";SEQ="+seq+";DATA=";
            for(size_t i = 0; i < buffer.recvFrags.size(); i++){
                message += buffer.recvFrags[i];
            };
            fragmentBuffers_.erase(it);
            std::cout << "Assembled fragments: " << message << std::endl;
            return message;
        }
        return "";
    }
}

void TransportLayer::cleanStuckFrags(){
    constexpr auto fragmentTimeout = std::chrono::seconds(30);
    const auto now = std::chrono::steady_clock::now();

    for(auto it = fragmentBuffers_.begin(); it != fragmentBuffers_.end();){
        if(now - it->second.firstSeen >= fragmentTimeout){
            it = fragmentBuffers_.erase(it);
        }
        else{
            ++it;
        }
    }
}