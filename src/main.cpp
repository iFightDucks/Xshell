#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <array>
#include <vector>
#include <fstream>

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

vector<string> split(string &str, char delimiter) {
    vector<string> tokens;
    string token = "";
    bool singlequoteopen = false, doublequoteopen = false, escaped = false;
    for (int i = 0; i < str.size(); i++) {
        char ch = str[i];
        if (escaped) {
            if (doublequoteopen && ch != '"' && ch != '\\') {
                token += '\\';
            }
            token += ch;
            escaped = false;
        } else if (ch == '\\') {
            escaped = true;
            if(singlequoteopen) token+=ch;
        } else if (ch == '\'') {
            if (!doublequoteopen) singlequoteopen = !singlequoteopen;
            else token += ch;
        } else if (ch == '"') {
            doublequoteopen = !doublequoteopen;
        } else if (ch == delimiter && !singlequoteopen && !doublequoteopen) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += ch;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

void execute_command(const std::string& command, const std::string& args) {
    std::string full_command = command + " " + args;
    std::string output_file;
    bool redirect_output = false;
    bool append_output = false;

    size_t redirect_pos = full_command.find('>');
    if (redirect_pos != std::string::npos) {
        if (full_command[redirect_pos + 1] == '>') {
            append_output = true;
            redirect_pos++;
        }
        redirect_output = true;
        output_file = full_command.substr(redirect_pos + 1);
        full_command = full_command.substr(0, redirect_pos);
        output_file.erase(0, output_file.find_first_not_of(" \t"));
        output_file.erase(output_file.find_last_not_of(" \t") + 1);
    }

    FILE* fp = popen(full_command.c_str(), "r");
    if (fp == nullptr) {
        std::cerr << command << ": command not found\n";
        return;
    }

    std::array<char, 128> buffer;
    std::string result;

    while (fgets(buffer.data(), buffer.size(), fp) != nullptr) {
        result += buffer.data();
    }

    int exit_status = pclose(fp);
    if (exit_status != 0) {
        std::cerr << command << ": command not found or failed\n";
    }

    if (redirect_output) {
        std::ofstream ofs;
        ofs.open(output_file, append_output ? std::ios::app : std::ios::out);
        if (ofs.is_open()) {
            ofs << result;
            ofs.close();
        } else {
            std::cerr << "Error opening file for writing: " << output_file << "\n";
        }
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
                std::string result = input; 
                result.erase(0, result.find(" ") + 1); 
                vector<string> tokens = split(result, ' ');
                for (const auto& token : tokens) {
                    std::cout << token << " ";
                }
                std::cout << "\n";
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
