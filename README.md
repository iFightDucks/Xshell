# XShell

## 🚀 Overview
XShell is a lightweight and efficient shell designed for simplicity and ease of use. It provides essential shell functionalities with a minimal footprint, making it ideal for embedded systems, lightweight Linux distributions, or custom shell scripting.

## 📂 Repository Setup
### Prerequisites
Ensure you have the following dependencies installed:
- GCC or Clang
- Make
- POSIX-compliant system (Linux, macOS, BSD)

### Build and Install
```sh
# Clone the repository
git clone https://github.com/gourabsen21s/Xshell.git
cd xshell

# Build the project
make

# Install (optional)
sudo make install
```

## 🖥️ REPL (Read-Eval-Print Loop)
Run XShell with:
```sh
./xshell
```
Once inside, you can execute commands as you would in a standard shell.

### 📝 Print a prompt
XShell displays a prompt that can be customized via the `~/.xshellrc` file.

### ❌ Handle invalid commands
When an invalid command is entered, XShell provides an error message and prompts again.

## 🔌 Built-in Commands
### ⏹️ The exit builtin
Exit the shell using the `exit` command.

### 🗣️ The echo builtin
Print messages to the terminal using `echo`.

### 🛠️ The type builtin: Builtins
Check if a command is a shell builtin using `type`.

### 📄 The type builtin: Executable files
Determine if a command is an external executable with `type`.

## ▶️ Run a program
Execute external programs as you would in a standard shell.

✅ **Base stages complete!**

## 📍 Navigation
### 📌 The pwd builtin
Display the current working directory with `pwd`.

### 📂 The cd builtin: Absolute paths
Change directory using absolute paths.

### 📂 The cd builtin: Relative paths
Change directory using relative paths.

### 🏠 The cd builtin: Home directory
Use `cd` without arguments to navigate to the home directory.

✅ **Extension complete!**

## 📝 Quoting
### 🖊️ Single quotes
Preserve literal characters using single quotes (`'`).

### 🖊️ Double quotes
Preserve spaces and special characters using double quotes (`"`).

### 🖊️ Backslash outside quotes
Escape special characters with `\` outside of quotes.

### 🖊️ Backslash within single quotes
Backslashes are treated literally inside single quotes.

### 🖊️ Backslash within double quotes
Escape special characters within double quotes using `\`.

### ▶️ Executing a quoted executable
Run commands that contain spaces by quoting the entire command.

✅ **Extension complete!**

## 🔄 Redirection
### 📤 Redirect stdout
Use `>` to redirect standard output to a file.

### 🚨 Redirect stderr
Use `2>` to redirect standard error to a file.

### 📌 Append stdout
Use `>>` to append standard output to a file.

### 🚨 Append stderr
Use `2>>` to append standard error to a file.

## ⌨️ Autocompletion
### ⚡ Builtin completion
Tab completion for built-in commands.

### 📝 Completion with arguments
Tab completion considers possible arguments.

### ❓ Missing completions
Handles cases where no completion is available.

### ▶️ Executable completion
Tab completion for executable files in the `$PATH`.

### 🔀 Multiple completions
Provides multiple options when possible.

### ✂️ Partial completions
Completes partially typed commands.

---

## 🎉 Contributing
Contributions are welcome! To contribute:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Open a pull request

## 📜 License
XShell is licensed under the MIT License. See `LICENSE` for more details.

## 📬 Contact
For questions or support, reach out via gourabsen.21.2001@gmail.com or open an issue on GitHub.

