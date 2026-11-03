#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include "task.h"

class TaskManager {
public:
    TaskManager(const std::filesystem::path filename);
    ~TaskManager();
    TaskManager(const TaskManager&) = delete;
    TaskManager& operator= (const TaskManager&) = delete;

    Task* add_task(std::string description);
    bool remove_task(int id);
    bool remove_all_tasks();
    bool remove_last_task();
    Task* get_task(int id);
    Task* update_task_status(int id, std::string new_status);
    Task* update_task_description(int id, std::string new_description);

    std::vector<const Task*> list_task();
    std::vector<const Task*> list_task(TaskStatus status);

    int get_last_task_id();

private:
    std::vector<std::unique_ptr<Task>> tasks;
    std::filesystem::path filename;
    int next_id = 1;

    void ensure_file_exist(std::filesystem::path);
    void save_to_file(std::filesystem::path);
    void load_from_file(std::filesystem::path);
    void ensure_file_exist();
    void save_to_file();
    void load_from_file();
};