#include "git_local.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <io.h>
#include <process.h>
#define popen _popen
#define pclose _pclose
#endif

#define MAX_COMMAND_LENGTH 512
#define MAX_LINE_LENGTH 2048
#define MAX_COMMIT_HASH_LENGTH 41

// Helper function to execute git commands and capture output
static FILE* execute_git_command(const char* command) {
    return popen(command, "r");
}

// Helper function to close command execution
static int close_git_command(FILE* fp) {
    return pclose(fp);
}

// Helper function to trim whitespace from strings
static char* trim_whitespace(char* str) {
    char* end;
    
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

// Parse a single line of git log output
static void parse_commit_line(char* line, char* hash, char* author_name, char* message, char* date) {
    // Format: hash|author|timestamp|message
    char* token = strtok(line, "|");
    if (token) strcpy(hash, trim_whitespace(token));
    
    token = strtok(NULL, "|");
    if (token) strcpy(author_name, trim_whitespace(token));
    
    token = strtok(NULL, "|");
    if (token) strcpy(date, trim_whitespace(token));
    
    token = strtok(NULL, "|");
    if (token) strcpy(message, trim_whitespace(token));
}

// Parse git diff output to extract modifications
static List* parse_diff_output(FILE* fp) {
    List* modifications = initialize_list();
    char line[MAX_LINE_LENGTH];
    char* current_file = NULL;
    char* old_code = NULL;
    char* new_code = NULL;
    size_t old_code_len = 0;
    size_t new_code_len = 0;
    size_t old_code_capacity = 1024;
    size_t new_code_capacity = 1024;
    int start_line = 0;
    int in_hunk = 0;
    
    // Initialize dynamic strings
    old_code = malloc(old_code_capacity);
    new_code = malloc(new_code_capacity);
    if (!old_code || !new_code) {
        free(old_code);
        free(new_code);
        return modifications;
    }
    old_code[0] = '\0';
    new_code[0] = '\0';
    
    while (fgets(line, sizeof(line), fp)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        if (strncmp(line, "diff --git", 10) == 0) {
            // Save previous modification if exists
            if (current_file && (old_code_len > 0 || new_code_len > 0)) {
                Modification* mod = initialize_modification(current_file, old_code, new_code, start_line);
                if (mod) insert_item(modifications, mod);
            }
            
            // Reset for new file
            old_code[0] = '\0';
            new_code[0] = '\0';
            old_code_len = 0;
            new_code_len = 0;
            start_line = 0;
            in_hunk = 0;
            
            // Extract filename from "diff --git a/file b/file"
            char* space = strchr(line + 11, ' ');
            if (space) {
                char* filename = space + 3; // Skip " b/"
                free(current_file); // Free previous filename
                current_file = malloc(strlen(filename) + 1);
                if (current_file) {
                    strcpy(current_file, filename);
                }
            }
        }
        else if (strncmp(line, "@@", 2) == 0) {
            // Parse hunk header to get line number
            char* plus_pos = strchr(line, '+');
            if (plus_pos) {
                start_line = atoi(plus_pos + 1);
            }
            in_hunk = 1;
        }
        else if (in_hunk) {
            if (line[0] == '-' && line[1] != '-') {
                // Removed line - add to old_code
                size_t line_len = strlen(line + 1) + 1; // +1 for newline
                if (old_code_len + line_len + 1 >= old_code_capacity) {
                    old_code_capacity *= 2;
                    char* temp = realloc(old_code, old_code_capacity);
                    if (temp) {
                        old_code = temp;
                    } else {
                        continue; // Skip if realloc fails
                    }
                }
                strcat(old_code, line + 1);
                strcat(old_code, "\n");
                old_code_len += line_len;
            }
            else if (line[0] == '+' && line[1] != '+') {
                // Added line - add to new_code
                size_t line_len = strlen(line + 1) + 1; // +1 for newline
                if (new_code_len + line_len + 1 >= new_code_capacity) {
                    new_code_capacity *= 2;
                    char* temp = realloc(new_code, new_code_capacity);
                    if (temp) {
                        new_code = temp;
                    } else {
                        continue; // Skip if realloc fails
                    }
                }
                strcat(new_code, line + 1);
                strcat(new_code, "\n");
                new_code_len += line_len;
            }
        }
    }
    
    // Save last modification
    if (current_file && (old_code_len > 0 || new_code_len > 0)) {
        Modification* mod = initialize_modification(current_file, old_code, new_code, start_line);
        if (mod) insert_item(modifications, mod);
    }
    
    // Cleanup
    free(current_file);
    free(old_code);
    free(new_code);
    
    return modifications;
}

int is_git_repository() {
    FILE* fp = execute_git_command("git rev-parse --git-dir 2>nul");
    if (!fp) return 0;
    
    char line[MAX_LINE_LENGTH];
    int is_repo = (fgets(line, sizeof(line), fp) != NULL);
    close_git_command(fp);
    
    return is_repo;
}

int get_git_root(char* buffer, size_t buffer_size) {
    FILE* fp = execute_git_command("git rev-parse --show-toplevel 2>nul");
    if (!fp) return 0;
    
    if (fgets(buffer, buffer_size, fp)) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;
        close_git_command(fp);
        return 1;
    }
    
    close_git_command(fp);
    return 0;
}

List* get_commit_diff(const char* commit_hash) {
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "git show --pretty=format: %s", commit_hash);
    
    FILE* fp = execute_git_command(command);
    if (!fp) return NULL;
    
    List* modifications = parse_diff_output(fp);
    close_git_command(fp);
    
    return modifications;
}

List* get_git_commits(int max_commits) {
    if (!is_git_repository()) {
        printf("Error: Not in a git repository\n");
        return NULL;
    }
    
    char command[MAX_COMMAND_LENGTH];
    if (max_commits > 0) {
        snprintf(command, sizeof(command), 
                "git log --pretty=format:\"%%H|%%an|%%at|%%s\" -n %d", max_commits);
    } else {
        strcpy(command, "git log --pretty=format:\"%H|%an|%at|%s\"");
    }
    
    FILE* fp = execute_git_command(command);
    if (!fp) {
        printf("Error: Failed to execute git command\n");
        return NULL;
    }
    
    List* commit_list = initialize_list();
    char line[MAX_LINE_LENGTH];
    int commit_id = 1;
    
    while (fgets(line, sizeof(line), fp)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) == 0) continue;
        
        // Use dynamic allocation for parsing variables
        char hash[MAX_COMMIT_HASH_LENGTH];
        char* author_name = malloc(256);
        char* message = malloc(512);
        char date[64];
        
        if (!author_name || !message) {
            free(author_name);
            free(message);
            continue;
        }
        
        // Parse the commit line
        parse_commit_line(line, hash, author_name, message, date);
        
        // Create author
        Author* author = initialize_author(commit_id, author_name);
        if (!author) {
            free(author_name);
            free(message);
            continue;
        }
        
        // Get modifications for this commit
        List* modifications = get_commit_diff(hash);
        
        // Parse UNIX timestamp from date field (git log %at gives epoch seconds)
        long long timestamp = 0;
        if (strlen(date) > 0) {
            timestamp = atoll(date);
        }

        // Create commit including timestamp
        Commit* commit = initialize_commit(commit_id, author, modifications, message, timestamp);
        if (commit) {
            insert_item(commit_list, commit);
            commit_id++;
        }
        
        // Free temporary strings (the data is now copied into the structures)
        free(author_name);
        free(message);
    }
    
    close_git_command(fp);
    
    if (commit_id == 1) {
        printf("Warning: No commits found in repository\n");
        return NULL;
    }
    
    printf("Successfully loaded %d commits from git repository\n", commit_id - 1);
    return commit_list;
}

void cleanup_git_data() {
    // Currently no persistent resources to clean up
    // This function is here for future use if needed
    printf("Git data cleanup completed\n");
}
