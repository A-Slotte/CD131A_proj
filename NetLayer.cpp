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

std::string NetLayer::getHeader(const std::string& headerParts, std::string targ, int headerIndex, int headerTarget){
    std::string res;
    std::vector<std::string> fields = split(headerParts, ';');
    for (const auto field : fields) {
        auto a = split(field, '=');
        if(a[headerIndex] == targ){
            res = a[headerTarget];
            return res;
        };
    }
    
}