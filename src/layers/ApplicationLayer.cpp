#include "ApplicationLayer.h"
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include <fstream>
#include <bitset>

/**
 * @file ApplicationLayer.cpp
 * @brief Implementation av L7 – Applikation: kommandotolk, visning av
 *        mottagna meddelanden och konversationshistorik.
 */

void ApplicationLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    printLayerStep("ApplicationLayer", sdu);
     
    // Sparar efter stacken körtklart ifall meddelandet inte skulle skickats. 
    try{
        down_->encapsulate(packetInfo, sdu); 
        addConversationMessage(packetInfo.dst, "me", sdu);
    }
    catch(const PacketDropped&){
        //Lagret som kastar har redan skrivit ut.
    }

};

void ApplicationLayer::decapsulate(const PacketInfo& packetInfo, std::string sdu){
    printLayerStep("ApplicationLayer", sdu);
    // src från nätverkslagret och skickades upp med packetInfo, så L7 vet avsändaren utan att läsa L3-headern.
    addConversationMessage(packetInfo.src, packetInfo.src, sdu);
    std::cout << divider << "\n" << "Message received from node " << packetInfo.src << ": " << sdu << std::endl;
};
 

void ApplicationLayer::run(){
    std::cout << divider + "\n" +"Type -help for all commands" + "\n" + divider +"\n" <<std::endl;
    std::string input;
    // Huvudtråden läser kommandon; mottagning sker parallellt i
    // PhysicalLayers egen tråd, så stdin kan blockera här.
    while (true)
    {
        if(!std::getline(std::cin, input)){
            break; // (Ctrl+D) avslutar programmet
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
    if(command.empty()){
        return;
    }

    if (command[0] == "help"){
        std::cout << divider+"\n""Commands:\n"
        "quit                                      Exits program\n"
        "msg <node> <port> <message>               Sends message to address\n"
        "msgerror <node> <port> <message>          Sends message with one random bit error\n"
        "msgflip <node> <port> <bit,bit,..> <msg>  Sends message with chosen bits flipped (L1)\n"
        "readfile <file>                           Feeds a frame/bit stream from file into L1\n"
        "history <node>                            Shows saved conversation\n";
    }
    else if(command[0] == "quit"){
        std::cout << "\n""Closing program!""\n";
        std::exit(0);
    }
    else if(command[0] == "history" && command.size() == 2){
        printConversation(command[1]);
    }
    else if(command[0] == "msg" && command.size() >= 4){
        sendMessage(command, 3, PacketInfo{});
    }
    else if(command[0] == "msgerror" && command.size() >= 4){
        PacketInfo packet;
        packet.flipBit = true;
        sendMessage(command, 3, packet);
    }
    else if(command[0] == "msgflip" && command.size() >= 5){
        PacketInfo packet;
        // Bitpositionerna räknas i hela L1-bitströmmen inclusive stx och headers.
        // CRC täcker bara DATA, så en flip i t.ex MAC_SRC upptäcks inte.
        for(const auto& pos : split(command[3], ',')){
            // Tom sträng t.ex. ("400,,4001") eller ogiltiga tecken t.ex. (x,-5,g)
            if(pos.empty() || pos.find_first_not_of("0123456789") != std::string::npos){
                std::cout << "Invalid bit position '" << pos << "'" << std::endl;
                return;
            }
            packet.flipPositions.push_back(std::stoul(pos));
        }
        sendMessage(command, 4, packet);
    }
    else if(command[0] == "readfile" && command.size() == 2){
        readFrameFile(command[1]);
    }
    else{std::cout << "Command " << "'" << input << "'" << " not found or missing arguments (see help)" << std::endl;};
};
/**
 * @brief Bygger meddelandet och packetInfo som skickas vidare till encapsulate.
 * 
 * @param command Kommandot uppdelat i ord: [kommando, nod, port, .... medelandet]. Meningar kommer uppdelade i seperata element.
 * 
 * @param msgStart Index där meddelandetexten börjar
 * 
 * @param packet Förifyllt packetInfo, t.ex med flipBit eller flipPositions sata.
 */
void ApplicationLayer::sendMessage(const std::vector<std::string>& command, size_t msgStart, PacketInfo packet){
    std::string message;
    //bygger ihop meddelandet med blanksteg.
    for(size_t i = msgStart; i < command.size(); i++){
        if(!message.empty()){
            message += " ";
        }
        message += command[i];
    }
    packet.msg = message;
    packet.mac_dst = "FF"; // alla ramar skickas som broadcast på L2
    packet.dst = command[1];
    packet.ttl = "3";
    packet.DPORT = command[2];
    packet.cryptkey = 3; // Caesar-skift för L6
    encapsulate(packet, packet.msg);
}

// Testverktyg: läser en ram (text) eller bitström från fil och lämnar den till L1
// som om den kommit från mediet, t.ex. specens overforingsfiler/frame_*.txt
void ApplicationLayer::readFrameFile(const std::string& path){
    std::ifstream file(path);
    if(!file){
        std::cout << "Could not open file '" << path << "'" << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    while(!content.empty() && (content.back() == '\n' || content.back() == '\r')){
        content.pop_back();
    }

    std::string bits;
    if(content.find_first_not_of("01 \r\n") == std::string::npos){
        for(char c : content){
            if(c == '0' || c == '1') bits += c;
        }
    }
    else{
        for(unsigned char c : content){
            bits += std::bitset<8>(c).to_string();
        }
    }
    if(!frameInjector_){
        std::cout << "No physical layer connected" << std::endl;
        return;
    }
    frameInjector_(bits);
}
