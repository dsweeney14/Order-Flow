#pragma once
#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <chrono>

class Order {
public: 
    std::string orderId;
    std::string side;
    double price;
    double size;
    std::chrono::system_clock::time_point timestamp;

    Order()
        : orderId("default"), side("default"),
        price(0.0), size(0.0), timestamp(std::chrono::system_clock::now()) {}

    Order(const std::string& orderId, const std::string& side,
        double price, double size, const std::chrono::system_clock::time_point& timestamp);
};

#endif
