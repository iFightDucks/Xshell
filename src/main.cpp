#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

enum class CommandType {
    EXIT,
    ECHO,
    TYPE,
    PWD,
    CD,
    EXECUTABLE,
    UNKNOWN
};

class ShellParser {
private:
    std::string currentDirectory;

    std::vector<std::string> split(std::string& input, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        while((pos = input.find(delimiter)) != std::string::npos) {
            token = input.substr(0, pos);
            tokens.push_back(token);
            input.erase(0, pos + delimiter.length());
        }
        tokens.push_back(input);
        return tokens;
    }

    CommandType identifyCommand(const std::string& command) {
        static const std::unordered_map<std::string, CommandType> commandMap = {
            {"exit", CommandType::EXIT},
            {"echo", CommandType::ECHO},
            {"type", CommandType::TYPE},
            {"pwd", CommandType::PWD},
            {"cd", CommandType::CD}
        };

        auto it = commandMap.find(command);
        return it != commandMap.end() ? it->second : CommandType::EXECUTABLE;
    }

    std::vector<std::string> getAllPathsFromEnvironment() {
        char const* path = std::getenv("PATH");
        if (path != NULL) {
            std::string pathAsString = path;
            return split(pathAsString, ":");
        }
        return {};
    }

    std::string extractExecutableName(const std::string& fullPath) {
        size_t lastSlash = fullPath.find_last_of('/');
        return (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
    }

    void handleEcho(const std::vector<std::string>& tokens) {
        std::string output;
        bool writeToFile = false;
        bool writeToStdOut = true;
        std::string fileName;

        for (size_t i = 1; i < tokens.size(); ++i) {
            if (tokens[i] == ">" || tokens[i] == "1>") {
                fileName = tokens[i + 1];
                writeToFile = true;
                writeToStdOut = false;
                break;
            }
            output += tokens[i] + " ";
        }

        if (writeToStdOut) {
            std::cout << output << std::endl;
        }

        if (writeToFile) {
            std::ofstream outputFile(fileName);
            if (outputFile.is_open()) {
                outputFile << output << std::endl;
            }
        }
    }

    void handleType(const std::vector<std::string>& tokens) {
        std::vector<std::string> builtinCommands = {"echo", "type", "pwd", "exit"};
        
        if (std::find(builtinCommands.begin(), builtinCommands.end(), tokens[1]) != builtinCommands.end()) {
            std::cout << tokens[1] << " is a shell builtin" << std::endl;
        } else {
            std::vector<std::string> paths = getAllPathsFromEnvironment();
            for (const auto& path : paths) {
                std::string fullPath = path + "/" + tokens[1];
                if (std::filesystem::exists(fullPath)) {
                    std::cout << tokens[1] << " is " << fullPath << std::endl;
                    return;
                }
            }
            std::cout << tokens[1] << ": not found" << std::endl;
        }
    }

    bool isExecutable(const std::filesystem::path& path) {
        if (std::filesystem::is_regular_file(path)) {
            auto permissions = std::filesystem::status(path).permissions();
            return (permissions & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ||
                   (permissions & std::filesystem::perms::group_exec) != std::filesystem::perms::none ||
                   (permissions & std::filesystem::perms::others_exec) != std::filesystem::perms::none;
        }
        return false;
    }

    bool handleExecutable(const std::vector<std::string>& tokens) {
        std::vector<std::string> paths = getAllPathsFromEnvironment();
        std::string firstToken = extractExecutableName(tokens[0]);

        for (const auto& path : paths) {
            std::string fullPath = path + "/" + firstToken;
            if (isExecutable(fullPath)) {
                std::string command;
                for (size_t i = 1; i < tokens.size(); ++i) {
                    command += '\'' + tokens[i] + "' ";
                }
                std::string fullCommandStr = fullPath + " " + command;
                system(fullCommandStr.c_str());
                return true;
            }
        }
        return false;
    }

    std::string goUpNDirectories(const std::string& currentDirectory, size_t count) {
        std::vector<std::string> pathSegments = split(const_cast<std::string&>(currentDirectory), "/");
        std::string newDirectory;
        for (size_t i = 1; i < pathSegments.size() - count; i++) {
            newDirectory += "/" + pathSegments[i];
        }
        return newDirectory;
    }

    void handleCd(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "cd: missing argument" << std::endl;
            return;
        }

        std::string newPath = tokens[1];
        std::string resolvedPath;

        if (newPath[0] == '/') {
            resolvedPath = newPath;
        } else if (newPath[0] == '~') {
            char const* home = std::getenv("HOME");
            resolvedPath = (home ? std::string(home) : "") + newPath.substr(1);
        } else if (newPath.substr(0, 2) == "./") {
            resolvedPath = currentDirectory + newPath.substr(1);
        } else if (newPath.substr(0, 3) == "../") {
            size_t count = std::count(newPath.begin(), newPath.end(), '.');
            resolvedPath = goUpNDirectories(currentDirectory, count / 2);
            if (newPath.length() > count) {
                resolvedPath += "/" + newPath.substr(count);
            }
        } else {
            resolvedPath = currentDirectory + "/" + newPath;
        }

        if (std::filesystem::is_directory(resolvedPath)) {
            currentDirectory = resolvedPath;
        } else {
            std::cout << "cd: " << newPath << ": No such file or directory" << std::endl;
        }
    }

public:
    ShellParser() : currentDirectory(std::filesystem::current_path()) {}

    void processCommand(const std::string& input) {
        std::vector<std::string> tokens = split(const_cast<std::string&>(input), " ");
        
        if (tokens.empty()) return;

        CommandType cmdType = identifyCommand(tokens[0]);

        switch (cmdType) {
            case CommandType::EXIT:
                exit(0);
                break;
            case CommandType::ECHO:
                handleEcho(tokens);
                break;
            case CommandType::TYPE:
                handleType(tokens);
                break;
            case CommandType::PWD:
                std::cout << currentDirectory << std::endl;
                break;
            case CommandType::CD:
                handleCd(tokens);
                break;
            case CommandType::EXECUTABLE:
                if (!handleExecutable(tokens)) {
                    std::cout << input << ": command not found" << std::endl;
                }
                break;
            default:
                std::cout << input << ": command not found" << std::endl;
                break;
        }
    }

    void run() {
        while (true) {
            std::cout << "$ ";
            std::string input;
            std::getline(std::cin, input);
            processCommand(input);
        }
    }
};

int main() {
    ShellParser shell;
    shell.run();
    return 0;
}