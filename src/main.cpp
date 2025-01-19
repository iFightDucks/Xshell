#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
using namespace std;

enum validCommands
{
    echo,
    cd,
    exit0,
    type,
    invalid,
};

validCommands isValid(std::string command){
    command = command.substr(0,command.find(" "));
    if(command == "echo") return validCommands::echo;
    if(command == "cd") return validCommands::cd;
    if(command == "exit") return validCommands::exit0;
    if(command == "type") return validCommands::type;
    return invalid;
}

std::string valid[4] = {"echo", "cd", "exit0"};

std::string get_path(std::string command){
    std::string path_env = std::getenv("PATH");
    std::stringstream ss(path_env);
    std::string path;
    while(!ss.eof()){
        getline(ss, path, ':');
        string abs_path = path + '/' + command;
        if(filesystem::exists(abs_path)){
            return abs_path;
        }
    }
    return "";  
}

int main() {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::string input;
    while(true){
        std::cout<<"$ ";
        std::getline(std::cin, input);
        if(input.empty()) continue;
        validCommands cmd = isValid(input);
        switch(cmd){
            case echo:
                std::cout<<input.substr(5)<<std::endl;
                break;
            case cd:
                std::cout<<"cd command is not implemented\n";
                break;
            case exit0:
                return 0;
            case type:
                input = input.substr(5);
                if(input == "echo" || input == "cd" || input == "exit"){
                    std::cout<<input<<" is a shell builtin\n";
                }
                else{
                    std::string path = get_path(input);
                    if(path.empty()){
                        std::cout<<input<<" not found\n";
                    }
                    else{
                        std::cout<<input<<" is "<<path<<std::endl;
                    }
                }
                break;
            default:
                std::cout<<input<<": command not found\n";
                break;
        }
    }
}
