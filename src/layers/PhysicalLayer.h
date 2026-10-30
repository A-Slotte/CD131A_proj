#pragma once
#include "Layer.h"
#include "net/UdpSocket.h"
#include <string>
#include <atomic>
#include <thread>
#include <mutex>

/**
 * @file PhysicalLayer.h
 * @brief L1 – Fysiska lagret.
 */

/**
 * @brief L1 – Fysisk: överföring av en rå bitström.
 *
 * Motsvarar kabel, fiber eller radio i verkligheten. Ramen från L2 görs om
 * till en bitsträng (8 tecken '0'/'1' per byte, UTF-8) och skickas som
 * UDP-broadcast, precis som på en delad Ethernet-buss. Det är här bitfel introduceras, efter att
 * 
 * L2 har räknat ut checksumman, som ett riktigt fel på mediet.
 *
 * Mottagning sker i en egen tråd så att användaren kan skriva kommandon samtidigt.
 */
class PhysicalLayer : public Layer{
public:
    /**
     * @brief Öppnar UDP-socketen på mediets port.
     * @param port Medium-porten (UDP) som alla noder delar, inte den logiska porten i L4.
     */
    PhysicalLayer(int port);
    /// Destructor. Stoppar och väntar in mottagartråden.
    ~PhysicalLayer() override;

    /**
     * @brief Gör om ramen till bitar, lägger in eventuella bitfel och broadcastar den.
     * @param packetInfo @c flipBit ger ett slumpat bitfel, @c flipPositions flippar valda bitar.
     * @param sdu        Ramen från datalänklagret.
     */
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;

    /**
     * @brief Gör om en bitström till tecken (8 bitar -> 1 byte) och skickar ramen till L2.
     *
     * Kastade paket fångas här, så att ett trasigt paket aldrig stoppar mottagningen.
     *
     * @param packetInfo Används inte
     * @param pdu        Bitsträngen
     */
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    
    void start();  // startar mottagartråden

    /**
     * @brief Tar emot en bitström och skickar den uppåt genom stacken.
     *
     * Används både av mottagartråden (UDP)
     *
     * @param bits Bitsträngen
     */
    void receiveBits(const std::string& bits);

private:
    /// Flippar en slumpvis vald bit i bitsträngen.
    void biterror(std::string& pdu);

    /// Mottagartrådens loop: läser från socketen tills running_ blir false.
    void receiveLoop();   

    int udpPort_;
    UdpSocket socket_;
    std::atomic<bool> running_{false}; /// Atomär: läses av mottagartråden, ändras av huvudtråden.
    std::thread listener_;           // Mottagartråden
    std::mutex receiveMutex_;        // en mottagen ram i taget genom stacken
};
