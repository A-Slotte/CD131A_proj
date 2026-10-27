#include "PhysicalLayer.h"
#include <string>
#include <bitset>
#include <iostream>
#include <cstdlib>
#include <thread>


PhysicalLayer::PhysicalLayer(int port) : socket_(port) {udpPort_ = port;}

void PhysicalLayer::start() {
    running_ = true;
    listener_ = std::thread(&PhysicalLayer::receiveLoop, this);
}

PhysicalLayer::~PhysicalLayer() {
    running_ = false;
    if (listener_.joinable()) listener_.join();   // annars anropas std::terminate
}

void PhysicalLayer::receiveLoop() {
    while (running_) {
        std::string data = socket_.receive();
        if (data.empty()) continue;                // timeout eller fel

        try{
            std::cout << data << std::endl;
            decapsulate(PacketInfo{}, data);           // bits -> text, skickas uppåt
        }
        catch (const PacketDropped&) {}
    }
}

void PhysicalLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    std::string pdu;
    pdu.reserve(sdu.size() * 8);
    for(unsigned char c : sdu){
        pdu += std::bitset<8>(c).to_string();
    }
    printLayerStep("PhysicalLayer", pdu);
    std::cout << "=================================================" << std::endl;
    if(packetInfo.flipBit){
        biterror(pdu);
    }
    socket_.send(pdu, "255.255.255.255", 9000);
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
        printLayerStep("PhysicalLayer", sdu);
        up_ -> decapsulate(pack, sdu);    
    }
    catch(const PacketDropped&){
    }
}

void PhysicalLayer::biterror(std::string& pdu){
    int rNumb = rand() % pdu.size();
    char c = pdu[rNumb];
    if(c == '1'){
        pdu[rNumb] = '0';
    }
    else if(c == '0'){
        pdu[rNumb] = '1';
    }
}