#include "UdpSocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <string>
UdpSocket::UdpSocket(int port) {
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_ < 0){
        perror("socket");
    }
    int broadcastEnable = 1;
    int s = setsockopt(sock_, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if(s < 0){
        perror("setsocktopt");
    };
    
    int reuse = 1;
    setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));   // bra att ha kvar från tidigare

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port); // addr.sin_port = 10275?
    memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

    int b =  bind(sock_, (struct sockaddr *)&addr, sizeof(addr));
    if(b < 0){
        perror("bind");
    }
}

UdpSocket::~UdpSocket() {
    close(sock_);
}

void UdpSocket::send(const std::string& msg, const std::string& destIp, int destPort){
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(destPort);
    dest.sin_addr.s_addr = inet_addr(destIp.c_str());
    ssize_t sent = sendto(sock_, msg.c_str(), msg.size(), 0, reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
    if (sent < 0){
        perror("sendto");
    }
};

std::string UdpSocket::receive() {
    char buffer[1024];
    sockaddr_in senderAddr{};
    socklen_t len = sizeof(senderAddr);
    ssize_t n = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0, reinterpret_cast<sockaddr*>(&senderAddr), &len);
    if (n < 0) {
        return "";
    }
    buffer[n] = '\0';
    return std::string(buffer, n);
}

