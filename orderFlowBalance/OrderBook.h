#pragma once

#include "Order.h"
#include <map>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <windows.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <map>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <rapidjson/document.h>


class OrderBook
{
private:
    std::map<std::string, Order> orderBook;
    std::multimap<std::chrono::system_clock::time_point, Order> orderSortedByTime;
    std::unordered_map<std::string, decltype(orderSortedByTime)::iterator> ordersById;
    std::thread orderBookThread;
    std::mutex orderBookMutex;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;
    std::queue<std::string> payloadQueue;

public:
    void processOrders();
    void addOrderToMaps(const Order& order);
    void removeOrderFromMaps(const std::string& orderId);
    int calculateOrderFlow();
    void addPayload(const std::string& payload);
    void orderFlowCalculationThread();
};

