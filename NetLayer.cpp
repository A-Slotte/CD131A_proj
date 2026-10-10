#include "NetLayer.h"
#include <string>
#include <vector>
#include <sstream>

void NetLayer::connect(NetLayer* up, NetLayer* down){
    up_ = up;
    down_ = down;
}; 

std::vector<std::string> NetLayer::split(const std::string& is, char delim){
    std::vector<std::string> res;
    std::stringstream ss(is);
    std::string part;
    while(std::getline(ss, part, delim)){
        res.push_back(part);
    }
    return res;
};

std::string NetLayer::getHeader(const std::string& headerParts, std::string targ){
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

bool NetLayer::splitPdu(const std::string& pdu, std::string& header, std::string& payload){
    size_t  pos = pdu.find("DATA=");
    if (pos == std::string::npos) return false;   // ogiltigt format
    header = pdu.substr(0, pos);
    payload = pdu.substr(pos + 5);
    if (!payload.empty() && payload.back() == '\n') {
        payload.pop_back();
    }
    return true;
}