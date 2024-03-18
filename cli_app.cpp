#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *data)
{
    data->append((char *)contents, size * nmemb);
    return size * nmemb;
}

class BinanceAPI
{
private:
    std::string dataFile = "binance_data.json";

public:
    std::string fetchDataFromAPI()
    {
        CURL *curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "https://api2.binance.com/api/v3/ticker/24hr");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }

            curl_easy_cleanup(curl);
        }

        // Save data to file for trend calculation persistence
        std::ofstream outFile(dataFile);
        if (outFile.is_open())
        {
            outFile << readBuffer;
            outFile.close();
        }
        else
        {
            std::cerr << "Error: Unable to open file for writing data." << std::endl;
        }

        return readBuffer;
    }

    json getDataFromFile()
    {
        std::ifstream inFile(dataFile);
        json jsonData;

        if (inFile.is_open())
        {
            inFile >> jsonData;
            inFile.close();
        }
        else
        {
            std::cerr << "Error: Unable to open file for reading data." << std::endl;
        }

        return jsonData;
    }
};

class Table
{
private:
    std::vector<std::string> data;

public:
    Table(std::vector<std::string> data) : data(data) {}

    void displayTable()
    {
        // Implement displaying table
        // Here you can parse JSON data and display in a tabular format
        for (const auto &row : data)
        {
            std::cout << row << std::endl;
        }
    }
};

class CLI
{
public:
    void parseOptions(int argc, char *argv[])
    {
        // Implement option parsing here
        // Use argc and argv to parse command line options
        // Example: --sort col1 asc --limit 10 --pairs BTC ETH --ignore-errors

        // For simplicity, let's assume a few options
        bool hasSort = false;
        bool hasLimit = false;
        bool hasPairs = false;
        bool hasIgnoreErrors = false;
        bool hasLive = false;
        bool hasTrend = false;

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "--sort")
            {
                // Parse sorting options
                hasSort = true;
                // Implement sorting logic
            }
            else if (arg == "--limit")
            {
                // Parse limit option
                hasLimit = true;
                // Implement limit logic
            }
            else if (arg == "--pairs")
            {
                // Parse pairs option
                hasPairs = true;
                // Implement pairs logic
            }
            else if (arg == "--ignore-errors")
            {
                hasIgnoreErrors = true;
            }
            else if (arg == "--live")
            {
                hasLive = true;
                // Implement live logic
            }
            else if (arg == "--trend")
            {
                hasTrend = true;
                // Implement trend logic
            }
            else
            {
                // Invalid option
                std::cerr << "Error: Invalid option - " << arg << std::endl;
                // Implement error handling
            }
        }

        // Now you can use these flags to customize behavior
    }
};

class DataProcessor
{
public:
    static std::vector<std::string> processJsonData(json &jsonData, bool hasTrend)
    {
        std::vector<std::string> tableData;

        for (const auto &item : jsonData)
        {
            std::string symbol = item["symbol"].get<std::string>();
            double price = std::stod(item["lastPrice"].get<std::string>());
            double priceChangePercent = std::stod(item["priceChangePercent"].get<std::string>());
            double volume = std::stod(item["quoteVolume"].get<std::string>());
            int numOfTrades = std::stoi(item["lastId"].get<std::string>());

            std::string priceChangeColor = getPriceChangeColor(priceChangePercent);
            std::string trendColor = getTrendColor(hasTrend, symbol);

            std::string trend = "";
            if (hasTrend)
            {
                double calculatedTrend = calculateTrend(symbol);
                trend = ", Trend: " + std::to_string(calculatedTrend);
                trendColor = getGradientColor(calculatedTrend, -100, 100);
            }

            std::string row = "Symbol: " + symbol + ", Price: " + std::to_string(price) + ", Price Change: " +
                              priceChangeColor + std::to_string(priceChangePercent) + "%" + "\033[0m" +
                              ", Volume: " + std::to_string(volume) + ", Num of Trades: " +
                              std::to_string(numOfTrades) + trendColor + trend + "\033[0m";

            tableData.push_back(row);
        }

        return tableData;
    }

private:
    static std::string getPriceChangeColor(double priceChangePercent)
    {
        if (priceChangePercent > 0)
        {
            return "\033[1;32m"; // Green color
        }
        else if (priceChangePercent < 0)
        {
            return "\033[1;31m"; // Red color
        }
        else
        {
            return "\033[0m"; // Reset color
        }
    }

    static std::string getTrendColor(bool hasTrend, const std::string &symbol)
    {
        if (!hasTrend)
        {
            return "";
        }

        // Fetch trend data from persistent storage
        double calculatedTrend = calculateTrend(symbol);
        return getGradientColor(calculatedTrend, -100, 100);
    }

    static std::string getGradientColor(double value, double min, double max)
    {
        // Calculate gradient color based on value between min and max
        double ratio = (value - min) / (max - min);
        int r = 255 * ratio;
        int g = 255 * (1 - ratio);
        return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";0m";
    }

    static double calculateTrend(const std::string &symbol)
    {
        // Dummy trend calculation, you can implement actual trend calculation logic here
        // For demonstration, we'll use a simple random value between -100 and 100
        srand(time(NULL));
        return rand() % 201 - 100;
    }
};

int main(int argc, char *argv[])
{
    CLI cli;
    cli.parseOptions(argc, argv);

    BinanceAPI binanceAPI;
    std::string jsonData = binanceAPI.fetchDataFromAPI();

    // Parse JSON data
    json parsedData = json::parse(jsonData);

    // Prepare data for Table class
    std::vector<std::string> tableData = DataProcessor::processJsonData(parsedData, false); // Change false to true for trend

    Table table(tableData);
    table.displayTable();

    return 0;
}
