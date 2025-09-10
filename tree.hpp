// tree.hpp
#ifndef TREE_HPP // Prevents multiple inclusion of this header file
#define TREE_HPP

#include <string>      // For std::string
#include <vector>      // For std::vector
#include <ctime>       // For std::time_t and std::time
#include <stdexcept>   // For exception handling

// TreeNode class represents a node in a version tree
class TreeNode {
private:
    int version_id;                    // Unique identifier for the version
    std::string content;               // Content stored in this version
    std::string message;               // Snapshot message
    std::time_t created_timestamp;     // Timestamp when node was created
    std::time_t snapshot_timestamp;    // Timestamp when node was snapshotted (0 if not snapshotted)
    TreeNode* parent;                  // Pointer to parent node
    std::vector<TreeNode*> children;   // List of child nodes

public:
    // Constructor: initializes a TreeNode with given version_id, content, and optional parent
    TreeNode(int version_id, const std::string& content = "", TreeNode* parent = nullptr)
        : version_id(version_id),
          content(content),
          message(""),
          created_timestamp(std::time(nullptr)),
          snapshot_timestamp(0),
          parent(nullptr) // add_child will set this
    {
        if (version_id < 0) {
            throw std::invalid_argument("Version ID must be non-negative"); // Ensure valid version_id
        }
        if (parent) { // If parent is provided, add this node as its child
            parent -> add_child(this); // Ensures both sides are updated
        }
    }

    // Disable copy constructor and assignment operator to prevent shallow copy issues
    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;

    // Destructor: deletes all child nodes recursively
    ~TreeNode() {
        for (auto* c : children) {delete c;}
    }

    // Getters for private members
    int get_version_id() const {return version_id;} // Returns version_id
    const std::string& get_content() const {return content;} // Returns content
    const std::string& get_message() const {return message;} // Returns snapshot message
    std::time_t get_created_time() const {return created_timestamp;} // Returns creation timestamp
    std::time_t get_snapshot_time() const {
        if (!is_snapshot()) {throw std::logic_error("Version has not been snapshotted");}
        return snapshot_timestamp; // Returns snapshot timestamp if snapshotted
    }
    bool is_snapshot() const {return snapshot_timestamp != 0;} // Checks if node is snapshotted
    TreeNode* get_parent() const {return parent;} // Returns parent node pointer
    const std::vector<TreeNode*>& get_children() const {return children;} // Returns children vector

    // Adds a child node to this node
    void add_child(TreeNode* child) {
        if (!child) throw std::invalid_argument("child is null"); // Ensure child is not null
        if (child -> parent == this) return; // Already added
        child -> parent = this; // Set child's parent pointer
        children.push_back(child); // Add child to children vector
    }

    // Updates the content of this node if not snapshotted
    void update_content(const std::string& new_content) {
        if (is_snapshot()) {
            throw std::logic_error("Version has been snapshotted, can't update content");
        }
        content = new_content;
    }

    // Snapshots this node with a message
    void snapshot(const std::string& msg) {
        if (is_snapshot()) {throw std::logic_error("Version already snapshotted");}
        if (msg.empty()) {throw std::invalid_argument("Message can't be empty");}
        message = msg;
        snapshot_timestamp = std::time(nullptr); // Set snapshot timestamp
    }
};

#endif // End of include guard
