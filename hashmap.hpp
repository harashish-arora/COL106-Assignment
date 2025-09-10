// hashmap.hpp
#ifndef HASHMAP_HPP // Prevents multiple inclusion of this header file
#define HASHMAP_HPP

#include "tree.hpp" // Includes the definition for TreeNode
#include <vector>   // Includes the vector container
#include <stdexcept> // Includes standard exception handling

// Map class provides a mapping from integer IDs to TreeNode pointers
class Map {
private:
    std::vector<TreeNode*> v; // Internal storage: vector indexed by id, stores TreeNode pointers
public:
    Map() = default; // Default constructor, creates an empty map
    
    // Associates the given id with the provided TreeNode pointer
    void put(int id, TreeNode* Node) {
        if (id >= static_cast<int>(v.size())) { // If id is out of current bounds, resize vector
            v.resize(id + 1, nullptr); // Resize vector and initialize new elements to nullptr
        }
        v[id] = Node; // Store the TreeNode pointer at index id
    }

    // Returns the TreeNode pointer associated with id, or nullptr if not present
    TreeNode* get(int id) const {
        if (id < 0 || id >= static_cast<int>(v.size())) { // Check for out-of-bounds id
            return nullptr; // Return nullptr if id is invalid
        }
        return v[id]; // Return the TreeNode pointer at index id
    }
};

#endif // End of include guard
