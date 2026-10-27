#include "Layer.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

void Layer::connect(Layer* up, Layer* down){
    up_ = up;
    down_ = down;
}; 

std::vector<std::string> Layer::split(const std::string& is, char delim){
    std::vector<std::string> res;
    std::stringstream ss(is);
    std::string part;
    while(std::getline(ss, part, delim)){
        res.push_back(part);
    }
    return res;
};

std::string Layer::getHeader(const std::string& headerParts, std::string targ){
    std::string res;
    std::vector<std::string> fields = split(headerParts, ';');
    for (const auto field : fields) {
        auto a = split(field, '=');
        if(a[0] == targ){
            res = a[1];
            return res;
        };
    }
    return "";
    
}

bool Layer::splitPdu(const std::string& pdu, std::string& header, std::string& payload){
    size_t  pos = pdu.find("DATA=");
    if (pos == std::string::npos) return false;   // ogiltigt format
    header = pdu.substr(0, pos);
    payload = pdu.substr(pos + 5);
    if (!payload.empty() && payload.back() == '\n') {
        payload.pop_back();
    }
    return true;
}

[[noreturn]] void Layer::throwPacket(const std::string& layer, const std::string& reason) const{
    std::cout << "Packet dropped by " << layer << ": " << reason << std::endl;
    throw PacketDropped{};
}