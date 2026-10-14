#include "ApplicationLayer.h"
#include <string>
#include <iostream>
#include <thread>

void ApplicationLayer::encapsulate(const PacketInfo& packetInfo, std::string sdu){
    std::cout << printDecor1;
    try{
        down_->encapsulate(packetInfo, sdu);
    }
    catch(const PacketDropped&){
    }
};


void ApplicationLayer::decapsulate(const PacketInfo&, std::string sdu){
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
        packet.mac_src = "AA";
        packet.src = "192.162.10.7";
        packet.dst = "192.162.10.8";
        packet.ttl = "3";
        packet.DPORT = "9000";
        packet.SPORT = "8000";
        packet.cryptkey = 3;

        encapsulate(packet, packet.msg);

    }
    else{std::cout << "Command " << "'" << input << "'" << " not found" << std::endl;};
};

    


        
 
  
