#include "PhysicalLayer.h"
#include <string>
#include <bitset>
#include <iostream>
#include <cstdlib>
#include <thread>

/**
 * @file PhysicalLayer.cpp
 * @brief Implementation av L1 – Fysisk.
 */

PhysicalLayer::PhysicalLayer(int port) : socket_(port) {udpPort_ = port;}

void PhysicalLayer::start() {
    running_ = true;
    // Ny tråd som kör receiveLoop på this
    listener_ = std::thread(&PhysicalLayer::receiveLoop, this);
}

PhysicalLayer::~PhysicalLayer() {
    // Be tråden sluta och vänta tills den gjort det. Socketen har 200 ms timeout,
    // så receiveLoop hinner se running_ == false inom kort.
    running_ = false;
    if (listener_.joinable()) listener_.join();   // annars anropas std::terminate
}

void PhysicalLayer::receiveLoop() {
    while (running_) {
        // Blockerar högst 200 ms (SO_RCVTIMEO i UdpSocket) och returnerar då tom sträng
        std::string data = socket_.receive();
        if (data.empty()) continue;                // timeout eller fel

        receiveBits(data);
    }
}

void PhysicalLayer::receiveBits(const std::string& bits) {
    // Mottagartråden och readfile (huvudtråden) kan komma hit samtidigt;
    // lagren ovanför (t.ex. fragmentbuffrarna i L4) är inte trådsäkra
    std::lock_guard<std::mutex> lock(receiveMutex_);
    std::cout << "============== Incoming frame ===================" << std::endl;
    try{
        decapsulate(PacketInfo{}, bits);           // bits -> text, skickas uppåt
    }
    catch (const PacketDropped&) {} // Throwpacket skriver ut.
}

void PhysicalLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    // Varje byte blir 8 tecken '0'/'1', mest signifikanta biten först (t.ex. 'S' -> "01010011")
    std::string pdu;
    pdu.reserve(sdu.size() * 8);
    for(unsigned char c : sdu){
        pdu += std::bitset<8>(c).to_string();
    }
    // Bitfel 
    if(packetInfo.flipBit){
        biterror(pdu);
    }
    for(size_t pos : packetInfo.flipPositions){
        if(pos < pdu.size()){
            pdu[pos] = (pdu[pos] == '1') ? '0' : '1';
            std::cout << "Flipped bit " << pos << std::endl;
        }
        else{
            std::cout << "Bit " << pos << " is outside the frame (" << pdu.size() << " bits)" << std::endl;
        }
    }
    printLayerStep("PhysicalLayer", pdu);
    std::cout << "=================================================" << std::endl;
    socket_.send(pdu, "255.255.255.255", udpPort_);   // alla noder på samma medium-port hör ramen
}

void PhysicalLayer::decapsulate(const PacketInfo&, std::string pdu){
    printLayerStep("PhysicalLayer", pdu);
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