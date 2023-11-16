#include "Order.h"

Order::Order(const std::string& orderId, const std::string& side,
    double price, double size, const std::chrono::system_clock::time_point& timestamp)
    : orderId(orderId), side(side), price(price), size(size), timestamp(timestamp) {}