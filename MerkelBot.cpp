#include "MerkelBot.h"
#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

MerkelBot::MerkelBot() {

}

void MerkelBot::init() {
    //start clock from first time
    currentTime = orderBook.getEarliestTime();
    //creates list with all existing products
    std::vector<std::string> knowprods = orderBook.getKnownProducts();
    //insert currency to start trading
    wallet.insertCurrency("BTC",  1);
    wallet.insertCurrency("ETH",  10);
    wallet.insertCurrency("USDT", 2000);
    wallet.insertCurrency("DOGE", 10000); 
    logWallet(wallet);

    std::cout << "###########" << std::endl;
    std::cout << wallet;
    std::cout << "###########" << std::endl;

    //As we currently don't have any data, we require at least 30 timeframes to be able to predict the future market with some certainty
    for(int i = 0; i < 30; i++) {
        gotoNextTimeframe();
    }

    while(true) {
        //iterates over all products
        for(std::string prod : knowprods) {
            //checks product by product market movement..
            bool bull = isBullish(prod);
            //..if positive, place ask to buy
            if(bull) {
                //creates list of all bids from current time
                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid, prod, currentTime);
                //find the most people will pay for the currency you are selling
                double priceRequested = orderBook.getHighPrice(entries);
                //calculates amount, it will be a percentage of the product
                std::string baseProduct = tokenise(prod,0);
                double amount = calculateTradeAmount(baseProduct);
                //enter ask into orderbook
                enterAsk(prod, priceRequested, amount);
                //log into external file
                logOrders(prod, amount, priceRequested, "Ask");
            } else {
                //creates list of all asks from current time
                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, prod, currentTime);
                //find the least people will sell their currency for
                double priceRequested = orderBook.getLowPrice(entries);
                //calculates amount, it will be a percentage of the product
                std::string baseProduct = tokenise(prod,1);
                double amount = calculateTradeAmount(baseProduct);
                //enter bid into orderbook
                enterBid(prod, priceRequested, amount);
                //log into external file
                logOrders(prod, amount, priceRequested, "Bid");
            }
        }
        //step to next timeframe
        gotoNextTimeframe();
        std::cout << "###########" << std::endl;
        std::cout << wallet;
        std::cout << "###########" << std::endl;
        logWallet(wallet);
    }
}
void MerkelBot::gotoNextTimeframe()
{
    //iterates over all five existing products
    for (std::string p : orderBook.getKnownProducts())
    {
        //find any possible sales and stores them into an array
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        for (OrderBookEntry& sale : sales)
        {
            //if sale was made by bot..
            if (sale.username == "simuser")
            {
                //..remove currency from wallet
                wallet.processSale(sale);
            }
        }
    }
    //step to next timeframe
    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelBot::enterAsk(std::string product, double price, double amount) {
    OrderBookEntry obe = OrderBookEntry(price, amount, currentTime, product, OrderBookType::ask);
    //creates variable type OrderBookEntry from parameters sent
    obe.username = "simuser";
    //checks if ask was sent by bot
    if (wallet.canFulfillOrder(obe)) //checks if bot owns currency
    {
        orderBook.insertOrder(obe); //insert order into OrderBook
    }
}
void MerkelBot::enterBid(std::string product, double price, double amount) {
    OrderBookEntry obe = OrderBookEntry(price, amount, currentTime, product, OrderBookType::bid);
    //creates variable type OrderBookEntry from parameters sent
    obe.username = "simuser";
    //checks if bid was sent by bot
    if (wallet.canFulfillOrder(obe))//checks if bot owns currency
    {
        orderBook.insertOrder(obe);//insert order into OrderBook
    }
}
bool MerkelBot::isBullish(std::string product) {
    //returns boolean -> if SMA(10) < SMA(20) if true, market is moving upwards
    return orderBook.getSMA(product,currentTime,10) < orderBook.getSMA(product,currentTime,20);
}
std::string MerkelBot::tokenise(std::string prods, int pos) {
    std::string temp;
    std::vector<std::string> tokens;
    std::istringstream isString(prods);

    while(std::getline(isString, temp, '/')) {
        tokens.push_back(temp);
    }
    //return token at position requested
    return tokens[pos];
    // https://www.cplusplus.com/reference/sstream/istringstream/istringstream/
}
double MerkelBot::calculateTradeAmount(std::string currency) {
    double amount = 0.1 * wallet.containsCurrency(currency); //Return amount, atm using 10% of total to trade
    return amount;
}

void MerkelBot::logOrders(std::string currencies, double amount, double price, std::string orderType) {
    std::ofstream logOut; //create variable
    logOut.open("logorders", std::ios_base::app); //open file
    logOut << currentTime << " " << orderType << " " << currencies << " Quantity: " << amount << " @" << price << std::endl << std::endl; //print line into external file
}
void MerkelBot::logWallet(Wallet wallet) {
    std::ofstream logOut; //create variable
    logOut.open("logwallet", std::ios_base::app); //open file
    logOut << "At: "<< currentTime << std::endl; //print line into external file
    logOut << wallet << std::endl;
}