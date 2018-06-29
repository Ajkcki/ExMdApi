cmake_minimum_required(VERSION 3.7)
project(ExMdApi)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(okex)

add_executable(OkTest
        okex/okcoinwebsocketapi.cpp
        okex/okcoinwebsocketapi.h
        okex/OkexMarketData.cpp
        okex/OkexMarketData.h
        okex/OkTest.cpp
        okex/parameter.cpp
        okex/parameter.h
        okex/websocket.h
        IExchangeMarketData.cpp
        IExchangeMarketData.h)

FIND_PACKAGE(Boost 1.62.0 COMPONENTS system thread REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(OkTest  ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)