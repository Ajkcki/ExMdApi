//
// Created by huang on 6/23/18.
//
#pragma once
#include <set>
#include <map>
#include <memory>
#include <thread>
#include <string>
#include <iostream>
#include "json.hpp"

//const short SOURCE_OKEX = 1;

namespace newton
{
namespace exmdapi
{

/*
 * Enumerate all supported exchanges.
 */
enum ExchangeEnum {
  OKEX = 1,
  BINANCE = 2,
  HOUBI = 3,
  BITFINEX = 4
};
inline const char *
c_str (ExchangeEnum p)
{
  switch (p)
    {
      case OKEX:
        return "OKEX";
      case BINANCE:
        return "BINANCE";
      case HOUBI:
        return "HOUBI";
      case BITFINEX:
        return "BITFINEX";
      default:
        return "UNKNOWN_EXCHANGE";
    }
}

/*
 * Enumerate all crypto pairs, not all pairs are traded at every exchange.
 */
enum CryptoPairEnum {
  BTC_USDT = 1,
  ETH_USDT,
  ETH_BTC
};
#define UNKNOWN_CRYPTO_PAIR "UNKNOWN_CRYPTO_PAIR"
inline const char *
c_str (CryptoPairEnum p)
{
  switch (p)
    {
      case BTC_USDT:
        return "BTC_USDT";
      case ETH_USDT:
        return "ETH_USDT";
      case ETH_BTC:
        return "ETH_BTC";
      default:
        return UNKNOWN_CRYPTO_PAIR;
    }
}

/*
 * Level 2 order book data gotten from an exchange.
 * Different exchanges have different market data format (in json),
 * all of them must be transformed to this format.
 *
 */
typedef double Price;
typedef double Volume;
struct Depth {
  ExchangeEnum ex;
  CryptoPairEnum cryptoPair;
  long timestamp;
  std::map<Price, Volume> mBid;
  std::map<Price, Volume> mAsk;
  friend void to_json (nlohmann::json &j, const Depth &p);
  friend void from_json (const nlohmann::json &j, Depth &p);
};

struct Trade {
  ExchangeEnum ex;
  CryptoPairEnum cryptoPair;
  long id; //an id from the exchange
  long timestamp;
  Price price;
  Volume volume;
  bool isbuy;
  friend void to_json (nlohmann::json &j, const Trade &p);
  friend void from_json (const nlohmann::json &j, Trade &p);
};

class IExchangeMarketData {
public:
  IExchangeMarketData (ExchangeEnum exx)
    : _isRunning (false), ex (exx)
  {};
  virtual ~IExchangeMarketData ()
  {}

  // add crypto pair(s) to sSubscribedCryptoPairs
  void
  addCryptoPair (CryptoPairEnum cp)
  {
    if (isSupported (cp))
      sSubscribedCryptoPairs.insert (cp);
  }
  void
  addCryptoPairs (std::initializer_list<CryptoPairEnum> lcp)
  {
    for (auto &cp : lcp)
      addCryptoPair (cp);
  }
  const std::set<CryptoPairEnum> &
  getCryptoPairs ()
  {
    return sSubscribedCryptoPairs;
  };

  void
  start ()
  {
    //If disconnected, automatically retry
    for (int i = 0; i < MAX_RETRY; i++)
      {
        initOutThread ();
        marketdata_thread = std::shared_ptr<std::thread> (new std::thread (&IExchangeMarketData::_start, this));
        marketdata_thread->join ();
        cleanupOutThread ();
        //std::cout<<"after join()"<<std::endl;
        //_isRunning = true means we lost connect to the exchange, just restart a thread and reconnect.
        if (isRunning ())
          {
            continue;
          }
        else
          {
            break;
          }
      }
  }

  void
  stop ()
  {
    _stop ();
  }

  void
  onDepth (const Depth depth)
  {
    nlohmann::json j = depth;
    std::cout << j << std::endl;
  };

  void
  onTrade (const std::vector<Trade> vTrade)
  {
    for (const Trade &t: vTrade)
      {
        nlohmann::json jt = t;
        std::cout << jt << std::endl;
      }
  };

protected:
  /*
   * Subclass (for a specific exchange) checks if crypto pair p is supported.
   * Not all crypto pairs list in CrytoPairEnum are supported by all exchanges.
   */
  virtual bool
  isSupported (CryptoPairEnum p) =0;
  /*
   * Subclass (for a specific exchange) implements these two methods to initialize
   * BEFORE starting a new thread to run the event loop, and to clean up AFTER
   * stopping the new thread.
   *
   * See start() for details
   */
  virtual void
  initOutThread () =0;
  virtual void
  cleanupOutThread () = 0;

  /*
   * Subclass (for a specific exchange) implements these two methods to
   * initialize right AFTER ENTERING in the new thread, and
   * to cleanup BEFORE EXITING from the new thread
   *
   * See _start() for details
   */
  virtual void
  initInThread () =0;
  virtual void
  cleanupInThread () = 0;

  /*
   * Subclass (for a specific exchange) implements this method to
   * start an event loop to receive market data from the exchange.
   *
   * See _start() for details
   */
  virtual void
  runEventLoop () =0;

private:
  void
  _start ()
  {
    _isRunning = true;
    initInThread ();
    //call subclass runEventLoop(), block here
    runEventLoop ();
    cleanupInThread ();
  }

  void
  _stop ()
  {
    _isRunning = false;
  }

  bool _isRunning;

protected:
  //identify which exchange
  ExchangeEnum ex;
  //hold all subscribed crypto pairs
  std::set<CryptoPairEnum> sSubscribedCryptoPairs;

  bool
  isRunning ()
  {
    return _isRunning;
  }
  std::shared_ptr<std::thread> marketdata_thread;
  const static int MAX_RETRY = 1;
};

typedef std::shared_ptr<IExchangeMarketData> IExchangeMarketDataPtr;

}
}

