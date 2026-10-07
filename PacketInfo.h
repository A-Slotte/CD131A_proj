#pragma once
#include <string>

struct PacketInfo{
    std::string msg;
    std::string msgEncrypted;
    std::string data;
    int cryptkey;
    int sid;
    int seq;
    std::string mac_dst;
    std::string mac_src;
    std::string dest;
    std::string src;
};