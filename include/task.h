#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

enum class TaskStatus{
    TO_DO,
    IN_PROGRESS,
    DONE
};

inline std::time_t string_2_time(std::string str) {
    std::string fmt = "%Y-%m-%d %H:%M:%S";
    std::tm tm{};
    tm.tm_isdst = -1;
    std::istringstream ss(str);
    ss >> std::get_time(&tm, fmt.c_str());
    if (ss.fail()) return static_cast<std::time_t>(-1);

    return std::mktime(&tm);
}

inline std::string time_2_string(std::time_t time) {
    const std::string fmt = "%Y-%m-%d %H:%M:%S";
    const std::tm* tm = std::localtime(&time);
    if (tm == nullptr) {
        throw std::invalid_argument("invalid time value");
    }

    std::ostringstream ss;
    ss << std::put_time(tm, fmt.c_str());
    return ss.str();
}

inline TaskStatus string_2_status(std::string str) {
    if (str == "TO_DO") return TaskStatus::TO_DO;
    else if (str == "IN_PROGRESS") return TaskStatus::IN_PROGRESS;
    else if (str == "DONE") return TaskStatus::DONE;
    throw std::invalid_argument("invalid status valid");
}

inline std::string status_2_string(TaskStatus status) {
    switch (status) {
        case TaskStatus::TO_DO: return "TO_DO";
        case TaskStatus::DONE: return "DONE";
        case TaskStatus::IN_PROGRESS: return "IN_PROGRESS";
    }
    throw std::invalid_argument("invalid task status");
}

class Task {
public:
    Task(int id, std::string description);
    Task(int id, std::string description, TaskStatus status, std::time_t created_time, std::time_t updated_time);
    ~Task();
    Task(const Task&) = delete;
    Task& operator= (const Task&) = delete;
    int get_id();
    std::string get_description();
    TaskStatus get_status();
    std::time_t get_created_time();
    std::time_t get_updated_time();
    void update_updated_time(const std::string);
    void update_status(const std::string);
    void update_description(const std::string);
private:
    int id;
    std::string description;
    TaskStatus status;
    std::time_t created_time;
    std::time_t updated_time;
};