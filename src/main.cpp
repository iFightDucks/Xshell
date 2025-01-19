#include <iostream>
#include <string>

int main() {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "$ ";
    std::string input;

    while (std::getline(std::cin, input) && input.find("exit") != 0) {
        if (input.substr(0, 5) == "type ") {
            std::string cmd = input.substr(5); // Extract the command after "type "
            if (cmd == "type" || cmd == "echo" || cmd == "exit") {
                std::cout << cmd << " is a shell builtin\n";
            } else {
                std::cout << cmd << ": not found\n";
            }
        } else if (input.substr(0, 5) == "echo ") {
            const int ECHO_LEN = 5;
            std::string text = input.substr(ECHO_LEN); // Extract text after "echo "
            std::cout << text << std::endl;
        } else {
            std::cout << input << ": command not found" << std::endl;
        }
        std::cout << "$ "; // Print the prompt again
    }

    return 0;
}
