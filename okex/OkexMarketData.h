//
// Created by phuang on 6/26/2018.
//

#pragma once
#include <boost/bind.hpp>
#include "IExchangeMarketData.h"
#include "okcoinwebsocketapi.h"

namespace newton
{
namespace exmdapi
{


class OkexMarketData : public IExchangeMarketData
{
public:
  OkexMarketData();
  ~OkexMarketData()
  {
    comapi->Close();
  }

protected:
  virtual bool
  isSupported(CryptoPairEnum p) override
  {
    switch (p)
    {
      case BTC_USDT: return true;
      case ETH_USDT: return true;
      case ETH_BTC: return false;
      default: return false;
    }
  }

  virtual void
  initOutThread() override;
  virtual void
  cleanupOutThread() override {};

  virtual void
  initInThread() override{};
  virtual void
  cleanupInThread() override{};

  virtual void
  runEventLoop() override
  {
    OKCoinWebSocketApiCom::RunThread(comapi.get());
  };
public:
  std::shared_ptr<OKCoinWebSocketApiCom> comapi;
};

}
}


