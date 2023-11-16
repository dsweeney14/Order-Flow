#include "HandlePayload.h"

void WebSocketClient::handlePayload(const std::string& payload) {
    rapidjson::Document payloadObject;
    payloadObject.Parse(payload.c_str());

    if (payloadObject.IsArray()) {
        if (payloadObject[0].IsString() && payloadObject[0] == "open") {
            std::string type = payloadObject[0].GetString();
            std::string orderId = payloadObject[3].GetString();
            std::string side = payloadObject[4].GetString();
            std::string price = payloadObject[5].GetString();
            std::string size = payloadObject[6].GetString();
            std::string time = payloadObject[7].GetString();

            ordersBook[orderId] = std::make_tuple(type, side, price, size, time);
        }
        else if (payloadObject[0].IsString() && payloadObject[0] == "done") {
            std::string orderId = payloadObject[3].GetString();

            auto it = ordersBook.find(orderId);
            if (it != ordersBook.end()) {
                ordersBook.erase(it);
                std::cout << "Order: " << orderId << " removed from order book." << std::endl;
            }
        }
    }

    //for (const auto& order : ordersBook) {

    //    const auto& orderDetails = order.second;

    //    // Extract individual elements from the tuple
    //    const std::string& type = std::get<0>(orderDetails);
    //    const std::string& side = std::get<1>(orderDetails);
    //    const std::string& priceString = std::get<2>(orderDetails);
    //    const std::string& sizeString = std::get<3>(orderDetails);
    //    const std::string& timeString = std::get<4>(orderDetails);

    //    double totalBidSize = 0.0;
    //    double totalAskSize = 0.0;


    //    std::tm tm = {};
    //    std::istringstream iss(timeString);
    //    iss >> std::get_time(&tm, "%y-%m-%d %h:%m:%s");
    //    std::chrono::system_clock::time_point orderTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    //    // convert ordertime to unix timestamp in milliseconds
    //    auto orderTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(orderTime.time_since_epoch()).count();

    //    // define the 60 second lookback period
    //    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    //    std::chrono::system_clock::time_point windowStart = currentTime - std::chrono::seconds(10);

    //    // convert to unix (for ease of computation)
    //    auto currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();
    //    auto windowStartMs = std::chrono::duration_cast<std::chrono::milliseconds>(windowStart.time_since_epoch()).count();


    //    if (orderTimeMs >= currentTimeMs - windowStartMs) {
    //        if (side == "buy") {
    //            double priceInt = std::stod(priceString);
    //            double sizeInt = std::stod(sizeString);
    //            // calculate weighted bid size
    //            double bidsize = priceInt * sizeInt;
    //            totalBidSize += bidsize;

    //        }
    //        else if (side == "sell") {
    //            double price = std::stod(priceString);
    //            double size = std::stod(sizeString);
    //            // calculate weighted ask price
    //            double asksize = price * size;
    //            totalAskSize += asksize;
    //        }
    //    }
    //    double OFI = (totalBidSize - totalAskSize) / (totalBidSize + totalAskSize);
    //    std::cout << "OFI: " << OFI << std::endl;
    //}
}