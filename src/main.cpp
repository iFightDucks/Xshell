#include <iostream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  std::cout<<"$ ";
  std::string input;
  int checkValid = 0;
  while(std::getline(std::cin, input) && input.find("exit") != 0){
    if(input.substr(0,5) == "type "){
      checkValid = 1;
      std::string cmd = input.substr(5);
      if(cmd.substr(0,4) == "type" || cmd.substr(0,4) == "echo" || cmd.substr(0,4) == "exit"){
        std::cout<< cmd << " is a shell builtin\n";
      }else{
        std::cout<<cmd<<" not found\n";
      }
    }
    if(input.find("echo") == 0 && checkValid == 0){
      const int ECHO_LEN = 5;
      std::string text = input.substr(ECHO_LEN);
      std::cout<< text << std::endl;
    }
    else{
      std::cout<< input << ": command not found" << std::endl;
    }
    std::cout<< "$ ";
  }
  
}
