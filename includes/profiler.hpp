//  Copyright 2026 Filippo Savi
//  Author: Filippo Savi <filssavi@gmail.com>
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef ANANKE_PROFILER_HPP
#define ANANKE_PROFILER_HPP

#include <iostream>
#include <fstream>
#include <chrono>
#include <mutex>
#include <string>
#include <source_location>
#include <vector>

class ProfileLogger {
public:
    struct LogEntry {
        std::string event_name;
        std::string caller_function;
        uint32_t line;
        uint64_t timestamp_ns;
    };

    static ProfileLogger& Get() {
        static ProfileLogger instance;
        return instance;
    }


    void Log(const std::string& event_name,
             const std::source_location location = std::source_location::current()) {


        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

        // Thread-safe push into our in-memory vector
        std::lock_guard lock(mutex_);
        logs_.push_back({
            event_name,
            location.function_name(),
            location.line(),
            static_cast<uint64_t>(ns)
        });
    }

private:
    std::vector<LogEntry> logs_;
    uint64_t initial_timestamp;
    std::mutex mutex_;

    ProfileLogger() {
        logs_.reserve(100'000);
        auto now = std::chrono::high_resolution_clock::now();
        initial_timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    }

    ~ProfileLogger() {
        std::ofstream f("profile_points.csv");
        f << std::fixed << std::setprecision(3);
        f << "Timestamp(ns),Event,Caller,Line\n";

        for (const auto& entry : logs_) {
            uint64_t delta_ns = entry.timestamp_ns - initial_timestamp;
            double delta_ms = static_cast<double>(delta_ns) / 1'000'000.0;
            f << fmt::format("{},{},{}:{}", delta_ms, entry.event_name, entry.caller_function,entry.line) << "\n";
        }
    }

    ProfileLogger(const ProfileLogger&) = delete;
    ProfileLogger& operator=(const ProfileLogger&) = delete;
};


#define LOG_TIMEPOINT(name) ProfileLogger::Get().Log(name)

#endif //ANANKE_PROFILER_HPP
