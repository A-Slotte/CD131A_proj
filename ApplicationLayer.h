#pragma once
#include <iostream>
#include <string>
#include "Layer.h"
#include <map>
#include <vector>

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

    std::map<std::string, std::vector<Message>> conversations;
private:
    std::string divider = "========================================";
};