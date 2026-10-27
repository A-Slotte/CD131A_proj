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

Starta en nod: `./build/chat <nod> <mac> <port>`. Alla noder ska använda port **9000**.

Öppna två terminaler och starta två noder:

```sh
./build/chat A AA 9000
```

```sh
./build/chat B BB 9000
```

Skriv `help` för att se kommandona, t.ex. `msg B 9000 hej` från nod A.

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
   ./build/chat A AA 9000
   ```
5. Öppna en ny terminal på Macen, kör `multipass shell osi` igen och starta nästa nod:
   ```sh
   cd ~/CD131A_proj && ./build/chat B BB 9000
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
