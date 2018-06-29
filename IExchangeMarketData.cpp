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

namespace newton
{
namespace exmdapi
{


}
}