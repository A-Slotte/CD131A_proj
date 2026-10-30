#include "UdpSocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <string>

/**
 * @file UdpSocket.cpp
 * @brief Implementation av UdpSocket med POSIX-sockets.
 */

UdpSocket::UdpSocket(int port) {
     // AF_INET = IPv4, SOCK_DGRAM = datagram, dvs. UDP
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_ < 0){
        perror("socket");
    }
     // Utan SO_BROADCAST tillåter OS att skicka till 255.255.255.255
    int broadcastEnable = 1;
    int s = setsockopt(sock_, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if(s < 0){
        perror("setsocktopt");
    };

    // Låter flera noder (processer) på samma dator binda samma port,
    // så att alla tar emot samma broadcast.
    //Linux
    int reuse = 1;
    setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));  
#ifdef SO_REUSEPORT
    // macOS/BSD kräver SO_REUSEPORT för att flera noder ska kunna binda samma port
    setsockopt(sock_, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
#endif

    // Väcker tråden. Timeout på recvfrom: receive() returnerar efter högst 200 ms även utan paket,
    // så att mottagartråden kan se att running_ blivit false och avslutas.
    timeval tv{};
    tv.tv_sec  = 0;
    tv.tv_usec = 200000;   // 200 ms
    if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt SO_RCVTIMEO");
    }

    // Lyssna på alla nätverkskort (INADDR_ANY) på given port
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port); 
    memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

    int b =  bind(sock_, (struct sockaddr *)&addr, sizeof(addr));
    if(b < 0){
        perror("bind");
        std::exit(1);
    }
}

UdpSocket::~UdpSocket() {
    close(sock_);
}

void UdpSocket::send(const std::string& msg, const std::string& destIp, int destPort){
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(destPort);
    dest.sin_addr.s_addr = inet_addr(destIp.c_str());// "255.255.255.255"
    ssize_t sent = sendto(sock_, msg.c_str(), msg.size(), 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
    if (sent < 0){
        perror("sendto");
    }
};

std::string UdpSocket::receive() {
    // Max 2047 bytes per paket. L1 skickar 8 tecken per byte, så en ram får vara
    // högst ~255 tecken innan den kapas.
    char buffer[2048];
    sockaddr_in senderAddr{};
    socklen_t len = sizeof(senderAddr);
    ssize_t n = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, reinterpret_cast<sockaddr*>(&senderAddr), &len);
    if (n < 0) {
        return "";
    }
    buffer[n] = '\0';
    return std::string(buffer, n);
}

