# GitDive

A C-based Git repository analysis tool that extracts and displays commit history with detailed code modifications from local Git repositories.

## Overview

GitDive is a command-line application that interfaces with local Git repositories to extract commit information, including author details, commit messages, and detailed code modifications (diff analysis). The tool provides a structured view of repository history with old and new code comparisons for each modification.

## Features

- **Real Git Integration**: Extracts actual commit data from local Git repositories
- **Detailed Diff Analysis**: Shows old vs new code for each file modification
- **Author Tracking**: Displays commit author information
- **File Modification History**: Tracks which files were changed and where
- **Line Number Precision**: Shows exact line numbers where changes occurred
- **Cross-Platform Git Support**: Uses standard Git commands for repository analysis

## Architecture

The project is structured with modular components:

- **Main Program** (`main.c`): Entry point and primary execution flow
- **Commit Management** (`commit.h`, `commit.c`): Data structures for commits, modifications, and authors
- **List Operations** (`list.h`, `list.c`): Generic linked list implementation for data storage
- **Git Integration** (`git_local.h`, `git_local.c`): Local Git repository interface and command execution

## Data Structures

### Commit Structure
- Commit ID and message
- Author information (name and ID)
- List of file modifications

### Modification Structure
- File path
- Old code content
- New code content
- Starting line number

### Author Structure
- Author ID
- Author name

## Prerequisites

- **Git**: Must be installed and available in system PATH
- **GCC Compiler**: For building the C source code
- **Git Repository**: Must be run within a Git repository directory

## Building

Use the provided build configuration or compile manually:

```bash
gcc -Wall -Wextra -std=c11 -o gitdive.exe main.c commit.c list.c git_local.c
```

## Usage

Navigate to a Git repository directory and run:

```bash
./gitdive.exe
```

The application will:
1. Detect the local Git repository
2. Extract the last 10 commits
3. Analyze code modifications for each commit
4. Display detailed commit history with diff information

## Output Format

The tool provides structured output including:

- **Commit Information**: ID, author, and commit message
- **Modification Details**: File path, line numbers, and code changes
- **Code Comparison**: Side-by-side old vs new code display

## Technical Implementation

### Git Command Integration
- Uses Windows API for process execution
- Captures Git command output through pipes
- Parses Git log and show commands for data extraction

### Memory Management
- Dynamic memory allocation for flexible data structures
- Proper cleanup and deallocation procedures
- Error handling for memory allocation failures

### Error Handling
- Git repository validation
- Git command execution error checking
- Graceful handling of missing or invalid data

## Limitations

- **Windows-Specific**: Current implementation uses Windows API for command execution
- **Git Dependency**: Requires Git installation and valid repository
- **Memory Constraints**: Limited by predefined buffer sizes for code content
- **Local Repository Only**: Does not support remote repository analysis

## Development

### Future Enhancements
- Cross-platform support (Linux/macOS)
- Remote repository analysis
- Enhanced diff parsing algorithms
- Configuration options for commit limits
- Output formatting options (JSON, XML)

## Contributing

When contributing to this project:

1. Maintain C11 standard compliance
2. Follow existing code structure and naming conventions
3. Ensure proper memory management
4. Add appropriate error handling
5. Test with various Git repository configurations
