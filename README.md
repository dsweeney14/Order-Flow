Order Flow Analyser

Overview

The Order Flow Analyser is a sophisticated tool designed to monitor and analyse real-time trading data, specifically focusing on the cryptocurrency market. It efficiently processes live order data and calculates the Order Flow. This metric provides valuable insights into market dynamics, revealing the underlying buying and selling pressure within a specified timeframe.

Key Features
1.	Real-Time Data Processing: Leverages a continuous feed of order data through a named pipe, ensuring up-to-date market analysis.
2.	Concurrent Order Processing: Utilises multi-threading to handle incoming order data and calculation of order flow simultaneously, ensuring efficient data handling without bottlenecks.
3.	Order Flow Calculation: Analyses the last minute of order data to calculate the net pressure exerted by buy and sell orders, offering a snapshot of market sentiment.
4.	Normalised Metrics: Scales the order flow by the number of transactions in the given timeframe, providing a normalised view of market pressure per order, which aids in comparative analysis across varying market activities.

Technical Implementation

•	Language: Developed in C++, taking advantage of its efficient memory management and multi-threading capabilities.

•	Concurrency Control: Implements std::mutex and std::thread to manage concurrent access to shared resources, ensuring thread safety.

•	Data Structures: Utilises std::map and std::multimap for efficient data storage and retrieval, sorting orders based on timestamps.

•	Error Handling: Robust error handling mechanisms to ensure stability and reliability during runtime.

Usage

The application connects to a data source through a named pipe, continuously receiving and processing new order data. It maintains an updated order book in real-time, and a separate thread calculates the order flow metric. This calculation is based on orders within the last minute, providing a near real-time analysis of market trends.
