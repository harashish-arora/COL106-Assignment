# Time-Travelling File System

## 1. Project Overview

This project implements a simplified version control system for files using custom data structures (trees, hashmaps, heaps). It supports commands like `CREATE`, `INSERT`, `UPDATE`, `SNAPSHOT`, `ROLLBACK`, `HISTORY`, `RECENT_FILES`, and `BIGGEST_TREES`.

## 2. File Structure and Explanations

- **main.cpp**  
  The main entry point. Handles command parsing, dispatch, and user interaction. Manages the global file table and heaps.

- **file.hpp**  
  Defines the `File` class, which manages a versioned file using a tree of versions. Handles operations like insert, update, snapshot, rollback, and history.

- **tree.hpp**  
  Defines the `TreeNode` class, representing a single version node in a file's version tree. Stores content, snapshot info, parent/children, and timestamps.

- **file_hash.hpp**  
  Implements a custom hash table mapping filenames to `File*` pointers for fast lookup and existence checks.

- **hashmap.hpp**  
  Implements a simple map from integer version IDs to `TreeNode*` pointers for efficient version lookup within a file.

- **heap.hpp**  
  Implements a max-heap for `File*` objects, used for queries like most recently modified files and files with the most versions. Supports custom comparators.

- **heap_pos_map.hpp**  
  Implements a hash table mapping filenames to their positions in the heap, enabling efficient heap updates and not using lazy heaps.

- **build.sh**  
  Shell script to compile the project using g++/clang++.

## 3. Compilation Instructions

To compile, use the provided shell script:

```sh
./build.sh
```

**Requirements:**
- g++ (or clang++) with at least C++11 support.
- Tested on macOS with clang++ version Apple clang++ 15.0.0.

The script compiles `main.cpp` and produces an executable called `main`.

## 4. Running the Program

Run the executable:

```sh
./main
```

**Input:**  
Commands are read from standard input.

**Interactive mode:**
```
./main
CREATE file.txt
INSERT file.txt hello
READ file.txt
```

**Using an input file:**
```
./main < test.in
```

## 5. Supported Commands and Syntax

- `CREATE <filename>`  
  Creates a new file with the given filename. Initializes version 0 (root snapshot).

- `READ <filename>`  
  Prints the content of the file’s active version.

- `INSERT <filename> <content>`  
  Appends new content. Creates a new version if the active node is a snapshot.

- `UPDATE <filename> <content>`  
  Replaces content. Creates a new version if the active node is a snapshot.

- `SNAPSHOT <filename> <message>`  
  Marks the active version as snapshotted with the given message.

- `ROLLBACK <filename> [versionID]`  
  With ID: sets active version pointer to that version.  
  Without ID: sets active version pointer to the parent.

- `HISTORY <filename>`  
  Lists all snapshotted versions on the path from root → active, showing ID, timestamp, and message. Timestamps are displayed as Unix epoch time (seconds since 1 Jan 1970).

- `RECENT_FILES <k>`  
  Lists the k most recently modified files (by last modification time).

- `BIGGEST_TREES <k>`  
  Lists the k files with the largest number of versions.

## 6. Error and Edge Case Handling

The system provides clear error messages for all invalid inputs and operations:

- **File-related errors**
  - Creating a file that already exists → `Error: File '<filename>' already exists.`
  - Referring to a non-existent file → `Error: File '<filename>' not found.`

- **Command usage errors**
  - Missing arguments for any command → usage error message showing correct syntax.
  - Empty content for `INSERT`/`UPDATE` → usage error message.
  - Empty message for `SNAPSHOT` → usage error message.

- **Rollback errors**
  - Negative version ID → `Error: VersionID must be non-negative.`
  - Nonexistent version ID → `Error: Version <id> not found for file '<filename>'.`
  - Rollback at root → `Error: Cannot rollback from root version.`

- **Heap query errors**
  - Missing `<k>` → usage error message.
  - Non-positive `k` → `Error: Invalid command. k must be positive.`
  - `k` larger than number of files →  
    `Error: k cannot exceed number of files. Currently only <n> file(s) exist.`

- **Unknown command**
  - Any unsupported command → `Error: Unknown command '<cmd>'.`


## 7. Design Choices / Assumptions

- **Heap queries:** We explicitly disallow k > number of files, instead of returning fewer results. This ensures consistent, predictable error handling.
- **Snapshot policy:** Snapshots do not create new nodes; they mark the current version.
- **last_modified:** Updated only on CREATE, INSERT, UPDATE (not SNAPSHOT/ROLLBACK).
- **Tie-breaking in heaps:** Arbitrary if two files have same timestamp/versions.
- **Hashmaps:** Fixed bucket size with chaining (1009).
- **Heaps:** Update-in-place with position map.

## 8. Complexity Analysis

- CREATE, READ, INSERT, UPDATE, SNAPSHOT, ROLLBACK: O(1) average (excluding string concatenation).
- HISTORY: O(h), where h is tree height.
- RECENT_FILES / BIGGEST_TREES: O(k log n).

## 9. Example Run

**Input:**
```
CREATE a.txt
INSERT a.txt Hello
SNAPSHOT a.txt first
INSERT a.txt World
SNAPSHOT a.txt second
HISTORY a.txt
```

**Output:**
```
File 'a.txt' created successfully.
New version 1 created for 'a.txt'. Parent is version 0.
Snapshot created for 'a.txt' with message: first
New version 2 created for 'a.txt'. Parent is version 1.
Snapshot created for 'a.txt' with message: second
0 <timestamp> This is the root
1 <timestamp> first
2 <timestamp> second
```

## 10. Comprehensive Test Suite

To run these tests, copy the commands into a file (e.g., `test.in`) and run:

```
./main < test.in
```

Timestamps in output are placeholders (`<timestamp>`) and will vary.

---

### Phase 1: Complex Branching

```
CREATE research_paper.tex
INSERT research_paper.tex Abstract
SNAPSHOT research_paper.tex v1.0-Abstract
INSERT research_paper.tex _Introduction
SNAPSHOT research_paper.tex v1.1-Intro
INSERT research_paper.tex _Methodology
SNAPSHOT research_paper.tex v1.2-Methods
ROLLBACK research_paper.tex 1
INSERT research_paper.tex _Revised_Abstract
SNAPSHOT research_paper.tex v1.0.1-revised-abstract
INSERT research_paper.tex _New_Section
SNAPSHOT research_paper.tex v1.0.2-new-section
HISTORY research_paper.tex
ROLLBACK research_paper.tex 2
INSERT research_paper.tex _Alternative_Introduction
SNAPSHOT research_paper.tex v1.1.1-alt-intro
HISTORY research_paper.tex
```

---

### Phase 2: High-Volume Versioning

```
CREATE dataset.csv
INSERT dataset.csv row1
SNAPSHOT dataset.csv data-v1
INSERT dataset.csv ,row2
SNAPSHOT dataset.csv data-v2
INSERT dataset.csv ,row3
SNAPSHOT dataset.csv data-v3
INSERT dataset.csv ,row4
SNAPSHOT dataset.csv data-v4
INSERT dataset.csv ,row5
SNAPSHOT dataset.csv data-v5
```

---

### Phase 3: Analytics and Recency Testing

```
CREATE main.py
INSERT main.py import_os
SNAPSHOT main.py init
BIGGEST_TREES 3
BIGGEST_TREES 1
RECENT_FILES 3
RECENT_FILES 0
UPDATE dataset.csv new_header
RECENT_FILES 1
```

---

### Phase 4: Intensive Error and Edge Case Handling

```
CREATE research_paper.tex
READ non_existent.file
INSERT non_existent.file data
SNAPSHOT non_existent.file msg
HISTORY non_existent.file
ROLLBACK research_paper.tex 100
ROLLBACK research_paper.tex
ROLLBACK research_paper.tex 0
ROLLBACK
INSERT
UPDATE research_paper.tex
SNAPSHOT research_paper.tex
UPDATE research_paper.tex 0 Cannot_update_snapshot_directly
READ research_paper.tex
```

---

### Additional Edge Cases

```
CREATE notes.md
SNAPSHOT notes.md first
ROLLBACK notes.md
ROLLBACK notes.md
INSERT notes.md More notes
SNAPSHOT notes.md second
HISTORY notes.md
BIGGEST_TREES 10
RECENT_FILES 10
```

---

## 11. How to Run the Test Cases

1. Copy any test block above into a file, e.g., `test.in`.
2. Run the program with:
   ```
   ./main < test.in
   ```
3. Compare the output to the expected results (timestamps will differ).
