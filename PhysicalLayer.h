#pragma once
#include "Layer.h"
#include "UdpSocket.h"
#include <string>
#include <atomic>
#include <thread>
#include <mutex>

class PhysicalLayer : public Layer{
public:
    PhysicalLayer(int port);
    ~PhysicalLayer() override;

    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    void start();                    // startar mottagartråden
    void receiveBits(const std::string& bits);   // en bitström från mediet (UDP eller testfil)
private:
    void biterror(std::string& pdu);
    void receiveLoop();              // använder socket_ direkt, behöver ingen parameter
    int udpPort_;
    UdpSocket socket_;
    std::atomic<bool> running_{false};
    std::thread listener_;           // annat namn än metoden
    std::mutex receiveMutex_;        // en mottagen ram i taget genom stacken
};
