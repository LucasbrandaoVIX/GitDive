#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "commit.h"
#include "git_local.h"

int main(){
    printf("GitDive - Real Git Repository Analysis\n");
    printf("======================================\n\n");
    
    // Get real commits from the local git repository
    printf("Fetching commits from local git repository...\n");
    List* commit_list = get_git_commits(10); // Get last 10 commits
    
    if (commit_list == NULL) {
        printf("Error: Could not retrieve commits from git repository.\n");
        printf("Please ensure:\n");
        printf("1. You are in a git repository\n");
        printf("2. Git is installed and available in PATH\n");
        printf("3. The repository has commits\n");
        return 1;
    }
    
    // Print the entire commit list with real git data
    printf("========== REAL COMMIT HISTORY ==========\n");
    print_list(commit_list, (void (*)(void*))print_commit);
    printf("=========================================\n");
    
    // Clean up
    cleanup_git_data();
    
    return 0;
}