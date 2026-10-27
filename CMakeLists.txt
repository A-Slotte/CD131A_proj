cmake_minimum_required(VERSION 3.16)
project(chatapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # ger VS Code bättre IntelliSense

# Standardbygge = Debug om inget annat anges
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Threads REQUIRED)

add_executable(chat
    src/main.cpp
    src/net/UdpSocket.cpp           # UDP-socket = det delade mediet
    src/layers/Layer.cpp            # gemensam basklass för lagren
    src/layers/ApplicationLayer.cpp     # L7
    src/layers/PresentationLayer.cpp    # L6
    src/layers/SessionLayer.cpp         # L5
    src/layers/TransportLayer.cpp       # L4
    src/layers/NetworkLayer.cpp         # L3
    src/layers/DatalinkLayer.cpp        # L2
    src/layers/PhysicalLayer.cpp        # L1
    # lägg till fler .cpp-filer här när du skapar dem
)

# Includes skrivs relativt src/, t.ex. #include "layers/SessionLayer.h"
target_include_directories(chat PRIVATE src)

target_compile_options(chat PRIVATE -Wall -Wextra)

# Sanitizers i debugbygget
target_compile_options(chat PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
target_link_options(chat PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

target_link_libraries(chat PRIVATE Threads::Threads)