cmake_minimum_required(VERSION 3.16)
project(chatapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Threads REQUIRED)

add_executable(chat
    src/main.cpp
    src/net/UdpSocket.cpp
    src/layers/Layer.cpp
    src/layers/ApplicationLayer.cpp
    src/layers/PresentationLayer.cpp
    src/layers/SessionLayer.cpp
    src/layers/TransportLayer.cpp
    src/layers/NetworkLayer.cpp
    src/layers/DatalinkLayer.cpp
    src/layers/PhysicalLayer.cpp
)

# Includes skrivs relativt src/, t.ex. #include "layers/SessionLayer.h"
target_include_directories(chat PRIVATE src)

target_compile_options(chat PRIVATE -Wall -Wextra)

target_link_libraries(chat PRIVATE Threads::Threads)