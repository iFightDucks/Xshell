#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <array>

using namespace std;

enum validCommands {
    echo,
    cd,
    exit0,
    type,
    invalid,
    pwd
};

validCommands isValid(const std::string& command) {
    std::string cmd = command.substr(0, command.find(" "));
    if (cmd.empty()) return invalid;
    if (cmd == "echo") return validCommands::echo;
    if (cmd == "cd") return validCommands::cd;
    if (cmd == "exit") return validCommands::exit0;
    if (cmd == "type") return validCommands::type;
    if (cmd == "pwd") return validCommands::pwd;
    return invalid;
}

std::string get_path(const std::string& command) {
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

std::string processQuotes(const std::string& input) {
    std::string result;
    bool inSingleQuote = false, inDoubleQuote = false;
    bool escape = false;

    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];

        if (escape) {
            result += c;
            escape = false;
            continue;
        }

        if (c == '\\') {
            escape = true;
        } else if (c == '\'' && !inDoubleQuote) {
            inSingleQuote = !inSingleQuote;
        } else if (c == '\"' && !inSingleQuote) {
            inDoubleQuote = !inDoubleQuote;
        } else if (c == ' ' && !inSingleQuote && !inDoubleQuote) {
            if (!result.empty() && result.back() != ' ') {
                result += ' ';
            }
        } else {
            result += c;
        }
    }

    return result;
}

void execute_command(const std::string& command, const std::string& args) {
    std::string full_command = command + " " + args + " 2>/dev/null";

    FILE* fp = popen(full_command.c_str(), "r");
    if (fp == nullptr) {
        std::cerr << command << ": command not found\n";
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
        std::cerr << command << ": command not found\n";
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

        if (input.empty()) continue;

        size_t space_pos = input.find(' ');
        std::string command = input.substr(0, space_pos);
        std::string args = (space_pos == std::string::npos) ? "" : input.substr(space_pos + 1);

        switch (isValid(command)) {
            case cd: {
                try {
                    if (args.empty() || args == "~") {
                        const char* home = std::getenv("HOME");
                        if (home) {
                            std::filesystem::current_path(home);
                        } else {
                            std::cerr << "cd: HOME not set\n";
                        }
                    } else if (args[0] == '~') {
                        const char* home = std::getenv("HOME");
                        if (home) {
                            std::string newPath = std::string(home) + args.substr(1);
                            std::filesystem::current_path(newPath);
                        } else {
                            std::cerr << "cd: HOME not set\n";
                        }
                    } else {
                        std::filesystem::current_path(args);
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "cd: " << args << ": No such file or directory\n";
                }
                break;
            }

            case echo: {
                std::string result = processQuotes(input);
                std::cout << result << "\n";
                break;
            }

            case exit0:
                exit = true;
                break;

            case type: {
                if (isValid(args) != invalid) {
                    std::cout << args << " is a shell builtin\n";
                } else {
                    std::string path = get_path(args);
                    if (path.empty()) {
                        std::cout << args << ": not found\n";
                    } else {
                        std::cout << args << " is " << path << "\n";
                    }
                }
                break;
            }

            case pwd: {
                std::cout << std::filesystem::current_path().string() << "\n";
                break;
            }

            default:
                execute_command(command, args);
                break;
        }
    }

    return 0;
}
