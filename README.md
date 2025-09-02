# GitDive - Commit-Genealogy: Semantic Change Similarity Explorer

A C program for analyzing Git repository history using semantic fingerprinting and similarity-based commit trees.

## Description

GitDive is an innovative command-line tool that goes beyond traditional chronological commit analysis. It analyzes local Git repositories and builds semantic similarity trees to reveal hidden conceptual relationships between commits. Instead of asking "What commit came before this one?", GitDive asks "What other commit in the project's entire history represents a similar type of change?"

## Core Concept: Semantic Diffs and Commit Similarity Trees

GitDive introduces the concept of "Semantic Fingerprinting" - a structured representation of the meaning of changes in a commit. Each commit is analyzed across three dimensions:

1. **Code Delta Analysis**: Semantic distance of code changes, lines added/removed, complexity delta
2. **File Path Analysis**: Module classification, file diversity, change distribution
3. **Commit Message NLP**: Intent extraction, sentiment analysis, ticket references

These components are combined into a high-dimensional vector that represents the unique "semantic fingerprint" of each commit.

## Features

### Traditional Analysis
- Chronological commit history with detailed information
- Binary search tree organization by timestamp
- Author information and commit messages
- File modification analysis

### Semantic Analysis (NEW)
- **Semantic Fingerprinting**: Multi-dimensional vector representation of commit characteristics
- **Similarity Tree Construction**: Non-chronological tree based on semantic similarity
- **Historical Precedent Search**: Find commits with similar characteristics from project history
- **Development Pattern Analysis**: Identify clusters of related commits
- **Commit Intent Classification**: Automatic categorization (fix, feat, refactor, docs, test, chore)
- **Sentiment Analysis**: Positive/negative sentiment scoring of commit messages
- **File Diversity Metrics**: Entropy-based analysis of change distribution

### Advanced Features
- **"Rhyming" Commits Discovery**: Find conceptually similar commits regardless of time
- **Semantic Distance Calculation**: Measure how much code meaning changed vs. volume of changes
- **Similarity Matrix Caching**: Optimized performance for large repositories
- **Tree Statistics**: Comprehensive analysis of similarity patterns

## Requirements

- C compiler (GCC recommended) with math library support (-lm)
- Git installed and available in PATH
- Must be run from within a Git repository

## Building

Compile the program using GCC with all modules:

```bash
gcc -Wall -Wextra -std=c11 -o gitdive.exe main.c commit.c list.c git_local.c binary_search_tree.c semantic_fingerprint.c commit_similarity_tree.c -lm
```

Or use the provided build task `build-gitdive-complete` if using VS Code.

## Usage

1. Navigate to a Git repository directory
2. Run the compiled program:

```bash
./gitdive.exe
```

The program will:
- Verify you're in a Git repository
- Display the repository root path
- Fetch and analyze commit history (last 15 commits)
- Show traditional chronological commit tree
- Generate semantic fingerprints for sample commits
- Build and display the semantic similarity tree
- Demonstrate historical precedent search
- Compare semantic similarity between commits

## Output Sections

1. **Commit History**: Traditional chronological listing
2. **Traditional BST**: Time-ordered binary search tree
3. **Semantic Fingerprint Analysis**: Detailed breakdown of commit characteristics
4. **Commit Similarity Tree**: Non-chronological tree organized by semantic similarity
5. **Tree Statistics**: Quantitative analysis of similarity patterns
6. **Historical Precedent Search**: Find similar commits from project history
7. **Similarity Comparison**: Direct comparison between specific commits

## Project Structure

### Core Modules
- `main.c` - Main program entry point and demonstration
- `commit.c/commit.h` - Commit data structures and accessor functions
- `list.c/list.h` - Enhanced linked list with node navigation
- `git_local.c/git_local.h` - Git repository interaction functions
- `binary_search_tree.c/binary_search_tree.h` - Traditional BST implementation

### Semantic Analysis Modules
- `semantic_fingerprint.c/semantic_fingerprint.h` - Multi-dimensional commit fingerprinting
- `commit_similarity_tree.c/commit_similarity_tree.h` - Similarity-based tree construction and analysis

## Key Algorithms

### Semantic Fingerprinting
- **Intent Recognition**: Pattern matching for conventional commit types
- **Sentiment Scoring**: Keyword-based positive/negative analysis  
- **File Classification**: Heuristic-based module identification (src, test, docs, config)
- **Complexity Analysis**: Line-based change impact assessment
- **Vector Normalization**: Unit vector representation for similarity calculation

### Similarity Tree Construction
- **Cosine Similarity**: High-dimensional vector comparison
- **Greedy Insertion**: Best-fit placement based on similarity scores
- **Binary Tree Structure**: Most similar (left) and second similar (right) children
- **Cached Matrix**: O(1) similarity lookups for performance

## Use Cases

### For Developers
- **Code Review Assistance**: "Show me similar changes made in the past"
- **Problem Solving**: "What other commits solved similar issues?"
- **Learning**: "How do we typically implement feature X?"

### For Teams
- **Onboarding**: Understand development patterns and practices
- **Quality Assessment**: Identify recurring problem patterns
- **Architecture Analysis**: Track conceptual evolution of codebase

### for Project Managers
- **Process Analysis**: Understand team development habits
- **Risk Assessment**: Identify fragile or problematic code areas
- **Resource Planning**: Predict effort based on historical similar changes
- `list.c/list.h` - Linked list implementation
- `git_local.c/git_local.h` - Git repository interaction functions

## Error Handling

The program handles common errors:
- Not being in a Git repository
- Git not being installed or accessible
- Repository having no commits
- Failed commit retrieval
- Memory allocation failures
- Invalid semantic fingerprint generation

## Performance Considerations

- **Similarity Matrix Caching**: Avoids redundant calculations for large commit sets
- **Memory Management**: Proper cleanup of all dynamically allocated structures
- **Scalable Architecture**: Modular design supports future optimizations
- **Configurable Limits**: Adjustable commit count and similarity thresholds

## Future Enhancements

The modular architecture supports extension with:
- Interactive similarity exploration
- Commit recommendation systems
- Team expertise mapping
- Architectural drift detection
- Integration with code review tools
- Web-based visualization interface

## Technical Innovation

GitDive represents a significant advancement in repository analysis by:
- Moving beyond chronological commit navigation
- Providing semantic understanding of code changes
- Enabling discovery of hidden development patterns
- Supporting data-driven development process improvements

## Author

Lucas Brandão
