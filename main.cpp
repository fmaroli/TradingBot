#include <iostream>
#include "Wallet.h"
#include "MerkelMain.h"
#include "MerkelBot.h"

#include <string>
#include "OrderBook.h"
#include "OrderBookEntry.h"

int main()
{
    //choose between running Main app or automatic trading with bot
    std::cout << "Please choose between:" << std::endl;
    std::cout << "1- Main app || 2- Bot app" << std::endl;
    std::string input_string;
    std::getline(std::cin, input_string);
    int input = std::stoi(input_string);

    MerkelMain appMain{};
    MerkelBot appBot{};
    //Once selection is made, to change, restart aplication
    switch(input) {
        case 1:
            appMain.init();  
        case 2:
            appBot.init();
    }
}