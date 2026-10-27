#pragma once
#include <string>

class UdpSocket{
public:
    UdpSocket(int port);
    ~UdpSocket();

    UdpSocket(const UdpSocket&) = delete;              // förbjud kopiering
    UdpSocket& operator=(const UdpSocket&) = delete;    // förbjud kopiering


    void send(const std::string& msg, const std::string& destIp, int destPort);
    std::string receive();
private:
    int sock_;
};
