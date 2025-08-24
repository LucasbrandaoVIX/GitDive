#include "git_local.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_COMMAND_LENGTH 512
#define MAX_OUTPUT_LENGTH 8192
#define MAX_LINE_LENGTH 1024

// Function to execute git commands and capture output
char* execute_git_command(const char* command) {
    HANDLE hReadPipe, hWritePipe;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    // Set up security attributes
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    
    // Create pipe for reading output
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        printf("Error creating pipe\n");
        return NULL;
    }
    
    // Set up startup info
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    
    // Prepare command
    char full_command[MAX_COMMAND_LENGTH];
    snprintf(full_command, sizeof(full_command), "cmd /c %s", command);
    
    // Create process
    if (!CreateProcess(NULL, full_command, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        printf("Error creating process for command: %s\n", command);
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return NULL;
    }
    
    // Close write handle (we only need to read)
    CloseHandle(hWritePipe);
    
    // Read output
    char* output = malloc(MAX_OUTPUT_LENGTH);
    if (!output) {
        CloseHandle(hReadPipe);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return NULL;
    }
    
    DWORD bytesRead;
    DWORD totalBytes = 0;
    char buffer[1024];
    
    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        if (totalBytes + bytesRead >= MAX_OUTPUT_LENGTH - 1) break;
        memcpy(output + totalBytes, buffer, bytesRead);
        totalBytes += bytesRead;
    }
    
    output[totalBytes] = '\0';
    
    // Clean up
    CloseHandle(hReadPipe);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return output;
}

// Function to parse author from git log line
Author* parse_author_from_line(const char* author_line, int author_id) {
    if (!author_line) return NULL;
    
    // Look for "Author: " prefix
    const char* author_start = strstr(author_line, "Author: ");
    if (!author_start) return NULL;
    
    author_start += 8; // Skip "Author: "
    
    // Find the end of the name (before email)
    const char* email_start = strchr(author_start, '<');
    if (!email_start) return NULL;
    
    // Extract name
    int name_length = email_start - author_start - 1; // -1 for space before <
    if (name_length <= 0 || name_length >= 32) return NULL;
    
    char name[32];
    strncpy(name, author_start, name_length);
    name[name_length] = '\0';
    
    return initialize_author(author_id, name);
}

// Function to parse git show output for modifications
List* parse_git_show(const char* git_show_output) {
    if (!git_show_output) return NULL;
    
    List* modifications = initialize_list();
    if (!modifications) return NULL;
    
    char* output_copy = malloc(strlen(git_show_output) + 1);
    strcpy(output_copy, git_show_output);
    
    char* line = strtok(output_copy, "\n");
    char current_file[256] = "";
    char old_code[1024] = "";
    char new_code[1024] = "";
    int start_line = 1;
    
    while (line != NULL) {
        // Check for file header
        if (strncmp(line, "diff --git", 10) == 0) {
            // Extract filename from diff header
            char* file_start = strstr(line, "b/");
            if (file_start) {
                strcpy(current_file, file_start + 2);
            }
        }
        // Check for hunk header to get line numbers
        else if (line[0] == '@' && line[1] == '@') {
            // Parse @@ -old_start,old_count +new_start,new_count @@
            char* plus_pos = strchr(line, '+');
            if (plus_pos) {
                start_line = atoi(plus_pos + 1);
            }
        }
        // Check for removed lines (old code)
        else if (line[0] == '-' && line[1] != '-') {
            if (strlen(old_code) + strlen(line) < 1023) {
                if (strlen(old_code) > 0) strcat(old_code, "\n");
                strcat(old_code, line + 1); // Skip the '-' prefix
            }
        }
        // Check for added lines (new code)
        else if (line[0] == '+' && line[1] != '+') {
            if (strlen(new_code) + strlen(line) < 1023) {
                if (strlen(new_code) > 0) strcat(new_code, "\n");
                strcat(new_code, line + 1); // Skip the '+' prefix
            }
        }
        // If we encounter another diff or end, create modification
        else if ((strncmp(line, "diff --git", 10) == 0 || line[0] == '\0') && 
                 strlen(current_file) > 0 && (strlen(old_code) > 0 || strlen(new_code) > 0)) {
            
            // If only new code, it's an addition (old_code should be empty)
            if (strlen(old_code) == 0 && strlen(new_code) > 0) {
                strcpy(old_code, "// [NEW FILE OR NEW LINES]");
            }
            // If only old code, it's a deletion (new_code should be empty)  
            if (strlen(new_code) == 0 && strlen(old_code) > 0) {
                strcpy(new_code, "// [DELETED LINES]");
            }
            
            Modification* mod = initialize_modification(current_file, old_code, new_code, start_line);
            if (mod) {
                insert_item(modifications, mod);
            }
            
            // Reset for next file
            strcpy(current_file, "");
            strcpy(old_code, "");
            strcpy(new_code, "");
            start_line = 1;
        }
        
        line = strtok(NULL, "\n");
    }
    
    // Handle last modification if exists
    if (strlen(current_file) > 0 && (strlen(old_code) > 0 || strlen(new_code) > 0)) {
        if (strlen(old_code) == 0 && strlen(new_code) > 0) {
            strcpy(old_code, "// [NEW FILE OR NEW LINES]");
        }
        if (strlen(new_code) == 0 && strlen(old_code) > 0) {
            strcpy(new_code, "// [DELETED LINES]");
        }
        
        Modification* mod = initialize_modification(current_file, old_code, new_code, start_line);
        if (mod) {
            insert_item(modifications, mod);
        }
    }
    
    free(output_copy);
    return modifications;
}

// Function to get modifications for a specific commit
List* get_commit_modifications(const char* commit_hash) {
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "git show %s", commit_hash);
    
    char* output = execute_git_command(command);
    if (!output) return NULL;
    
    List* modifications = parse_git_show(output);
    free(output);
    
    return modifications;
}

// Function to get commit details by hash
Commit* get_commit_by_hash(const char* commit_hash, int commit_id) {
    // Get commit info
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "git log --format=\"Author: %%an <%%ae>%%nDate: %%ad%%nMessage: %%s\" -1 %s", commit_hash);
    
    char* output = execute_git_command(command);
    if (!output) return NULL;
    
    // Parse author
    Author* author = parse_author_from_line(output, commit_id);
    if (!author) {
        free(output);
        return NULL;
    }
    
    // Parse message
    char message[256] = "";
    char* message_line = strstr(output, "Message: ");
    if (message_line) {
        message_line += 9; // Skip "Message: "
        char* newline = strchr(message_line, '\n');
        if (newline) *newline = '\0';
        strncpy(message, message_line, sizeof(message) - 1);
        message[sizeof(message) - 1] = '\0';
    }
    
    free(output);
    
    // Get modifications
    List* modifications = get_commit_modifications(commit_hash);
    
    return initialize_commit(commit_id, author, modifications, message);
}

// Function to parse git log output into commits
List* parse_git_log(const char* git_log_output) {
    if (!git_log_output) return NULL;
    
    List* commits = initialize_list();
    if (!commits) return NULL;
    
    char* output_copy = malloc(strlen(git_log_output) + 1);
    strcpy(output_copy, git_log_output);
    
    char* line = strtok(output_copy, "\n");
    int commit_id = 1;
    
    while (line != NULL) {
        // Look for commit hash lines
        if (strncmp(line, "commit ", 7) == 0) {
            char* commit_hash = line + 7; // Skip "commit "
            
            // Get first 7 characters of hash for short hash
            char short_hash[8];
            strncpy(short_hash, commit_hash, 7);
            short_hash[7] = '\0';
            
            Commit* commit = get_commit_by_hash(short_hash, commit_id);
            if (commit) {
                insert_item(commits, commit);
                commit_id++;
            }
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(output_copy);
    return commits;
}

// Function to get commits from local git repository
List* get_git_commits(int max_commits) {
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "git log --oneline -n %d", max_commits);
    
    char* output = execute_git_command(command);
    if (!output) {
        printf("Error: Could not execute git command. Make sure you're in a git repository.\n");
        return NULL;
    }
    
    if (strlen(output) == 0) {
        printf("No commits found in repository.\n");
        free(output);
        return NULL;
    }
    
    List* commits = initialize_list();
    if (!commits) {
        free(output);
        return NULL;
    }
    
    // Parse each line of git log --oneline output
    char* output_copy = malloc(strlen(output) + 1);
    strcpy(output_copy, output);
    
    char* line = strtok(output_copy, "\n");
    int commit_id = 1;
    
    while (line != NULL && commit_id <= max_commits) {
        // Each line format: "hash message"
        char* space_pos = strchr(line, ' ');
        if (space_pos) {
            *space_pos = '\0'; // Split hash from message
            char* commit_hash = line;
            
            Commit* commit = get_commit_by_hash(commit_hash, commit_id);
            if (commit) {
                insert_item(commits, commit);
                commit_id++;
            }
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(output);
    free(output_copy);
    return commits;
}

// Function to clean up temporary files and memory
void cleanup_git_data() {
    // Currently no cleanup needed, but can be extended
    // if temporary files or other resources are used
}
