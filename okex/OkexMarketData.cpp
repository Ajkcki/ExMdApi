//
// Created by phuang on 6/26/2018.
//

#include <string>
#include <vector>
#include  <stdexcept>
#include <boost/algorithm/string.hpp>
#include "json.hpp"
#include "OkexMarketData.h"

using namespace std;
using json = nlohmann::json;

namespace newton
{
namespace exmdapi
{

#define DEPTH 10
#define TRADE 20

/*
 * From string value input
 * "ok_sub_spot_btc_usdt_depth_20"
 * "ok_sub_spot_btc_usdt_deals"
 * find out if the message is a "depth" or a "deal"
 */
int
parseChannel(string channel)
{
  //depth last char is '0' ("ok_sub_spot_btc_usdt_depth_20")
  //trade last char is 's' ("ok_sub_spot_btc_usdt_deals")
  char c = channel.at(channel.size() - 1);
  if ('0' == c)
    return DEPTH;
  else if ('s' == c)
    return TRADE;
  else
    throw runtime_error("okex unknown channel, " + channel);
}
/*
 * From string value input
 * "ok_sub_spot_btc_usdt_depth_20"
 * "ok_sub_spot_btc_usdt_deals"
 * find out which crypto pair
 */
CryptoPairEnum
parseCryptoPair(string channel)
{
  vector<string> vStr;
  boost::split(vStr, channel, boost::is_any_of("_"));
  string nm = vStr[3] + "_" + vStr[4];
  if ("btc_usdt" == nm)
    return BTC_USDT;
  else if ("eth_usdt" == nm)
    return ETH_USDT;
  else if ("eth_btc" == nm)
    return ETH_BTC;
  else
    throw runtime_error("okex unknown crypto pair, " + nm);
}

Depth
parseDepth()
{

}

Trade
parseTrade()
{

}

/*
 * These three callback functions are invoked by function pointers (see websocket.h).
 * Thus they can not be method functions of OkexMarketData.
 * We keep a raw pointer gOkex here for them to access the
 * instance.
 */
static OkexMarketData *gOkex;
void
com_callbak_open()
{
  std::cout << "国际站连接成功！" << std::endl;
  this_thread::sleep_for(chrono::seconds(3));
  auto comapi = gOkex->comapi;
  for (CryptoPairEnum p : gOkex->getCryptoPairs())
  {
    switch (p)
    {
      case BTC_USDT:
      {
        //each of the following two will get 1 response from the server, as expected
        comapi->Emit("ok_sub_spot_btc_usdt_depth_20");
        comapi->Emit("ok_sub_spot_btc_usdt_deals");
      }
      case ETH_USDT:
      {
        //each of the following two will get 2 exactly the same responses from the server
        //don't know why
        comapi->Emit("ok_sub_spot_eth_usdt_depth_20");
        comapi->Emit("ok_sub_spot_eth_usdt_deals");
      }
      case ETH_BTC:
      {
        //each of the following two will get 2 exactly the same responses from the server
        //don't know why
        comapi->Emit("ok_sub_spot_eth_btc_depth_20");
        comapi->Emit("ok_sub_spot_eth_btc_deals");
      }
      //should not get here as sSubscribedCryptoPairs should not contain any pair not supported
      default:;
    }
  }
};

void
com_callbak_close()
{
  std::cout << "连接已经断开！ " << std::endl;
};

static int count = 0;

void
com_callbak_message(const char *message)
{
  // test auto-reconnect, need to comment out in production
//  if (count++ == 5)
//  {
//    gOkex->comapi->Close();
//    count = 0;
//  }

  std::cout << "Message: " << message << std::endl;
  json jmsg = json::parse(message)[0];
  //std::cout << jmsg << std::endl;
  string channel = jmsg.at("channel").get<string>();
  //acknowledgment from okex
  if("addChannel" == channel) {
    //std::cout << jmsg << std::endl;
    return;
  }
  //std::cout << "string: " << channel << std::endl;
  int ch = parseChannel(channel);
  if (DEPTH == ch)
  {

  }
  else if (TRADE == ch)
  {

  }
  else
  {

  }

};

//TODO ExchangeEnum::OKEX = 1
OkexMarketData::OkexMarketData() : IExchangeMarketData(ExchangeEnum::OKEX)
{
  std::string com_apiKey = "f99e1b5d-3433-47fe-b3b3-0aa1b8faa932";                 //请到www.okcoin.com申请。
  std::string com_secretKey = "894AA72C6D234B889A47E2A260581B7B";                  //请到www.okcoin.com申请。
  comapi = std::shared_ptr<OKCoinWebSocketApiCom>(new OKCoinWebSocketApiCom(com_apiKey, com_secretKey));    //国际站
}

void
OkexMarketData::initOutThread()
{
  gOkex = this;
  comapi->SetCallBackOpen(com_callbak_open);
  comapi->SetCallBackClose(com_callbak_close);
  comapi->SetCallBackMessage(com_callbak_message);
}


}
}
