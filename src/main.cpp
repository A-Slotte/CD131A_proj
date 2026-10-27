#include "net/UdpSocket.h"
#include "layers/ApplicationLayer.h"
#include "layers/PresentationLayer.h"
#include "layers/SessionLayer.h"
#include "layers/TransportLayer.h"
#include "layers/NetworkLayer.h"
#include "layers/DatalinkLayer.h"
#include "layers/PhysicalLayer.h"
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
int main(int argc, char* argv[]) {
    if (argc < 4 || argc > 5) {
        std::cerr << "Usage: " << argv[0] << " <node address> <mac> <port> [medium udp port, default 9000]\n";
        return 1;
    }

    const std::string node = argv[1];
    const std::string mac  = argv[2];
    const std::string port = argv[3];                     // logisk port i L4 (SPORT/DPORT)
    const int mediumPort = argc == 5 ? std::stoi(argv[4]) : 9000;   // UDP-porten = det delade mediet (L1)

    auto app = std::make_unique<ApplicationLayer>();
    auto pres = std::make_unique<PresentationLayer>();
    auto sesh = std::make_unique<SessionLayer>();
    auto trans = std::make_unique<TransportLayer>();
    auto net = std::make_unique<NetworkLayer>();;
    auto dlink = std::make_unique<DatalinkLayer>();
    auto phys = std::make_unique<PhysicalLayer>(mediumPort);

    net -> updateSrc(node);
    dlink -> setMac(mac);
    trans -> setPort(port);

    app -> connect(nullptr, pres.get());
    pres -> connect(app.get(), sesh.get());
    sesh -> connect(pres.get(), trans.get());
    trans -> connect(sesh.get(), net.get());
    net -> connect(trans.get(), dlink.get());
    dlink -> connect(net.get(), phys.get());
    phys -> connect(dlink.get(), nullptr);

    // readfile-kommandot simulerar mediet: bitströmmen från filen lämnas till L1
    app -> setFrameInjector([&phys](const std::string& bits){ phys -> receiveBits(bits); });

    phys -> start();
    app -> run();
    return 0;
}