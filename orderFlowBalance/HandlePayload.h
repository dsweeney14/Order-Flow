#pragma once
#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include <map>
#include <iomanip>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

#pragma comment(lib, "crypt32.lib")

class WebSocketClient {
private:
    std::map<std::string, std::tuple<std::string, std::string, std::string, std::string, std::string>> ordersBook;

public:
    void handlePayload(const std::string& payload);
};

#endif
