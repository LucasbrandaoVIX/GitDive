#ifndef GIT_LOCAL_H
#define GIT_LOCAL_H

#include "list.h"
#include "commit.h"
#include <stddef.h>

/**
 * Get commits from the local git repository
 * @param max_commits Maximum number of commits to retrieve (0 for all)
 * @return List of Commit* objects, or NULL on error
 */
List* get_git_commits(int max_commits);

/**
 * Get detailed diff information for a specific commit
 * @param commit_hash The commit hash to get diff for
 * @return List of Modification* objects, or NULL on error
 */
List* get_commit_diff(const char* commit_hash);

/**
 * Check if current directory is a git repository
 * @return 1 if git repo, 0 otherwise
 */
int is_git_repository();

/**
 * Get the current repository root path
 * @param buffer Buffer to store the path
 * @param buffer_size Size of the buffer
 * @return 1 on success, 0 on error
 */
int get_git_root(char* buffer, size_t buffer_size);

/**
 * Clean up any resources used by git operations
 */
void cleanup_git_data();

#endif
