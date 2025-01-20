#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <array>

using namespace std;

enum validCommands
{
    echo,
    cd,
    exit0,
    type,
    invalid,
};

validCommands isValid(std::string command) {
    command = command.substr(0, command.find(" "));
    if (command.empty()) return invalid; 
    if (command == "echo") return validCommands::echo;
    if (command == "cd") return validCommands::cd;
    if (command == "exit") return validCommands::exit0;
    if (command == "type") return validCommands::type;
    return invalid;
}

std::string get_path(std::string command) {
    if (command.empty()) return ""; 
    std::string path_env = std::getenv("PATH");
    std::stringstream ss(path_env);
    std::string path;

    while (!ss.eof()) {
        getline(ss, path, ':');
        std::string abs_path = path + '/' + command;
        if (filesystem::exists(abs_path)) {
            return abs_path;
        }
    }
    return "";
}

void execute_command(const std::string& command, const std::string& args) {
    std::string full_command = command + " " + args + " 2>/dev/null";

    FILE* fp = popen(full_command.c_str(), "r");
    if (fp == nullptr) {
        std::cout << command << ": command not found\n"; 
        return;
    }

    std::array<char, 128> buffer;
    std::string result;
    bool command_output = false;

    while (fgets(buffer.data(), buffer.size(), fp) != nullptr) {
        result += buffer.data();
        command_output = true;
    }

    int exit_status = pclose(fp);
    if (exit_status != 0 && !command_output) {
        std::cout << command << ": command not found\n"; 
    } else {
        std::cout << result;
    }
}

int main() {
    bool exit = false;

    while (!exit) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") {
            exit = true;
            continue;
        }

        size_t space_pos = input.find(' ');
        std::string command = input.substr(0, space_pos);
        std::string args = (space_pos == std::string::npos) ? "" : input.substr(space_pos + 1);

        switch (isValid(command)) {
            case cd:
                break;

            case echo:
                input.erase(0, input.find(" ") + 1);
                std::cout << input << "\n";
                break;

            case exit0:
                exit = true;
                break;

            case type: {
                input.erase(0, input.find(" ") + 1);
                if (isValid(input) != invalid) {
                    std::cout << input << " is a shell builtin\n";
                } else {
                    std::string path = get_path(input);
                    if (path.empty()) {
                        std::cout << input << ": not found\n";
                    } else {
                        std::cout << input << " is " << path << "\n";
                    }
                }
                break;
            }

            default:
                execute_command(command, args);
                break;
        }
    }

    return 0;
}
