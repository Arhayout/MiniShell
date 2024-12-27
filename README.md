# Mini-Shell Project

This project implements a simplified command-line shell interpreter. The Mini-Shell provides basic shell functionalities like command execution, redirections, pipelines, and job control (background processes), offering features similar to popular shells like Bash or PowerShell.

## Features
- Execution of basic shell commands.
- Redirection of input/output using `<` and `>`.
- Support for pipelines (`|`) for chaining commands.
- Job control (e.g., running processes in the background).
- Basic handling of signals (`Ctrl+C`, `Ctrl+Z`).

## Files
- `minishell.c`: Provides additional functions for process and signal handling.
- `job.c`: Manages jobs and their states.
- `readcmd.c`: file for reading and parsing commands.

## Requirements
- GCC compiler or any C compiler supporting POSIX standards.
- Unix-like operating system (Linux or macOS recommended).

## Installation
1. Clone this repository to your local machine:
   ```bash
   git clone <repository_url>
   cd <repository_folder>
   ```

2. Compile the program using `gcc`:
   ```bash
   gcc -Wall -o minishell readcmd.c minishell.c job.c -lpthread
   ```

## How to Execute
1. Run the compiled Mini-Shell:
   ```bash
   ./minishell
   ```

2. You will see a prompt similar to:
   ```
   arhayout@Minishell:~$ 
   ```
   Start typing commands to execute them.

## Usage Examples
- **Run a basic command**:
  ```bash
  arhayout@Minishell:~$ ls -l
  ```
- **Run a command in the background**:
  ```bash
  arhayout@Minishell:~$ sleep 10 &
  ```
- **Redirect output to a file**:
  ```bash
  arhayout@Minishell:~$ ls > output.txt
  ```
- **Use a pipeline**:
  ```bash
  arhayout@Minishell:~$ cat file.txt | grep keyword
  ```

## Known Limitations
- Some signal handling (e.g., `Ctrl+Z`, `Ctrl+C`) is not fully functional.
- Complex pipelines or advanced shell functionalities (like command history) are not supported.

## Future Improvements
- Add a command history feature.
- Improve signal handling and error recovery.
- Implement additional built-in commands for better usability like tabulations



## Author
Abdelmalek Rhayoute

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.
