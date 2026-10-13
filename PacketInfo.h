#pragma once
#include <string>

struct PacketInfo{
    std::string msg;
    std::string msgEncrypted;
    std::string data;
    int cryptkey;
    int sid;
    int seq;
    std::string ttl;
    std::string mac_dst;
    std::string mac_src;
    std::string DPORT;
    std::string SPORT;
    std::string src;
    std::string dst;
};