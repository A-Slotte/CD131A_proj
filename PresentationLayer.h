#pragma Once
#include "NetLayer.h"
#include <string>

class PresentationLayer : public NetLayer {
public:
    void encapsulate(const PacketInfo& packetInfo, std::string packet) override;
    void decapsulate(const PacketInfo& packetInfo, std::string packet) override;
private:
    
    std::string encrypt(const PacketInfo& data);
    std::string decrypt(const PacketInfo& data);
};