#ifndef GIT_LOCAL_H
#define GIT_LOCAL_H

#include "commit.h"
#include "list.h"

// Function to get commits from local git repository
List* get_git_commits(int max_commits);

// Function to get commit details by hash
Commit* get_commit_by_hash(const char* commit_hash, int commit_id);

// Function to get modifications for a specific commit
List* get_commit_modifications(const char* commit_hash);

// Helper function to execute git commands and capture output
char* execute_git_command(const char* command);

// Function to parse git log output into commits
List* parse_git_log(const char* git_log_output);

// Function to parse git show output for modifications
List* parse_git_show(const char* git_show_output);

// Function to extract author information from git log line
Author* parse_author_from_line(const char* author_line, int author_id);

// Function to clean up temporary files and memory
void cleanup_git_data();

#endif
