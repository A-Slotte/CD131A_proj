#include "ApplicationLayer.h"
#include <string>
#include <iostream>
#include <thread>

// ADD - Handle SID;
void ApplicationLayer::encapsulate(const PacketInfo& packetInfo, std::string packet){
    down_->encapsulate(packetInfo, "");
};

// ADD - Handle SID;

void ApplicationLayer::decapsulate(const PacketInfo& packetInfo, std::string packet){
    std::cout << divider << "\n" << "Message recieved: " << packetInfo.msg << std::endl;
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

void ApplicationLayer::handleCommand(const std::string& input){

    size_t pos = input.find_first_of(' ');
    std::string command = input.substr(0,pos);
    if (command == "help"){
        std::cout << divider+"\n""Commands:\n" 
        "quit                         Exits program\n"
        "msg <address> <message>      Sends message to address\n"
        "broadcast <message>          Sends broadcast message\n" ;
    }
    else if(command == "quit"){
        std::cout << "\n""Closing program!""\n";
        std::exit(0);
    }
    else if(command == "broadcast"){
        PacketInfo packet;
        packet.msg = input.substr(pos + 1);
        packet.mac_dst = "FF";
        packet.mac_dst = "AA";
        packet.dest = "192.168.10.1";
        packet.cryptkey = 3;

        encapsulate(packet, "");

    }
    else{std::cout << "Command " << "'" << input << "'" << " not found" << std::endl;};
};

    


        
 
  
