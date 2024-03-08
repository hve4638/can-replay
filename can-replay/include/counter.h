#pragma once
#include <chrono>

inline std::chrono::_V2::system_clock::time_point counter_start() {
    return std::chrono::high_resolution_clock::now();
}

inline unsigned long long counter_end(std::chrono::_V2::system_clock::time_point start) {
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed_nanoseconds = end - start;
    
    return elapsed_nanoseconds.count();
}