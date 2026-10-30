#pragma once
#include <string>

/**
 * @file UdpSocket.h
 * @brief Tunn C++-klass runt en POSIX UDP-socket.
 */

/**
 * @brief UDP-socket som fungerar som det delade mediet för det fysiska lagret.
 *
 * Socketen binds till en port på alla nätverkskort och kan skicka broadcast.
 * Flera processer på samma dator kan binda samma port, så att alla noder
 * tar emot samma broadcast, som på en delad Ethernet-buss.
 *
 * receive() har en timeout på 200 ms, så att mottagartråden regelbundet kan
 * kontrollera om den ska avslutas.
 *
 * Använder POSIX-sockets och fungerar därför på Linux och macOS, inte Windows.
 */

class UdpSocket{
public:
    /**
     * @brief Skapar socketen, slår på broadcast och delad port, och binder den till @p port.
     *
     * Fel skrivs ut med perror men stoppar inte programmet.
     *
     * @param port UDP-porten att lyssna på (medium-porten).
     */
    UdpSocket(int port);

    /// Stänger socketen.
    ~UdpSocket();

    // En socket är en resurs i operativsystemet som bara får stängas en gång.
    // En kopia skulle stänga samma socket i sin destruktor, därför förbjuds kopiering.
    UdpSocket(const UdpSocket&) = delete; // förbjud kopiering
    UdpSocket& operator=(const UdpSocket&) = delete; // förbjud kopiering

    /**
     * @brief Skickar ett UDP-paket.
     * @param msg      Datan som skickas.
     * @param destIp   Mottagarens IPv4-adress
     * @param destPort Mottagarens UDP-port.
     */
    void send(const std::string& msg, const std::string& destIp, int destPort);

    /**
     * @brief Väntar högst 200 ms på ett UDP-paket.
     * @return Paketets innehåll (högst 2047 bytes), eller tom sträng vid timeout eller fel.
     */
    std::string receive();
private:
    int sock_;
};
