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
    main.cpp
    UdpSocket.cpp
    Layer.cpp
    PresentationLayer.cpp
    ApplicationLayer.cpp
    SessionLayer.cpp
    TransportLayer.cpp
    DatalinkLayer.cpp
    NetworkLayer.cpp
    PhysicalLayer.cpp
)

target_compile_options(chat PRIVATE -Wall -Wextra)

target_link_libraries(chat PRIVATE Threads::Threads)