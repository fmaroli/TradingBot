#pragma once
#include <string>

#include "OrderBook.h"
#include "Wallet.h"

class MerkelBot {
    public:
        /** Constructor*/
        MerkelBot();
        /** Initialise*/
        void init();
        /** Insert ask into orderbook*/
        void enterAsk(std::string product, double price, double amount);
        /** Insert bid into orderbook*/
        void enterBid(std::string product, double price, double amount);
        /**Receives product and returns boolean -> if SMA(10) < SMA(20) if true, market is moving upwards*/
        bool isBullish(std::string product);
        /**Returns amount to be traded, currently trading with 10% of total amount*/
        double calculateTradeAmount(std::string currency);
        /**Returns base product after receiving product(eg: ETH/BTC)*/
        std::string tokenise(std::string prods, int pos);
        /**Log bids and asks into external file*/
        void logOrders(std::string currencies, double amount, double price, std::string orderType);
        /**Log wallet into external file*/
        void logWallet(Wallet wallet);
    private:
        /** Moves ahead to next timeframe*/
        void gotoNextTimeframe();

        //Current time will be stored here
        std::string currentTime;
        //Bot's wallet
        Wallet wallet;
        //Load all orders from CSV
        OrderBook orderBook{"20200601.csv"};
};