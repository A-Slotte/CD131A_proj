# CD131A – OSI-modellen i C++

En chattapplikation som simulerar OSI-modellens sju lager. Meddelanden skickas nedåt genom
lagren (Application → Physical), sänds som broadcast över UDP (port 9000) och packas upp
uppåt igen hos mottagaren.

Kräver CMake (3.16+) och en C++20-kompilator. Skrivet för Linux.

## Bygga och köra på Linux

```sh
sudo apt update && sudo apt install -y build-essential cmake git   # Ubuntu/Debian
cmake -B build
cmake --build build
```

Starta en nod: `./build/chat <nodadress> <mac> <port> [medium-port]`

- `nodadress`: heltal, logisk nodadress i L3 (`SRC`/`DST`)
- `mac`: kort länkadress i L2 (t.ex. `AA`)
- `port`: logisk port i L4 (`SPORT`/`DPORT`), dvs. vilken tjänst på noden meddelandet ska till
- `medium-port` (valfri, standard 9000): UDP-porten som är det delade mediet i L1. Alla noder som
  ska höra varandra måste använda samma medium-port.

Öppna två terminaler och starta två noder:

```sh
./build/chat 1 AA 5000
```

```sh
./build/chat 2 BB 6000
```

Skriv `help` för att se kommandona:

| Kommando | Beskrivning |
|---|---|
| `msg <nod> <port> <meddelande>` | Skickar ett meddelande, t.ex. `msg 2 6000 hej` från nod 1 |
| `msgerror <nod> <port> <meddelande>` | Skickar med en slumpad bitflip i L1 |
| `msgflip <nod> <port> <bit,bit,..> <meddelande>` | Skickar med valda bitar flippade i L1, t.ex. `msgflip 2 6000 400,401 hej` |
| `readfile <fil>` | Läser en ram (text eller bitström) från fil och matar in den i L1 som om den kom från mediet |
| `history <nod>` | Visar konversationen med en nod |
| `quit` | Avslutar |

`readfile` används för att testa avkodningen mot specens testfiler, t.ex.
`./build/chat 5 BB 6000` och sedan `readfile overforingsfiler/frame_A_korrekt.txt`.

## Köra på macOS i en Linux-VM (Multipass)

[Multipass](https://canonical.com/multipass) startar en Ubuntu-VM med ett kommando.
Det fungerar på både Apple Silicon och Intel-Mac.

1. Installera Multipass (kräver [Homebrew](https://brew.sh)):
   ```sh
   brew install --cask multipass
   ```
2. Skapa och starta VM:en:
   ```sh
   multipass launch --name osi
   ```
3. Kopiera in projektet (kör från mappen som innehåller projektmappen):
   ```sh
   multipass transfer --recursive CD131A_proj osi:/home/ubuntu/
   ```
4. Öppna ett skal i VM:en, installera verktyg och bygg:
   ```sh
   multipass shell osi
   sudo apt update && sudo apt install -y build-essential cmake
   cd ~/CD131A_proj
   cmake -B build
   cmake --build build
   ./build/chat 1 AA 5000
   ```
5. Öppna en ny terminal på Macen, kör `multipass shell osi` igen och starta nästa nod:
   ```sh
   cd ~/CD131A_proj && ./build/chat 2 BB 6000
   ```

Ta bort VM:en efteråt med `multipass delete --purge osi`.

## Bygga direkt på macOS (utan VM)

```sh
xcode-select --install
brew install cmake
cmake -B build
cmake --build build
```

Kör sedan noderna som på Linux. Första gången kan macOS-brandväggen fråga om programmet får
ta emot inkommande anslutningar. Broadcast kräver att Macen har ett aktivt nätverk (wifi/ethernet).
