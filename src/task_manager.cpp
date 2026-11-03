#include "task_manager.h"
#include <nlohmann/json.hpp>

TaskManager::TaskManager(const std::filesystem::path filename) 
    :filename(filename) {
        ensure_file_exist(filename);
        load_from_file(filename);
    }
    
TaskManager::~TaskManager() {
}

void TaskManager::save_to_file() {
    save_to_file(this->filename);
}

void TaskManager::load_from_file() {
    load_from_file(this->filename);
}

void TaskManager::ensure_file_exist() {
    ensure_file_exist(this->filename);
}

void TaskManager::ensure_file_exist(std::filesystem::path path) {
    const auto parent_path = path.parent_path();
    std::error_code ec;
    if (!parent_path.empty()) {
        std::filesystem::create_directories(parent_path, ec);
        if (ec) {
            throw std::filesystem::filesystem_error("create directories fail", parent_path, ec);
        }
    }
    std::ofstream ofs(path, std::ios::out | std::ios::app);
    if (!ofs) {
        throw std::runtime_error("cant open/create this file" + path.string());
    }
}

void TaskManager::load_from_file(std::filesystem::path path) {
    std::ifstream file(path);
    if (! file.is_open()) {
        throw std::runtime_error("cant open this file" + path.string());
    }

    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        file.close();
        return;
    }

    file.seekg(0, std::ios::beg);
    nlohmann::json j;

    try {
        file >> j;
        file.close();
        this->tasks.clear();
        this->next_id = 1;

        if (j.contains("next_id")) {
            next_id = j["next_id"].get<int>();
        }
        if(j.contains("tasks")) {
			for (const auto& item : j["tasks"]) {
				int id = item["id"];
				std::string description = item["description"];
				std::time_t created_at = item.value("created_at", std::time_t());
				std::time_t updated_at = item.value("updated_at", std::time_t());
				std::string status_str = item["status"];
				TaskStatus statu = string_2_status(status_str);
				std::unique_ptr<Task> task = std::make_unique<Task>(id, description, statu, created_at, updated_at);
				tasks.push_back(std::move(task));
			}
		}
    }
    catch( const nlohmann::json::parse_error& e) {
        std::cerr << "json error: " << filename << e.what() << std::endl;
    }
}

void TaskManager::save_to_file(std::filesystem::path path) {
    nlohmann::json j_root;

	j_root["next_id"] = next_id; 

	nlohmann::json j_tasks = nlohmann::json::array();

	for (const auto& task : tasks) {
		nlohmann::json j_task;

		if (!task) {
			continue; // Skip null tasks
		}

		j_task["id"] = task->get_id();
		j_task["description"] = task->get_description();
		j_task["status"] = status_2_string(task->get_status());
		j_task["created_at"] = task->get_created_time();
		j_task["updated_at"] = task->get_updated_time();
		j_tasks.push_back(j_task);
	}
	j_root["tasks"] = j_tasks;

	std::ofstream file(path);
	if(!file.is_open()) {
		std::cerr << "Failed to open file for writing: " << path.string() << std::endl;
		throw std::runtime_error("Could not open file for writing: " + path.string());
	}

	file << j_root.dump(4); // Pretty print with 4 spaces indentation
	file.close();
}

Task* TaskManager::add_task(std::string description) {
    this->tasks.emplace_back(std::make_unique<Task>(this->next_id, description));
    ++this->next_id;
    this->save_to_file(this->filename);
    return this->tasks.back().get();
}

bool TaskManager::remove_task(int id) {
    int result = std::erase_if(tasks, [id](const std::unique_ptr<Task>& del_task) {
        return del_task->get_id() == id;
    });
    this->save_to_file(this->filename);
    return result > 0;
}

bool TaskManager::remove_all_tasks() {
    if(tasks.empty()) {
        return false;
    }
    tasks.clear();
    this->save_to_file(this->filename);
    return true;
}

bool TaskManager::remove_last_task() {
    if(tasks.empty()) {
        return false;
    }
    tasks.pop_back();
    this->save_to_file(this->filename);
    return true;
}

Task* TaskManager::get_task(int id) {
    for (const auto& task:tasks) {
        if (task->get_id() == id) {
            return task.get();
        }
    }
    return nullptr;
}

Task* TaskManager::update_task_status(int id, std::string new_status) {
    auto task = get_task(id);
    if(task) {
        task->update_status(new_status);
    }
    else {
        std::cerr << "task not found, id: " << id << std::endl;
    }
    save_to_file(this->filename);
    return task;
}

Task* TaskManager::update_task_description(int id, std::string new_description) {
    auto task = get_task(id);
    if(task) {
        task->update_description(new_description);
    }
    else {
        std::cerr << "task not found, id: " << id << std::endl;
    }
    save_to_file(this->filename);
    return task;
}

std::vector<const Task*> TaskManager::list_task() {
    std::vector<const Task*> tasks_lst;
    tasks_lst.reserve(this->tasks.size());
    for (const auto& task : this->tasks) {
        tasks_lst.push_back(task.get());
    }
    return tasks_lst;
}

std::vector<const Task*> TaskManager::list_task(TaskStatus status) {
    std::vector<const Task*> tasks_lst;
    tasks_lst.reserve(this->tasks.size());
    for (const auto& task : this->tasks) {
        if (task->get_status() == status) {
            tasks_lst.push_back(task.get());
        }
    }
    return tasks_lst;
}