#include <ctime>
#include "utils.h"
#include <iostream>
#include <chrono>


void timed_log(const std::string& content) {
    auto now = std::chrono::system_clock::now();
    std::time_t date_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&date_time);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
    std::cout << buffer << ": " << content << std::endl;
}
