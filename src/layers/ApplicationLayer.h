#pragma once
#include <iostream>
#include <string>
#include "Layer.h"
#include <map>
#include <vector>
#include <functional>

class ApplicationLayer : public Layer {
public:
    struct Message {
        std::string from;
        std::string text;
    };

    
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
    void run();
    void handleCommand(const std::string& input);
    std::vector<std::string> splitWhitespaces(const std::string& str);

    void addConversationMessage(const std::string& node, const std::string& from, const std::string& msg);
    void printConversation(const std::string& node) const;
    void setFrameInjector(std::function<void(const std::string&)> injector){
        frameInjector_ = std::move(injector);
    }

    std::map<std::string, std::vector<Message>> conversations;
private:
    void sendMessage(const std::vector<std::string>& command, size_t msgStart, PacketInfo packet);
    void readFrameFile(const std::string& path);
    std::function<void(const std::string&)> frameInjector_;   // går direkt till L1, används bara av readfile
    std::string divider = "========================================";
};