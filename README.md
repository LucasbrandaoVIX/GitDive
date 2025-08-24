# GitDive

A C program for analyzing Git repository history and commit data.

## Description

GitDive is a command-line tool that analyzes local Git repositories and displays commit history with detailed information about modifications. It provides insights into repository structure and commit patterns.

## Features

- Displays real commit history from local Git repositories
- Shows commit details including author information and messages
- Analyzes file modifications and changes
- Validates Git repository structure
- Provides detailed diff information for commits

## Requirements

- C compiler (GCC recommended)
- Git installed and available in PATH
- Must be run from within a Git repository

## Building

Compile the program using GCC:

```bash
gcc -Wall -Wextra -std=c11 -o gitdive.exe main.c commit.c list.c git_local.c
```

Or use the provided build task if using VS Code.

## Usage

1. Navigate to a Git repository directory
2. Run the compiled program:

```bash
./gitdive.exe
```

The program will:
- Verify you're in a Git repository
- Display the repository root path
- Fetch and display the last 10 commits with details

## Project Structure

- `main.c` - Main program entry point
- `commit.c/commit.h` - Commit data structures and operations
- `list.c/list.h` - Linked list implementation
- `git_local.c/git_local.h` - Git repository interaction functions

## Error Handling

The program handles common errors:
- Not being in a Git repository
- Git not being installed or accessible
- Repository having no commits
- Failed commit retrieval

## Author

Lucas Brandão
