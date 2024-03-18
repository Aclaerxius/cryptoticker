#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "include/VariadicTable.h"
#include "include/HTTPRequest.hpp"
#include "include/color.hpp"
#include "include/json.hpp"

using json = nlohmann::json;

void error(const std::string message, const UserConfig config)
{
    if (config.ignore_message)
    {
        return 0;
    }
    else
    {
        std::cout << "Error: incorect option" << std::endl;
        // helpMenu
        return 1;
    }
};

const std::string valid_sort_columns[] = {"symbol", "priceChange", "priceChangePercent", "quoteVolume", "lastId", "trend"};

class Sort
{ //
};

class UserConfig
{ //
};

class BiannceDataModel
{

    std::string symbol = valid_sort_columns[0],
                std::double priceChange,
                std::double priceChangePercent,
};

class RequestManager
{
    std::ifstream f("https://api2.binance.com/api/v3/ticker/24hr");
    json data = json::parse(f);
};

class OutputModel
{ //
};

class OutputComputationManager
{ //
};

class DisplayManager
{ //
};

int main(int argc, char *argv[])
{

    return 0;
}