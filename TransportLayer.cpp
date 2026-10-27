#include "TransportLayer.h"
#include <iostream>
#include <string>
#include <utility>

// Enligt specens format (testfil D1/D2) blir varje fragment en egen L5-PDU:
// SEQ räknas upp per fragment och L6-headern (ENC=...) upprepas i varje fragment.
void TransportLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    lastSegmentCount_ = 1;
    std::string sessionHeader, presentationPdu, presentationHeader, cipher;
    const bool splittable = splitPdu(sdu, sessionHeader, presentationPdu)
        && splitPdu(presentationPdu, presentationHeader, cipher);

    if(splittable && cipher.size() > maxFragmentSize){
        std::vector<std::string> fragments = fragmentSdu(cipher, maxFragmentSize);
        size_t fragmentsLen = fragments.size();
        lastSegmentCount_ = fragmentsLen;
        for(size_t i = 0;i < fragmentsLen; i++){
            std::string pdu =
            "SPORT=" + port_ +
            ";DPORT=" + packetInfo.DPORT +
            ";FRAG=" + std::to_string(i+1) +"/" +std::to_string(fragmentsLen) +
            ";DATA=SID=" + std::to_string(packetInfo.sid) +
            ";SEQ=" + std::to_string(packetInfo.seq + static_cast<int>(i)) +
            ";DATA=" + presentationHeader + "DATA=" + fragments[i];
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
    const std::string seqText = getHeader(payload, "SEQ");
    if(sid.empty() || seqText.empty() || seqText.size() > 9
        || (sid + seqText).find_first_not_of("0123456789") != std::string::npos){
        throwPacket("TransportLayer", "Fragment has missing or invalid SID/SEQ");
    }
    // Fragment i har SEQ = första SEQ + (i - 1), så alla fragment i ett meddelande får samma startSeq
    const int startSeq = std::stoi(seqText) - static_cast<int>(fragmentNumber - 1);
    if(startSeq < 1){
        throwPacket("TransportLayer", "Fragment SEQ does not match its FRAG number");
    }
    const std::string message = defragPdu(sessionPayload, fragmentNumber, totalFrags, packetInfo.src, std::to_string(startSeq), sid);
    if(message.empty()){
        return;
    }

    packetInfo.seq = startSeq;
    packetInfo.sid = std::stoi(sid);
    packetInfo.fragments = static_cast<int>(totalFrags);
    up_ -> decapsulate(packetInfo, message);
}

bool TransportLayer::checkDport(std::string port){
    if(port_ == port){
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
    // Varje fragment bär L6-headern (ENC=...) + en bit av chiffertexten
    std::string presentationHeader, chunk;
    if(!splitPdu(fragment, presentationHeader, chunk)){
        throwPacket("TransportLayer", "Fragment has no presentation payload");
    }

    fragmentKey key(src, seq, sid);
    auto it = fragmentBuffers_.find(key);
    if(it == fragmentBuffers_.end()){
        fragmentBuffer buffer;
        buffer.firstSeen = std::chrono::steady_clock::now();
        buffer.totalFrags = totalFrags;
        buffer.presentationHeader = presentationHeader;
        buffer.recvFrags.resize(totalFrags);
        buffer.receivedFrags.resize(totalFrags, false);
        it = fragmentBuffers_.emplace(key, std::move(buffer)).first;
    }

    auto& buffer = it->second;
    if(buffer.totalFrags != totalFrags){
        throwPacket("TransportLayer", "Fragment count changed within the same message");
    }
    const size_t fragmentIndex = fragmentNumber - 1;
    if(!buffer.receivedFrags[fragmentIndex]){
        buffer.recvFrags[fragmentIndex] = chunk;
        buffer.receivedFrags[fragmentIndex] = true;
        ++buffer.receivedCount;
    }
    std::cout << "Fragment " << fragmentNumber << "/" << totalFrags << " buffered ("
              << buffer.receivedCount << "/" << totalFrags << " received)" << std::endl;
    if(buffer.receivedCount < buffer.totalFrags){
        return "";
    }

    // Alla fragment finns: en L5-PDU med första SEQ och hela chiffertexten i ordning
    std::string message = "SID="+sid+";SEQ="+seq+";DATA="+buffer.presentationHeader+"DATA=";
    for(const auto& part : buffer.recvFrags){
        message += part;
    }
    fragmentBuffers_.erase(it);
    std::cout << "Assembled fragments: " << message << std::endl;
    return message;
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