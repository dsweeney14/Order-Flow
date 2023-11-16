#include "OrderBook.h"
#include "Order.h"

// A function that adds the payloads from the WebSocket stream to a queue
void OrderBook::addPayload(const std::string& payload) {
    std::lock_guard<std::mutex> guard(queueMutex);
    payloadQueue.push(payload);
    queueCondVar.notify_one();
}

// A function that processes the incoming orders and initialises the main thread
void OrderBook::processOrders() {
    while (true) {
        std::string payload;
        {
            // Read orders from the queue
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondVar.wait(lock, [this] { return !this->payloadQueue.empty(); });
            payload = payloadQueue.front();
            payloadQueue.pop();
        }
        rapidjson::Document rawOrder;
        rawOrder.Parse(payload.c_str());

        // Handle a new order
        if (rawOrder.IsArray()) {
            std::string orderType = rawOrder[0].GetString();
            if (orderType == "open") {
                std::string orderId = rawOrder[3].GetString();
                std::string side = rawOrder[4].GetString();
                std::string priceString = rawOrder[5].GetString();
                std::string sizeString = rawOrder[6].GetString();
                std::string time = rawOrder[7].GetString();

                double price = std::stod(priceString);
                double size = std::stod(sizeString);

                std::istringstream iss(time);
                std::tm tm = {};
                std::chrono::system_clock::time_point timestamp;

                // Parse the date and time up until the fractional seconds
                iss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
                if (iss.fail()) {
                    std::cerr << "Failed to parse timestamp" << std::endl;
                }

                // Extract the fractional seconds manually
                char dot;
                unsigned int microseconds;
                iss >> dot >> microseconds;
                if (iss.fail() || dot != '.') {
                    std::cerr << "Failed to parse microseconds" << std::endl;
                }

                // Form the timestamp
                auto time_since_epoch = std::chrono::seconds(std::mktime(&tm));
                timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
                timestamp += std::chrono::microseconds(microseconds);

                // Create a new order object and add it to the raw orders map
                Order newOrder(orderId, side, price, size, timestamp);
                std::lock_guard<std::mutex> guard(orderBookMutex);
                orderBook[orderId] = newOrder;
                // Send the order object to be added to ordered and unordered maps
                addOrderToMaps(newOrder);
            }
            else if (orderType == "done") {
                // Remove an order from the raw orders map if the order type is "done"
                std::string orderId = rawOrder[3].GetString();
                std::lock_guard<std::mutex> guard(orderBookMutex);
                auto orderToRemove = orderBook.find(orderId);
                if (orderToRemove != orderBook.end()) {
                    // Send the order ID to a function that also removes the order from the ordered and unordered maps
                    removeOrderFromMaps(orderId);
                    orderBook.erase(orderToRemove);
                }
            }
        }
    }
}

// A function that adds incoming orders to maps
void OrderBook::addOrderToMaps(const Order& order) {
    try {
        // Add the incoming order to the multimap that uses the timestamp of the order as the iterator
        auto timeIterator = orderSortedByTime.emplace(order.timestamp, order);
        // Also add the incoming order to the unordered map that uses the order ID as the iterator
        ordersById[order.orderId] = timeIterator;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught in addOrderToMaps: " << e.what() << std::endl;
    }
}

// A function that removes orders from the unordered map and multimap
void OrderBook::removeOrderFromMaps(const std::string& orderId) {
    try {
        auto idIterator = ordersById.find(orderId);
        if (idIterator != ordersById.end()) {
            orderSortedByTime.erase(idIterator->second);
            ordersById.erase(idIterator);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught in removeOrderFromMaps: " << e.what() << std::endl;
    }
}

// A function that calculates the price pressure in the market 
int OrderBook::calculateOrderFlow() {
    std::lock_guard<std::mutex>guard(orderBookMutex);
    int netPressure = 0;
    int numberOfObservations = 0;

    // Define the look back period to 1 minute
    auto timeNow = std::chrono::system_clock::now();
    auto oneMinuteInterval = timeNow - std::chrono::minutes(1);

    // Read the orderSortedByTime in reverse
    for (auto latestOrder = orderSortedByTime.rbegin(); latestOrder != orderSortedByTime.rend(); ++latestOrder) {
        const std::chrono::system_clock::time_point& orderTime = latestOrder->first;
        const Order& order = latestOrder->second;

        // Calculate the weighted price pressure
        if (order.timestamp > oneMinuteInterval) {
            double orderWeightedSize = order.size * order.price;
            if (order.side == "buy") {
                netPressure += orderWeightedSize;
            }
            else if (order.side == "sell") {
                netPressure -= orderWeightedSize;
            }

            numberOfObservations++;
        }
        else {
            if (order.timestamp < oneMinuteInterval) {
                break;
            }
        }
    }
    // Normalise the price pressure by n number of observations in the lookback period
    if (numberOfObservations > 0) {
        return netPressure / numberOfObservations;
    }
    else {
        return 0;
    }
}

// A function to initialise the thread to constantly monitor order flow
void OrderBook::orderFlowCalculationThread() {
    while (true) {
        double marketPressure = this->calculateOrderFlow();
        std::cout << "Order Flow: " << marketPressure << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}