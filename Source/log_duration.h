#pragma once
#include <chrono>
#include <iostream>


using namespace std;
using namespace chrono;
using namespace literals;

#define PROFILE_CONCAT_INTERNAL(X, Y) X##Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)
#define LOG_DURATION_STREAM(x, out_stream) LogDuration UNIQUE_VAR_NAME_PROFILE(x, out_stream)

class LogDuration {
public:
    LogDuration(const std::string& id, std::ostream& out = std::cerr)
        : id_(id), out_(out) {
    }

    ~LogDuration() {
        const auto end_time = steady_clock::now();
        const auto dur = end_time - start_time_;
        out_ << id_ << ": "s << duration_cast<milliseconds>(dur).count() << " ms"s << endl;
    }

private:
    const std::string id_;
    std::ostream& out_;
    const steady_clock::time_point start_time_ = steady_clock::now();
};