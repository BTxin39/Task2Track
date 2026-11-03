#include "task.h"

Task::Task(int id, std::string description)
    :id(id), description(description) {
        this->status = TaskStatus::TO_DO;
        this->created_time = std::time(nullptr);
        this->updated_time = this->created_time;
}

Task::Task(int id, std::string description, TaskStatus status, std::time_t created_time, std::time_t updated_time)
    :id(id), description(description), status(status), created_time(created_time), updated_time(updated_time) {
}

Task::~Task() {}

int Task::get_id() {
    return this->id;
}

std::string Task::get_description() {
    return this->description;
}

TaskStatus Task::get_status() {
    return this->status;
}

std::time_t Task::get_created_time() {
    return this->created_time;
}

std::time_t Task::get_updated_time() {
    return this->updated_time;
}

void Task::update_updated_time(const std::string new_updated_time) {
    const std::time_t parsed_time = string_2_time(new_updated_time);
    if (parsed_time == static_cast<std::time_t>(-1)) {
        throw std::invalid_argument("invalid time format");
    }
    this->updated_time = parsed_time;
}

void Task::update_status(const std::string new_updated_status) {
    this->status = string_2_status(new_updated_status);
}

void Task::update_description(const std::string new_description) {
    this->description = new_description;
}