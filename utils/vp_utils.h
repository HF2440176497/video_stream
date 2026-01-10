#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <regex>
#include <opencv2/imgproc.hpp>

// https://github.com/HowardHinnant/date/blob/master/include/date/date.h
#include "utils/date.h"


namespace utils {
    // string format in C++17
    template<typename ... Args>
    inline std::string string_format(const std::string& format, Args ... args){
        size_t size = 1 + snprintf(nullptr, 0, format.c_str(), args ...); 
        // unique_ptr<char[]> buf(new char[size]);
        char bytes[size];
        snprintf(bytes, size, format.c_str(), args ...);
        return std::string(bytes);
    }

    // use std::string::end_with(...) in standard library directly for C++20
    inline bool ends_with(std::string const & value, std::string const & ending) {
        if (ending.size() > value.size()) return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }

    // round double data using specific precision, return string.
    inline std::string round_any(double input, int precision) {
        input = input * std::pow(10, precision) + 0.5;
        auto output = std::to_string(std::floor(input) / std::pow(10, precision));

        output.erase(std::find_if(output.rbegin(), output.rend(), [](unsigned char ch) {
            return ch != '0';
        }).base(), output.end());

        // remove the last point `.` if possible
        if (ends_with(output, ".")) {
            output.pop_back();
        }
        
        return output;
    }

    inline std::vector<std::string> string_split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    // split time point to 7 parts, include year, month, day ... in order
    inline void time_split(system_clock::time_point tp, std::vector<int>& time_parts, int time_zone = 8) {
        // right time zone
        tp = tp + std::chrono::hours{time_zone};

        auto dp = date::floor<date::days>(tp);

        auto ymd = date::year_month_day{dp};
        auto time = date::make_time(std::chrono::duration_cast<std::chrono::milliseconds>(tp - dp));

        time_parts.clear();

        // push year/month/day
        time_parts.push_back(static_cast<int>(ymd.year()));
        time_parts.push_back(static_cast<unsigned>(ymd.month()));
        time_parts.push_back(static_cast<unsigned>(ymd.day()));

        // push hour/minute/second/milisecond
        time_parts.push_back(time.hours().count());
        time_parts.push_back(time.minutes().count());
        time_parts.push_back(time.seconds().count());
        time_parts.push_back(time.subseconds().count());
    }

    // format time point to string, chars in <> are keywords.
    // in: <year>-<mon>-<day> <hour>:<min>:<sec>.<mili>
    // out: 2022-10-08 13:53:07.230
    // or
    // in: [<day>/<mon>/<year> <hour>:<min>:<sec>.<mili>]
    // out: [08/10/2022 13:53:07.230]
    inline std::string time_format(system_clock::time_point tp, std::string template_str = "<year>-<mon>-<day> <hour>:<min>:<sec>.<mili>", int time_zone = 8) {
        // right time zone
        tp = tp + std::chrono::hours{time_zone};

        // 7 parts
        std::vector<int> time_parts;
        time_split(tp, time_parts, 0);

        auto time_str = std::regex_replace(template_str, std::regex("<year>"), std::to_string(time_parts[0]));
        time_str = std::regex_replace(time_str, std::regex("<mon>"), set_width_and_fill(std::to_string(time_parts[1]), 2));
        time_str = std::regex_replace(time_str, std::regex("<day>"), set_width_and_fill(std::to_string(time_parts[2]), 2));
        time_str = std::regex_replace(time_str, std::regex("<hour>"), set_width_and_fill(std::to_string(time_parts[3]), 2));
        time_str = std::regex_replace(time_str, std::regex("<min>"), set_width_and_fill(std::to_string(time_parts[4]), 2));
        time_str = std::regex_replace(time_str, std::regex("<sec>"), set_width_and_fill(std::to_string(time_parts[5]), 2));
        time_str = std::regex_replace(time_str, std::regex("<mili>"), set_width_and_fill(std::to_string(time_parts[6]), 3));

        return time_str;
    }

}