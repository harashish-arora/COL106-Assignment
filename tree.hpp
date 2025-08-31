#ifndef TREE_HPP
#define TREE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
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
    TreeNode(int version_id, string content, TreeNode* parent = nullptr) {
        this -> version_id = version_id;
        this -> content = content;
        this -> message = "";
        this -> created_timestamp = time(nullptr);
        this -> snapshot_timestamp = NULL;
        this -> parent = parent;
    };
};

#endif
