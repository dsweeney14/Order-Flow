#include "Order.h"
#include "OrderBook.h"


int main() {
    // Initate the pipe
    HANDLE hPipe;
    DWORD dwRead;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // Connect to the pipe
    hPipe = CreateFile(TEXT("\\\\.\\pipe\\coinbase_pipe"),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to open pipe. GLE=" << GetLastError() << std::endl;
        return -1;
    }

    std::cout << "Pipe connected." << std::endl;

    // Initalise threads 
    OrderBook orderBook;
    std::thread orderBookThread(&OrderBook::processOrders, &orderBook);

    std::thread orderFlowThread(&OrderBook::orderFlowCalculationThread, &orderBook);

    // Read data from the pipe
    while (true) {
        bool success = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);

        if (!success || dwRead == 0) {
            break;
        }
        buffer[dwRead] = '\0';
        orderBook.addPayload(std::string(buffer));
    }

    // Join threads
    if (orderBookThread.joinable()) {
        orderBookThread.join();
    }

    if (orderFlowThread.joinable()) {
        orderFlowThread.join();
    }

    CloseHandle(hPipe);
    return 0;
}
