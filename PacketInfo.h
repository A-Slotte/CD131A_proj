#pragma once
#include <string>

struct PacketInfo{
    std::string msg;
    std::string data;
    int cryptkey;
    int sid;
    std::string mac_dst;
    std::string mac_src;
    std::string dest;
    std::string src;
};