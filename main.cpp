#include "UdpSocket.h"
#include "ApplicationLayer.h"
#include "PresentationLayer.h"
#include "SessionLayer.h"
#include "TransportLayer.h"
#include <iostream>
#include <string>
#include <thread>

/*
void handleCommand(const std::string& input, std::thread& t, UdpSocket& udp) {
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
        t.join();
        std::exit(0);
    }
    else if(command == "broadcast"){
        std::string message = input.substr(pos + 1);
        udp.send(message, "255.255.255.255", 9000);
        std::cout << "message broadcasted" << std::endl;
    }
    else{std::cout << "Command " << "'" << input << "'" << " not found" << std::endl;}
};
*/
/*
void recieveLoop(UdpSocket& udp){
    while(true){
        std::string msg = udp.receive();
        if(!msg.empty()){
            std::cout << "\n" << divider << "\n" << "New message: " << msg << "\n" << std::endl;
        };
    };
};
UdpSocket udp(9000);
    std::cout << divider + "\n" +"Type -help for all commands" + "\n" + divider +"\n" <<std::endl;
    std::thread t(recieveLoop, std::ref(udp));
    std::string input;
*/
int main() {
    UdpSocket udp(9000);
    auto app = std::make_unique<ApplicationLayer>();
    auto pres = std::make_unique<PresentationLayer>();
    auto sesh = std::make_unique<SessionLayer>();
    auto trans = std::make_unique<TransportLayer>();

    app -> connect(nullptr, pres.get());
    pres -> connect(app.get(), sesh.get());
    sesh -> connect(pres.get(), trans.get());
    trans -> connect(sesh.get(), nullptr);

    app -> run();

    
    
    return 0;
}