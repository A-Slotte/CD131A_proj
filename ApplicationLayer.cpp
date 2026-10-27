#include "ApplicationLayer.h"
#include <string>
#include <iostream>
#include <thread>
#include <sstream>

void ApplicationLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    printLayerStep("ApplicationLayer", sdu);
    addConversationMessage(packetInfo.dst, "me", sdu);
    try{
        down_->encapsulate(packetInfo, sdu);
    }
    catch(const PacketDropped&){
    }
};


void ApplicationLayer::decapsulate(const PacketInfo& packetInfo, std::string sdu){
    addConversationMessage(packetInfo.src, packetInfo.src, sdu);
    std::cout << divider << "\n" << "Message received: " << sdu << std::endl;
};
 

void ApplicationLayer::run(){
    std::cout << divider + "\n" +"Type -help for all commands" + "\n" + divider +"\n" <<std::endl;
    std::string input;
    while (true)
    {
        if(!std::getline(std::cin, input)){
            break;
        }
        if(input.empty()){
            continue;
        }
        handleCommand(input);
    }
};
std::vector<std::string> ApplicationLayer::splitWhitespaces(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void ApplicationLayer::addConversationMessage(const std::string& node, const std::string& from, const std::string& msg) {
    conversations[node].push_back({from, msg});
}

void ApplicationLayer::printConversation(const std::string& node) const {
    auto it = conversations.find(node);
    if (it == conversations.end()) {
        std::cout << "No conversation with " << node << "\n";
        return;
    }

    std::cout << "Conversation with " << node << ":\n";
    for (const auto& msg : it->second) {
        std::cout << "  [" << msg.from << "] " << msg.text << "\n";
    }
}

void ApplicationLayer::handleCommand(const std::string& input){

    auto command = splitWhitespaces(input);

    if (command[0] == "help"){
        std::cout << divider+"\n""Commands:\n" 
        "quit                               Exits program\n"
        "msg <node> <port> <message>        Sends message to address\n"
        "msgerror <node> <port> <message>   Sends message to address with bit error\n"
        "history <node>                     Shows saved conversation\n";
    }
    else if(command[0] == "quit"){
        std::cout << "\n""Closing program!""\n";
        std::exit(0);
    }
    else if(command[0] == "history" && command.size() == 2){
        printConversation(command[1]);
    }
    else if(command[0] == "msg"){
        PacketInfo packet;
        std::string message;
        for(size_t i = 3; i < command.size(); i++){
            message += command[i] +" ";
        }
        packet.flipBit = false;
        packet.msg = message;
        packet.mac_dst = "FF";
        packet.dst = command[1];
        packet.ttl = "3";
        packet.DPORT = command[2];
        packet.cryptkey = 3;
        encapsulate(packet, packet.msg);
    }
    else if(command[0] == "msgerror"){
        PacketInfo packet;
        std::string message = "";
        for(size_t i = 3; i < command.size(); i++){
            message += command[i] +" ";
        }
        packet.flipBit = true;
        packet.msg = message;
        packet.mac_dst = "FF";
        packet.dst = command[1];
        packet.ttl = "3";
        packet.DPORT = command[2];
        packet.cryptkey = 3;
        encapsulate(packet, packet.msg);
    }
    else{std::cout << "Command " << "'" << input << "'" << " not found" << std::endl;};
};







