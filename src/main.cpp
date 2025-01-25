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

// Function to validate commands
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

// Function to split strings by delimiter
vector<string> split(string &str, char delimiter) {
    vector<string> tokens;
    string token = "";
    bool singlequoteopen = false, doublequoteopen = false, escaped = false;
    for (int i = 0; i < str.size(); i++) {
        char ch = str[i];
        if (escaped) {
            token += ch;
            escaped = false;
        } else if (ch == '\\') {
            escaped = true;
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

// Function to execute commands
void execute_command(const std::string& command, const std::string& args) {
    std::string full_command = command + " " + args;
    std::string output_file;
    bool redirect_output = false;
    bool append_output = false;

    size_t redirect_pos = full_command.find('>');
    if (redirect_pos != std::string::npos) {
        size_t append_pos = full_command.find(">>", redirect_pos);
        if (append_pos != std::string::npos) {
            append_output = true;
            full_command = full_command.substr(0, append_pos) + full_command.substr(append_pos + 2);
        }
        size_t file_pos = full_command.find_first_not_of(' ', redirect_pos + 1);
        output_file = full_command.substr(file_pos);
        full_command = full_command.substr(0, redirect_pos);
        redirect_output = true;
    }

    if (redirect_output) {
        std::ofstream out(output_file, append_output ? std::ios_base::app : std::ios_base::trunc);
        std::streambuf* orig = std::cout.rdbuf();
        std::cout.rdbuf(out.rdbuf());
        system(full_command.c_str());
        std::cout.rdbuf(orig);
    } else {
        system(full_command.c_str());
    }
}

// Function to check if the command exists in PATH
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

// Function to handle echo command
void handle_echo(const vector<string>& tokens) {
    std::string output = "";
    bool redirect_output = false;
    std::string file_name;

    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == ">" || tokens[i] == "1>") {
            file_name = tokens[i + 1];
            redirect_output = true;
            break;
        }
        output += tokens[i] + " ";
    }

    if (redirect_output) {
        std::ofstream outfile(file_name);
        if (outfile.is_open()) {
            outfile << output;
            outfile.close();
        }
    } else {
        std::cout << output << std::endl;
    }
}

// Function to handle cd command
void handle_cd(const vector<string>& tokens, std::string& current_directory) {
    if (tokens.size() < 2) {
        std::cout << "cd: missing operand" << std::endl;
        return;
    }

    std::string new_directory = tokens[1];

    if (new_directory == "~") {
        const char* home_dir = std::getenv("HOME");
        new_directory = home_dir ? home_dir : "/";
    } else if (new_directory == "..") {
        current_directory = current_directory.substr(0, current_directory.find_last_of('/'));
    } else {
        current_directory = new_directory;
    }

    if (!std::filesystem::exists(current_directory)) {
        std::cout << "cd: no such file or directory: " << new_directory << std::endl;
    }
}

// Function to handle type command
void handle_type(const vector<string>& tokens) {
    if (tokens.size() < 2) {
        std::cout << "type: missing operand" << std::endl;
        return;
    }

    std::string command = tokens[1];
    std::string path = get_path(command);
    if (path.empty()) {
        std::cout << command << ": not found" << std::endl;
    } else {
        std::cout << command << " is " << path << std::endl;
    }
}

int main() {
    std::string current_directory = std::filesystem::current_path();

    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);

        vector<string> tokens = split(input, ' ');

        if (tokens.empty()) continue;

        validCommands cmd = isValid(tokens[0]);

        switch (cmd) {
            case echo:
                handle_echo(tokens);
                break;
            case cd:
                handle_cd(tokens, current_directory);
                break;
            case exit0:
                return 0;
            case type:
                handle_type(tokens);
                break;
            case pwd:
                std::cout << current_directory << std::endl;
                break;
            default:
                std::cout << input << ": command not found" << std::endl;
        }
    }

    return 0;
}
