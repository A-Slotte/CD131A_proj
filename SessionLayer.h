#pragma once
#include "Layer.h"
#include <map>
#include <mutex>

class SessionLayer : public Layer{
public:
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
protected:

    struct sessionInfo{
        int sid_;
        int sendSeq_;   // senaste skickade SEQ till motparten
        int recvSeq_;   // senaste mottagna SEQ från motparten
    };
    sessionInfo& getSession(const std::string& peer);
    bool parseNumber(const std::string& text, int& out) const;
    std::map<std::string, sessionInfo> sessions; // key: motpartens adress
    std::mutex sessionsMutex_;                   // sändning och mottagning körs i olika trådar
};
