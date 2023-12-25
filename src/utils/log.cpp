#include <ctime>
#include "utils.h"
#include <iostream>
#include <chrono>


void timed_log(const std::string& content) {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time{};
    localtime_s(&local_time, &current_time);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "[%a %b %d %H:%M:%S %Y] ", &local_time);
    std::cout << buffer << content << std::endl;
}
