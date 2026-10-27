#pragma once
#include <string>

struct PacketInfo{
    std::string msg;
    std::string msgEncrypted;
    std::string data;
    int cryptkey = 0;
    int sid = 0;
    int seq = 0;
    std::string ttl;
    std::string mac_dst;
    std::string mac_src;
    std::string DPORT;
    std::string SPORT;
    std::string src;
    std::string dst;
    bool flipBit = false;
};