# Basic Linux Shell
This repository contains a custom Linux shell implementation written in C. The shell supports basic shell functionality, including command execution, alias management, and script sourcing.

---

## Features

1. **Command Execution**:
   - Executes standard Linux commands using `execvp`.
   - Supports process creation with `fork`.

2. **Alias Management**:
   - Create aliases for commands using the `alias` command.
   - Remove aliases using the `unalias` command.
   - List all current aliases.

3. **Script Execution**:
   - Supports sourcing `.sh` scripts with the `source` command.
   - Executes valid scripts line by line.

4. **Counters and Prompt**:
   - Tracks the number of commands, aliases, and script lines executed.
   - Displays these counters in the shell prompt.

5. **Memory Management**:
   - Dynamically allocates memory for counters and alias lists.
   - Ensures proper cleanup of memory on exit.

---

## Files

### 1. `shell.c`
The main program file containing the implementation of the custom shell. Key functionalities include:
- Command parsing and execution.
- Alias management (add, remove, list).
- Script handling via the `source` command.
- Memory management for dynamic allocations.

### 2. `run_me.sh`
A helper script to compile and run the shell program:
- Compiles `shell.c` with `gcc` using the `-Wall` flag to enable warnings.
- Runs the compiled program if compilation is successful.

---

## Setup and Usage

### Prerequisites
Ensure you have the following installed:
- GCC (GNU Compiler Collection)
- Bash (for running scripts)

### Steps to Run
1. Clone this repository to your local machine.
2. Make the `run_me.sh` script executable:
``` chmod +x run_me.sh ```
3. Run the helper script to compile and execute the shell:
``` ./run_me.sh ```

## Commands Supported

### Regular Commands
You can execute standard Linux commands like `ls`, `pwd`, etc., directly in the shell.

### Alias Management
- Add an alias:  
``` alias name='command' ```
- Remove an alias: 
``` unalias name ```
- List all aliases:
``` alias ```

### Script Sourcing
Run a `.sh` script using the `source` command:
``` source script.sh ```

Note: The script must start with a valid shebang (`#!/bin/bash`) and have a `.sh` extension.

### Exit Command
Exit the shell using:
``` exit_shell ```
This will free all allocated memory and terminate the program.

## Prompt Format
The shell prompt displays counters for tracking activity:
``` #cmd:<number_of_commands>|#alias:<number_of_aliases>|#script lines:<number_of_script_lines> ```
Example prompt after executing some commands:
``` #cmd:3|#alias:2|#script lines:5> ```

## Code Structure

### Key Functions in `shell.c`
1. **Alias Management**:
   - `createAliasNode()`: Creates a new alias node.
   - `insertAndCheckAlias()`: Adds or updates an alias.
   - `removeAlias()`: Removes an alias from the list.
   - `printAliasList()`: Prints all aliases.

2. **Command Handling**:
   - `handle_regular_command()`: Parses and executes regular commands.
   - `createProcess()`: Forks a process to execute commands.

3. **Script Handling**:
   - `handleSourceCommand()`: Handles sourcing of scripts.
   - `handleScript()`: Reads and executes commands from a script file.

4. **Memory Management**:
   - `checkMalloc()`: Ensures successful memory allocation.
   - `freeAliasList()`: Frees memory allocated for aliases.
   - `freeCounters()`: Frees memory allocated for counters.

5. **Exit Handling**:
   - `ifExit()`: Frees all resources and exits the program when `exit_shell` is entered.

## Error Handling
The shell includes error handling for:
- Invalid commands or syntax (e.g., malformed aliases).
- Memory allocation failures (`malloc`).
- File handling errors (e.g., missing or invalid scripts).

## License
This project is open-source and available under the MIT License. Feel free to use, modify, and distribute it as needed!
