#ifndef TREE_HPP
#define TREE_HPP

#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
using namespace std;

class TreeNode {
private:
    int version_id;
    string content;
    string message;
    time_t created_timestamp;
    time_t snapshot_timestamp;
    TreeNode* parent;
    vector<TreeNode*> children;
public:
    TreeNode(int version_id, const string& content = "", TreeNode* parent = nullptr) {
        if (version_id < 0) {
            throw invalid_argument("Version ID must be non-negative");
        }
        this -> version_id = version_id;
        this -> content = content;
        this -> message = "";
        this -> created_timestamp = time(nullptr);
        this -> snapshot_timestamp = 0;
        this -> parent = parent;
    }

    // getter functions
    int get_version_id() const {return version_id;}
    const string& get_content() const {return content;}
    const string& get_message() const {return message;}
    const time_t& get_created_time() const {return created_timestamp;}
    const time_t& get_snapshot_time() const {
        if (!is_snapshot()) {throw logic_error("Version has not been snapshotted");}
        return snapshot_timestamp;
    }
    bool is_snapshot() const {return message != "";}
    TreeNode* get_parent() const {return parent;}
    const vector<TreeNode*>& get_children() const {return children;}
        
    // modifier functions
    void add_child(TreeNode* child) {
        if (child == nullptr) {throw invalid_argument("Child cannot be null");}
        children.push_back(child);
    }
    void update_content(const string& new_content) {
        if (is_snapshot()) {
            throw logic_error("Version has been snapshotted, can't update content");
        }
        content = new_content;
    }
    void snapshot(const string& message) {
        if (is_snapshot()) {
            throw logic_error("Version already snapshotted");
        }
        if (message == "") {
            throw invalid_argument("Message can't be empty");
        }
        this -> message = message;
        this -> snapshot_timestamp = time(nullptr);
    }
};

#endif
