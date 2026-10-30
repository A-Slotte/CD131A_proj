En chattapplikation som simulerar OSI-modellens sju lager. Meddelanden skickas nedåt genom
lagren (Application → Physical), sänds som broadcast över UDP (port 9000) och packas upp
uppåt igen hos mottagaren.

Kräver CMake och c++ kompilator. Skrivet för linux.

Bygga ocj köra på linux

sudo apt update && sudo apt install -y build-essential cmake git   # Ubuntu/Debian
cmake -B build
cmake --build build

Starta en nod: ./build/chat <nodadress> <mac> <port> [medium-port]

nodadress: heltal, logisk nodadress i L3 (SRC/DST)
mac: kort länkadress i L2 (t.ex. AA)
port: logisk port i L4 (SPORT/DPORT), dvs. vilken tjänst på noden meddelandet ska till
medium-port: UDP-porten som är det delade mediet i L1. Alla noder som ska höra varandra måste använda samma medium-port.

Öppna två terminaler och starta två noder:


./build/chat 1 AA 5000



./build/chat 2 BB 6000


Skriv help för att se kommandona:

| Kommando | Beskrivning |
| msg <nod> <port> <meddelande> | Skickar ett meddelande, t.ex. msg 2 6000 hej från nod 1 
| msgerror <nod> <port> <meddelande> | Skickar med en slumpad bitflip i L1 
| msgflip <nod> <port> <bit,bit,..> <meddelande> | Skickar med valda bitar flippade i L1, t.ex. msgflip 2 6000 400,401 hej |
| readfile <fil> | Läser en ram (text eller bitström) från fil och matar in den i L1 som om den kom från mediet |
| history <nod> | Visar konversationen med en nod |
| quit | Avslutar |

readfile används för att testa avkodningen mot specens testfiler, t.ex.
./build/chat 5 BB 6000 och sedan readfile overforingsfiler/frame_A_korrekt.txt.

Bygga på MACOS


xcode-select --install
brew install cmake
cmake -B build
cmake --build build


Kör sedan noderna som på Linux. 
OBS- inte testat köra programmet på mac endast att bygget kompilerar via GItHub Actions.
