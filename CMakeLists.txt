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
    main.cpp
    UdpSocket.cpp
    NetLayer.cpp
    PresentationLayer.cpp
    ApplicationLayer.cpp
    SessionLayer.cpp
    TransportLayer.cpp
    # lägg till fler .cpp-filer här när du skapar dem
)

target_compile_options(chat PRIVATE -Wall -Wextra)

# Sanitizers i debugbygget
target_compile_options(chat PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
target_link_options(chat PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

target_link_libraries(chat PRIVATE Threads::Threads)