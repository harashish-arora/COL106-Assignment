// file.hpp
#ifndef FILE_HPP // Prevents multiple inclusion of this header file
#define FILE_HPP

#include "tree.hpp"      // Includes TreeNode definition
#include "hashmap.hpp"   // Includes Map definition
#include <algorithm>     // For std::reverse
#include <ctime>         // For std::time_t and std::time
#include <stdexcept>     // For exception handling
#include <vector>        // For std::vector
#include <string>        // For std::string

// File class manages versioned content using a tree structure
class File{
private:
    std::string file_name;      // Name of the file
    TreeNode* root;             // Root version node
    TreeNode* active_version;   // Currently active version node
    Map version_map;            // Maps version IDs to TreeNode pointers
    int total_versions;         // Total number of versions created
    std::time_t last_modified;  // Timestamp of last modification
public:
    // Constructor: creates a new file with initial root version
    File(const std::string& name) { // CREATE
        file_name = name;
        root = new TreeNode(0); // Create root node with version 0
        root -> snapshot("This is the root"); // Snapshot root with message
        version_map.put(0, root); // Map version 0 to root node
        total_versions = 1; // Initialize version count
        active_version = root; // Set active version to root
        last_modified = std::time(nullptr); // Set last modified timestamp
    }
    // Destructor: deletes the root node (recursively deletes all children)
    ~File() {
        delete root;
    }
    // Returns the content of the active version
    const std::string& Read() const { // READ
        return active_version -> get_content();
    }
    // Inserts content to the active version; creates new version if snapshotted
    void Insert(const std::string& content) { // INSERT
        if (active_version -> is_snapshot()) {
            TreeNode* child = new TreeNode(total_versions, active_version -> get_content() + content, active_version);
            active_version = child;
            version_map.put(total_versions, child);
            total_versions++;
        }
        else {
            active_version -> update_content(active_version -> get_content() + content);
        }
        last_modified = std::time(nullptr);
    }
    // Updates the content of the active version; creates new version if snapshotted
    void Update(const std::string& content) { // UPDATE
        if (active_version -> is_snapshot()) {
            TreeNode* child = new TreeNode(total_versions, content, active_version);
            active_version = child;
            version_map.put(total_versions, child);
            total_versions++;
        }
        else {
            active_version -> update_content(content);
        }
        last_modified = std::time(nullptr);
    }
    // Snapshots the active version with a message
    void Snapshot(const std::string& message = "This version has been snapshotted") {
        active_version -> snapshot(message);
    }
    // Rolls back to a previous version by ID or to parent if no ID is given
    void Rollback(int versionID = -1) {
        if (versionID == -1) {
            TreeNode* parent = active_version -> get_parent();
            if (!parent) {
                throw std::invalid_argument("Already at root; cannot rollback to parent");
            }
            active_version = parent;
        }
        else {
            TreeNode* target = version_map.get(versionID);
            if (!target) {
                throw std::invalid_argument("Supplied version ID does not exist");
            }
            active_version = target;
        }
    }
    // Returns a vector of all snapshotted versions from root to active
    std::vector<TreeNode*> History() const {
        std::vector<TreeNode*> result;
        TreeNode* cur = active_version;

        while (cur) {
            if (cur -> is_snapshot()) {
                result.push_back(cur);
            }
            cur = cur -> get_parent();
        }

        std::reverse(result.begin(), result.end());
        return result;
    }
    // Returns the file name
    const std::string& get_filename() const {
        return file_name;
    }
    // Returns the last modified timestamp
    std::time_t get_last_modified() const {
        return last_modified;
    }
    // Returns the total number of versions
    int get_total_versions() const {
        return total_versions;
    }
    // Returns the currently active version node
    TreeNode* get_active_version() const {
        return active_version;
    }

};

#endif // End of include guard
