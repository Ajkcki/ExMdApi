//
// Created by phuang on 6/27/2018.
//
#include "OkexMarketData.h"

using namespace  newton::exmdapi;

int main(){
  OkexMarketData okex;
  okex.addCryptoPair(CryptoPairEnum::BTC_USDT);
  //okex.addCryptoPair(CryptoPairEnum::ETH_USDT);
  okex.addCryptoPair(CryptoPairEnum::ETH_BTC);
  okex.start();
}