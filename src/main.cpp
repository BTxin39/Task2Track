#include <task.h>
#include <task_manager.h>
#include <cxxopts.hpp>

#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
    TaskManager task_manager("task.json");
    cxxopts::Options options("task_cli", "Task2Track manager");
    options.add_options()
        ("l,list", "list all tasks")
        ("g,get", "get single task by [id]", cxxopts::value<int>(), "<task[id]>")
        ("a,add", "add a new task", cxxopts::value<std::string>(), "<task[description]>")
        ("r,remove", "delete a task by [id]", cxxopts::value<int>(), "<task[id]>")
        ("u,update", "update a task by [id] (--desc, --status)", cxxopts::value<int>(), "<task[id]>")
        ("r-last", "delete the last task")
        ("c,clear", "clear all tasks")
        ("d,desc", "set a new task description", cxxopts::value<std::string>())
        ("s,status", "set a new task status (TO_DO, IN_PROGRESS, DONE)", cxxopts::value<std::string>())
        ("h,help", "show help")
        ("exit", "exit the program")
        ("quit", "exit the program");

    using ParseResult = cxxopts::ParseResult;
    using CommandHandler = std::function<void(const ParseResult&)>;

    bool running = true;
    const auto print_task = [](const Task* task) {
        auto* readable_task = const_cast<Task*>(task);
        std::cout << readable_task->get_id() << " ["
                  << status_2_string(readable_task->get_status()) << "] "
                  << readable_task->get_description() << '\n';
    };

    const std::unordered_map<std::string, std::string> command_names{
        {"l", "list"}, {"list", "list"}, {"ls", "list"},
        {"g", "get"}, {"get", "get"},
        {"a", "add"}, {"add", "add"},
        {"r", "remove"}, {"remove", "remove"}, {"rm", "remove"},
        {"u", "update"}, {"update", "update"}, {"ud", "update"},
        {"r-last", "r-last"}, 
        {"c", "clear"}, {"clear", "clear"},
        {"h", "help"}, {"help", "help"},
        {"exit", "exit"}, {"quit", "quit"}
    };

    const std::unordered_map<std::string, CommandHandler> handlers{
        {"list", [&task_manager, &print_task](const ParseResult&) {
            for (const Task* task : task_manager.list_task()) {
                print_task(task);
            }
        }},
        {"get", [&task_manager, &print_task](const ParseResult& result) {
            const Task* task = task_manager.get_task(result["get"].as<int>());
            if (task != nullptr) {
                print_task(task);
            } else {
                std::cerr << "task not found\n";
            }
        }},
        {"add", [&task_manager](const ParseResult& result) {
            Task* task = task_manager.add_task(result["add"].as<std::string>());
            std::cout << "added task " << task->get_id() << '\n';
        }},
        {"remove", [&task_manager](const ParseResult& result) {
            const bool removed = task_manager.remove_task(result["remove"].as<int>());
            std::cout << (removed ? "task removed\n" : "task not found\n");
        }},
        {"update", [&task_manager](const ParseResult& result) {
            const int id = result["update"].as<int>();
            if (result.count("desc") != 0) {
                task_manager.update_task_description(id, result["desc"].as<std::string>());
            }
            if (result.count("status") != 0) {
                task_manager.update_task_status(id, result["status"].as<std::string>());
            }
            if (result.count("desc") == 0 && result.count("status") == 0) {
                std::cerr << "provide --desc and/or --status\n";
            }
        }},
        {"r-last", [&task_manager](const ParseResult&) {
            const bool removed = task_manager.remove_last_task();
            std::cout << (removed ? "last task removed\n" : "no tasks to remove\n");
        }},
        {"clear", [&task_manager](const ParseResult&) {
            const bool cleared = task_manager.remove_all_tasks();
            std::cout << (cleared ? "all tasks cleared\n" : "no tasks to clear\n");
        }},
        {"help", [&options](const ParseResult&) {
            std::cout << options.help() << '\n';
        }},
        {"exit", [&running](const ParseResult&) {
            running = false;
        }},
        {"quit", [&running](const ParseResult&) {
            running = false;
        }}
    };

    std::cout << "here is Task2Track manager\n"
              << "input 'help' to get help, 'exit' or 'quit' to quit\n\n";

    std::string line;
    while (running) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        std::istringstream input(line);
        std::vector<std::string> args;
        std::string arg;
        while (input >> std::quoted(arg)) {
            args.push_back(arg);
        }
        if (args.empty()) {
            continue;
        }

        std::string command_name = args.front();
        if (command_name.starts_with("--")) {
            command_name.erase(0, 2);
        } else if (command_name.starts_with('-')) {
            command_name.erase(0, 1);
        }

        const auto command = command_names.find(command_name);
        if (command == command_names.end()) {
            std::cerr << "unknown command: " << args.front() << '\n';
            continue;
        }

        args.front() = args.front().starts_with('-')
            ? args.front()
            : "--" + args.front();

        try {
            std::vector<const char*> argv;
            argv.reserve(args.size() + 1);
            argv.push_back("task_cli");
            for (const auto& value : args) {
                argv.push_back(value.c_str());
            }
            const ParseResult result = options.parse(
                static_cast<int>(argv.size()), argv.data());
            handlers.at(command->second)(result);
        } catch (const cxxopts::exceptions::exception& error) {
            std::cerr << "invalid command: " << error.what() << '\n';
        } catch (const std::exception& error) {
            std::cerr << "command failed: " << error.what() << '\n';
        }
    }

    return 0;
}
