#include "file.hpp"      // File class for versioned files
#include "file_hash.hpp" // FileHash for mapping filenames to File*
#include "heap.hpp"      // MaxHeap for recent and biggest files
#include <iostream>      // For input/output
#include <sstream>       // For stringstream

using namespace std;

// Global file table and heaps
FileHash file_table; // Maps filename to File*
MaxHeap recentHeap(cmp_recent);   // Heap for most recently modified files
MaxHeap biggestHeap(cmp_biggest); // Heap for files with most versions

// Updates both heaps with the given file
void update_heaps(File* f) {
    recentHeap.update(f);
    biggestHeap.update(f);
}

// ---------------- COMMAND HANDLERS ----------------

// CREATE
void handle_create(stringstream& ss) {
    string fname;
    if (!(ss >> fname)) {
        cout << "Error: Invalid command. Usage: CREATE <filename>\n";
        return;
    }
    if (file_table.exists(fname)) {
        cout << "Error: File '" << fname << "' already exists.\n";
        return;
    }
    File* f = new File(fname);
    file_table.put(fname, f);
    recentHeap.insert(f);
    biggestHeap.insert(f);
    cout << "File '" << fname << "' created successfully.\n";
}

// READ
void handle_read(stringstream& ss) {
    string fname;
    if (!(ss >> fname)) {
        cout << "Error: Invalid command. Usage: READ <filename>\n";
        return;
    }
    File* f = file_table.get(fname);
    if (!f) { 
        cout << "Error: File '" << fname << "' not found.\n"; 
        return; 
    }
    cout << "Content of '" << fname << "' (Version " 
         << f->get_active_version()->get_version_id() << "):\n"
         << f->Read() << "\n";
}

// INSERT / UPDATE
void handle_insert_update(stringstream& ss, bool is_insert) {
    string fname;
    if (!(ss >> fname)) {
        cout << "Error: Invalid command. Usage: " 
             << (is_insert ? "INSERT" : "UPDATE") << " <filename> <content>\n";
        return;
    }
    string content;
    getline(ss, content);
    if (content.empty() || content == " ") {
        cout << "Error: Invalid command. Usage: " 
             << (is_insert ? "INSERT" : "UPDATE") << " <filename> <content>\n";
        return;
    }
    if (content[0] == ' ') content.erase(0,1);

    File* f = file_table.get(fname);
    if (!f) {
        cout << "Error: File '" << fname << "' not found.\n";
        return;
    }

    if (is_insert) f->Insert(content);
    else f->Update(content);

    update_heaps(f);

    TreeNode* active = f->get_active_version();
    TreeNode* parent = active->get_parent();
    cout << "New version " << active->get_version_id()
         << " created for '" << fname
         << "'. Parent is version "
         << (parent ? parent->get_version_id() : -1)
         << ".\n";
}

// SNAPSHOT
void handle_snapshot(stringstream& ss) {
    string fname;
    if (!(ss >> fname)) {
        cout << "Error: Invalid command. Usage: SNAPSHOT <filename> <message>\n";
        return;
    }
    string message;
    getline(ss, message);
    if (message.empty() || message == " ") {
        cout << "Error: Invalid command. Usage: SNAPSHOT <filename> <message>\n";
        return;
    }
    if (message[0] == ' ') message.erase(0,1);

    File* f = file_table.get(fname);
    if (!f) {
        cout << "Error: File '" << fname << "' not found.\n";
        return;
    }

    try {
        f->Snapshot(message);
        cout << "Snapshot created for '" << fname 
             << "' with message: " << message << "\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

// ROLLBACK
void handle_rollback(stringstream& ss) {
    string fname;
    if (!(ss >> fname)) {
        cout << "Error: Invalid command. Usage: ROLLBACK <filename> [versionID]\n";
        return;
    }
    File* f = file_table.get(fname);
    if (!f) { 
        cout << "Error: File '" << fname << "' not found.\n"; 
        return;
    }

    int versionID;
    if (ss >> versionID) {
        if (versionID < 0) {
            cout << "Error: VersionID must be non-negative.\n";
            return;
        }
        try {
            f->Rollback(versionID);
            cout << "Active version for '" << fname 
                 << "' set to " << versionID << ".\n";
        } catch (...) {
            cout << "Error: Version " << versionID 
                 << " not found for file '" << fname << "'.\n";
        }
    } else {
        TreeNode* active = f->get_active_version();
        TreeNode* parent = (active ? active->get_parent() : nullptr);
        if (!parent) {
            cout << "Error: Cannot rollback from root version.\n";
            return;
        }
        int parentID = parent->get_version_id();
        try {
            f->Rollback();
            cout << "Active version for '" << fname 
                 << "' set to parent version " << parentID << ".\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

// HISTORY
void handle_history(stringstream& ss) {
    string fname;
    if (!(ss >> fname)) {
        cout << "Error: Invalid command. Usage: HISTORY <filename>\n";
        return;
    }
    File* f = file_table.get(fname);
    if (!f) { 
        cout << "Error: File '" << fname << "' not found.\n"; 
        return; 
    }
    auto hist = f->History();
    for (auto* node : hist) {
        cout << node->get_version_id() << " "
             << node->get_snapshot_time() << " "
             << node->get_message() << "\n";
    }
}

// RECENT_FILES / BIGGEST_TREES
void handle_heap_query(MaxHeap& heap, stringstream& ss, bool is_recent) {
    int num;
    if (!(ss >> num)) {
        cout << "Error: Invalid command. Usage: " 
             << (is_recent ? "RECENT_FILES <k>" : "BIGGEST_TREES <k>") << "\n";
        return;
    }
    if (num <= 0) {
        cout << "Error: Invalid command. k must be positive.\n";
        return;
    }
    if (num > heap.size()) {
        cout << "Error: k cannot exceed number of files. Currently only " 
             << heap.size() << " file(s) exist.\n";
        return;
    }

    vector<File*> results;
    for (int i = 0; i < num; i++) {
        File* f = heap.extract_max();
        results.push_back(f);
    }
    for (File* f : results) {
        cout << f->get_filename() << " "
             << (is_recent ? f->get_last_modified() : f->get_total_versions()) << "\n";
        heap.insert(f); // put back
    }
}

// ---------------- MAIN LOOP ----------------

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cmd;
        ss >> cmd;

        try {
            if (cmd == "CREATE") handle_create(ss);
            else if (cmd == "READ") handle_read(ss);
            else if (cmd == "INSERT") handle_insert_update(ss, true);
            else if (cmd == "UPDATE") handle_insert_update(ss, false);
            else if (cmd == "SNAPSHOT") handle_snapshot(ss);
            else if (cmd == "ROLLBACK") handle_rollback(ss);
            else if (cmd == "HISTORY") handle_history(ss);
            else if (cmd == "RECENT_FILES") handle_heap_query(recentHeap, ss, true);
            else if (cmd == "BIGGEST_TREES") handle_heap_query(biggestHeap, ss, false);
            else cout << "Error: Unknown command '" << cmd << "'.\n";
        }
        catch (exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}
