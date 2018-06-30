//
// Created by huang on 6/23/18.
//

/*
Binance example:

BinaCPP_websocket::init();

BinaCPP_websocket::connect_endpoint( ws_aggTrade_OnData ,"/ws/bnbbtc@aggTrade" );
BinaCPP_websocket::connect_endpoint( ws_userStream_OnData , ws_path.c_str() );
BinaCPP_websocket::connect_endpoint( ws_klines_onData ,"/ws/bnbbtc@kline_1m" );
BinaCPP_websocket::connect_endpoint( ws_depth_onData ,"/ws/bnbbtc@depth" );

BinaCPP_websocket::enter_event_loop();
*/

#include "IExchangeMarketData.h"

using json = nlohmann::json;

namespace nlohmann
{
template<typename KEY, typename VAL>
struct adl_serializer<std::map<KEY, VAL>> {
  static void
  to_json (json &j, const std::map<KEY, VAL> _map)
  {
    j = json::array ();
    size_t sz = _map.size ();
    int count = 0;
    for (auto &p: _map)
      {
        double price = p.first;
        double vol = p.second;
        json jitem = json::array ();
        jitem[0] = price;
        jitem[1] = vol;
        j[count++] = jitem;
      }

  }
  static void
  from_json (const nlohmann::json &j, std::map<KEY, VAL> &sh_ptr)
  {
    //not implemented
  }
};
}

namespace newton
{
namespace exmdapi
{
void
to_json (json &j, const Depth &p)
{
  j = json{{"ex",   c_str (p.ex)},
           {"pair", c_str (p.cryptoPair)},
           {"tm",   p.timestamp},
           {"bids", p.mBid},
           {"asks", p.mAsk}};
}

void
from_json (const json &j, Depth &p)
{
  //not implemented
}

void
to_json (nlohmann::json &j, const Trade &p)
{
  j = json{{"ex",    c_str (p.ex)},
           {"pair",  c_str (p.cryptoPair)},
           {"id",    p.id},
           {"tm",    p.timestamp},
           {"prc",   p.price},
           {"vol",   p.volume},
           {"isbuy", p.isbuy}};
}

void
from_json (const json &j, Trade &p)
{
  //not implemented
}

}
}