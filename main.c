#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "commit.h"
#include "git_local.h"

int main(){
    printf("GitDive - Real Git Repository Analysis\n");
    printf("======================================\n\n");
    
    // Check if we're in a git repository
    if (!is_git_repository()) {
        printf("Error: Not in a git repository.\n");
        printf("Please run this program from within a git repository.\n");
        return 1;
    }
    
    // Get repository root path
    char repo_path[512];
    if (get_git_root(repo_path, sizeof(repo_path))) {
        printf("Repository root: %s\n\n", repo_path);
    }
    
    // Get real commits from the local git repository
    printf("Fetching commits from local git repository...\n");
    List* commit_list = get_git_commits(10); // Get last 10 commits
    
    if (commit_list == NULL) {
        printf("Error: Could not retrieve commits from git repository.\n");
        printf("Please ensure:\n");
        printf("1. Git is installed and available in PATH\n");
        printf("2. The repository has commits\n");
        return 1;
    }
    
    // Print the entire commit list with real git data
    printf("\n========== REAL COMMIT HISTORY ==========\n");
    print_list(commit_list, (void (*)(void*))print_commit);
    printf("=========================================\n");
    
    // Clean up
    cleanup_git_data();
    
    return 0;
}