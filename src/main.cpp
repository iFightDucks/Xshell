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
        output_file = full_command.substr(redirect_pos + 1);
        full_command = full_command.substr(0, redirect_pos);
        redirect_output = true;
    }

    // Execute command
    const char* command_cstr = full_command.c_str();
    if (redirect_output) {
        std::ofstream out(output_file);
        out << system(command_cstr);
        out.close();
    } else {
        system(command_cstr);
    }
}

void handle_echo(const std::vector<std::string>& tokens) {
    std::string output = "";
    bool redirect = false;
    std::string file_name;
    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == ">" || tokens[i] == "1>") {
            file_name = tokens[i + 1];
            redirect = true;
            break;
        }
        output += tokens[i] + " ";
    }
    if (redirect) {
        std::ofstream outfile(file_name);
        outfile << output;
        outfile.close();
    } else {
        std::cout << output << std::endl;
    }
}

void handle_cd(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cerr << "cd: missing argument" << std::endl;
        return;
    }
    std::string dir = tokens[1];
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        std::filesystem::current_path(dir);
    } else {
        std::cerr << "cd: " << dir << ": No such file or directory" << std::endl;
    }
}

void handle_pwd() {
    std::cout << std::filesystem::current_path() << std::endl;
}

void handle_type(const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cerr << "type: missing argument" << std::endl;
        return;
    }
    std::string command = tokens[1];
    std::string path = get_path(command);
    if (!path.empty()) {
        std::cout << command << " is " << path << std::endl;
    } else {
        std::cerr << command << ": command not found" << std::endl;
    }
}

int main() {
    string input;
    while (true) {
        std::cout << "$ ";
        std::getline(std::cin, input);

        vector<string> tokens = split(input, ' ');
        if (tokens.empty()) continue;

        validCommands cmd = isValid(tokens[0]);

        switch (cmd) {
            case echo:
                handle_echo(tokens);
                break;
            case cd:
                handle_cd(tokens);
                break;
            case exit0:
                return 0;
            case type:
                handle_type(tokens);
                break;
            case pwd:
                handle_pwd();
                break;
            case invalid:
            default:
                std::cerr << "Invalid command" << std::endl;
        }
    }
    return 0;
}
