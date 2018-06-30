//
// Created by phuang on 6/26/2018.
//

#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
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
parseChannel (string channel)
{
  //depth last char is '0' ("ok_sub_spot_btc_usdt_depth_20")
  //trade last char is 's' ("ok_sub_spot_btc_usdt_deals")
  char c = channel.at (channel.size () - 1);
  if ('0' == c || '5' == c)
    return DEPTH;
  else if ('s' == c)
    return TRADE;
  else
    throw runtime_error ("okex unknown channel, " + channel);
}
/*
 * From string value input
 * "ok_sub_spot_btc_usdt_depth_20"
 * "ok_sub_spot_btc_usdt_deals"
 * find out which crypto pair
 */
CryptoPairEnum
parseCryptoPair (string channel)
{
  vector<string> vStr;
  boost::split (vStr, channel, boost::is_any_of ("_"));
  string nm = vStr[3] + "_" + vStr[4];
  if ("btc_usdt" == nm)
    return BTC_USDT;
  else if ("eth_usdt" == nm)
    return ETH_USDT;
  else if ("eth_btc" == nm)
    return ETH_BTC;
  else
    throw runtime_error ("okex unknown crypto pair, " + nm);
}

/*
 * This is a depth message
 *
 *
 * [{"binary":0,"channel":"ok_sub_spot_btc_usdt_depth_20","data":
 * {"asks":[["6400.7321","0.23"],["6400.5316","3"],["6400.394","0.05"],["6400.2015","0.177"],["6399.9109","0.8"],
 * ["6399.0495","0.8"],["6398.3121","0.8"],["6397.6135","0.87"],["6397.5719","0.8"],["6397.5281","0.35818531"],
 * ["6397.4247","0.13"],["6397.3774","0.70527035"],["6396.9692","0.87"],["6396.7115","0.13"],["6396.7104","0.399"],
 * ["6395.9515","0.02"],["6394.9717","0.23607631"],["6394.8942","0.17"],["6394.3335","2.42546898"],["6394.2335","0.09999999"]],
 * "bids":[["6391.4728","0.8"],["6391.2503","0.17"],["6391.2304","0.02"],["6390.8977","1"],["6390.8319","0.8"],
 * ["6390.0994","0.37122337"],["6389.8","0.848"],["6389.6062","0.04251002"],["6386.9466","0.05"],["6386.7766","0.155"],
 * ["6386.7063","0.05000002"],["6386.5781","0.158"],["6386.5342","0.05000002"],["6386.3795","0.03100002"],
 * ["6386.1897","0.00472295"],["6386.0203","0.29472964"],["6385.2018","3"],["6385.1393","0.07720001"],
 * ["6385","0.04524424"],["6384.0993","0.069"]],"timestamp":1530343384314}}]
 */

Depth
parseDepth (json jmsg)
{
  Depth dp;

  json array = jmsg.at ("data").at ("bids");
  for (int i = 0; i < array.size (); i++)
    {
      //cout<<array[i]<<endl;
      json item = array[i];
      double price = std::stod (item[0].get<string> ());
      double vol = std::stod (item[1].get<string> ());
      //printf("%.8f, %.8f\n", price, vol);
      dp.mBid.insert (make_pair (price, vol));
    }

  array = jmsg.at ("data").at ("asks");
  for (int i = 0; i < array.size (); i++)
    {
      //cout<<array[i]<<endl;
      json item = array[i];
      double price = std::stod (item[0].get<string> ());
      double vol = std::stod (item[1].get<string> ());
      //printf("%.8f, %.8f\n", price, vol);
      dp.mAsk.insert (make_pair (price, vol));
    }

  dp.ex = ExchangeEnum::OKEX;
  //parse string to get crypto pair
  string channel = jmsg.at ("channel").get<string> ();
  dp.cryptoPair = parseCryptoPair (channel);
  //timestamp
  dp.timestamp = jmsg.at ("data").at ("timestamp").get<long> ();

  return dp;
}

/*
 * This is a trade msg
 *
 * [{"binary":0,"channel":"ok_sub_spot_btc_usdt_deals",
 * "data":[["425965263","6395.004","0.0413","14:45:28","ask"],
 * ["425965268","6395.004","0.14555093","14:45:28","ask"],
 * ["425965307","6395.004","0.0413","14:45:30","ask"],
 * ["425965309","6395.004","0.26479009","14:45:30","ask"],
 * ["425965338","6395.004","0.0168","14:45:33","ask"],
 * ["425965340","6395.004","0.09007756","14:45:33","ask"],
 * ["425965382","6395.0041","0.0046","14:45:35","ask"],
 * ["425965456","6398.4202","0.091","14:45:39","bid"],
 * ["425965458","6398.4243","0.17","14:45:39","bid"],
 * ["425965460","6398.4244","0.139","14:45:39","bid"],
 * ["425965462","6398.4244","0.39265246","14:45:39","bid"],
 * ["425965464","6398.4244","0.47385723","14:45:39","bid"],
 * ["425965466","6398.4244","0.63034773","14:45:39","bid"],
 * ["425965495","6395.004","0.0228","14:45:40","ask"],
 * ["425965511","6397.5521","0.0082","14:45:41","ask"],
 * ["425965512","6397.5521","0.03139016","14:45:41","ask"],
 * ["425965544","6397.5522","0.17","14:45:43","ask"],
 * ["425965547","6397.5522","0.044","14:45:43","ask"],
 * ["425965549","6397.5522","0.002","14:45:43","ask"],
 * ["425965551","6397.5522","0.001","14:45:43","ask"],
 * ["425965554","6397.5522","0.001","14:45:43","ask"],
 * ["425965557","6397.5522","0.0061","14:45:43","ask"],
 * ["425965562","6397.5522","0.03331436","14:45:43","ask"],
 * ["425965566","6397.5522","0.1444","14:45:43","ask"]]}]
 */
vector<Trade>
parseVecTrade (json jmsg)
{
  vector<Trade> vTrd;
  json array = jmsg.at ("data");
  for (int i = 0; i < array.size (); i++)
    {
      json item = array[i];
      //cout << item << endl;
      Trade td;
      td.ex = ExchangeEnum::OKEX;

      //parse string to get cryto pair
      string channel = jmsg.at ("channel").get<string> ();
      td.cryptoPair = parseCryptoPair (channel);

      //this is the id from okex, not from us
      td.id = stod (item[0].get<string> ());

      //get price and volume
      td.price = std::stod (item[1].get<string> ());
      td.volume = stod (item[2].get<string> ());

      //transfer "14:45:33" to a long timestamp
      int h, m, s = 0;
      sscanf (item[3].get<string> ().c_str (), "%d:%d:%d", &h, &m, &s);
      td.timestamp = h * 3600 + m * 60 + s;
      //isbuy = true means buy side initiated the trade, ie, item[4]== "bid"
      if ("bid" == item[4].get<string> ()) td.isbuy = true;
      else td.isbuy = false;

      vTrd.push_back (td);
    }

  return vTrd;

}

/*
 * These three callback functions are invoked by function pointers (see websocket.h).
 * Thus they can not be method functions of OkexMarketData.
 * We keep a raw pointer gOkex here for them to access the
 * instance.
 */
static OkexMarketData *gOkex;
void
com_callbak_open ()
{
  std::cout << "国际站连接成功！" << std::endl;
  //this_thread::sleep_for (chrono::seconds (3));
  auto comapi = gOkex->comapi;
  for (CryptoPairEnum p : gOkex->getCryptoPairs ())
    {
      switch (p)
        {
          case BTC_USDT:
            {
              //each of the following two will get 1 response from the server, as expected
              comapi->Emit ("ok_sub_spot_btc_usdt_depth_20");
              comapi->Emit ("ok_sub_spot_btc_usdt_deals");
              break;
            }
          case ETH_USDT:
            {
              //each of the following two will get 2 exactly the same responses from the server
              //don't know why
              comapi->Emit ("ok_sub_spot_eth_usdt_depth_20");
              comapi->Emit ("ok_sub_spot_eth_usdt_deals");
              break;
            }
          case ETH_BTC:
            {
              //each of the following two will get 2 exactly the same responses from the server
              //don't know why
              comapi->Emit ("ok_sub_spot_eth_btc_depth_20");
              comapi->Emit ("ok_sub_spot_eth_btc_deals");
              break;
            }
          //should not get here as sSubscribedCryptoPairs should not contain any pair not supported
          default:;
        }
    }
};

void
com_callbak_close ()
{
  std::cout << "连接已经断开！ " << std::endl;
};

static int count = 0;

void
com_callbak_message (const char *message)
{
  // test auto-reconnect, need to comment out in production
//  if (count++ == 5)
//  {
//    gOkex->comapi->Close();
//    count = 0;
//  }

  //std::cout << "Message: " << message << std::endl;
  json jmsg = json::parse (message)[0];
  //std::cout << jmsg << std::endl;
  string channel = jmsg.at ("channel").get<string> ();
  //filter out acknowledgments from okex
  if ("addChannel" == channel)
    {
      //std::cout << jmsg << std::endl;
      return;
    }
  //std::cout << "string: " << channel << std::endl;
  int ch = parseChannel (channel);
  if (DEPTH == ch)
    {
      Depth dp = parseDepth (jmsg);
      gOkex->onDepth (dp);
      return;
    }
  else if (TRADE == ch)
    {
      vector<Trade> vTd = parseVecTrade (jmsg);
      gOkex->onTrade (vTd);
    }
  else
    {

    }

};

//TODO ExchangeEnum::OKEX = 1
OkexMarketData::OkexMarketData () : IExchangeMarketData (ExchangeEnum::OKEX)
{
  std::string com_apiKey = "f99e1b5d-3433-47fe-b3b3-0aa1b8faa932";                 //请到www.okcoin.com申请。
  std::string com_secretKey = "894AA72C6D234B889A47E2A260581B7B";                  //请到www.okcoin.com申请。
  comapi = std::shared_ptr<OKCoinWebSocketApiCom> (new OKCoinWebSocketApiCom (com_apiKey, com_secretKey));    //国际站
}

void
OkexMarketData::initOutThread ()
{
  gOkex = this;
  comapi->SetCallBackOpen (com_callbak_open);
  comapi->SetCallBackClose (com_callbak_close);
  comapi->SetCallBackMessage (com_callbak_message);
}

}
}
