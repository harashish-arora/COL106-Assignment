#ifndef HEAP_POS_MAP_HPP // Prevents multiple inclusion of this header file
#define HEAP_POS_MAP_HPP

#include "heap.hpp"      // Includes heap definitions
#include <vector>        // For std::vector
#include <string>        // For std::string
#include <stdexcept>     // For exception handling

// HeapPos class provides a hash table mapping string keys to integer positions
class HeapPos {
private:
    std::vector<std::vector<std::pair<std::string, int>>> buckets; // Buckets for hash table, each bucket is a vector of (key, position) pairs
    int bucket_count;    // Number of buckets in the hash table
    int n_entries;       // Total number of entries in the hash table

    static constexpr unsigned long long P = 131; // Prime base for string hashing

    // Hashes a string key to an integer bucket index
    int hash_str(const std::string& key) const {
        unsigned long long hash_value = 0;
        for (char c : key) {
            hash_value = hash_value * P + c; // Polynomial rolling hash
        }
        return hash_value % bucket_count; // Map hash value to bucket index
    }

public:
    // Constructor: initializes hash table with given number of buckets (default 1009)
    HeapPos(int b = 1009) {
        bucket_count = b;
        n_entries = 0;
        buckets.resize(bucket_count); // Resize buckets vector
    }
    // Inserts or updates a (key, position) pair in the hash table
    void put(const std::string& key, int pos) {
        int idx = hash_str(key); // Compute bucket index
        for (auto& pr : buckets[idx]) {
            if (pr.first == key) {
                pr.second = pos; // Update position if key exists
                return;
            }
        }
        buckets[idx].emplace_back(key, pos); // Add new pair if key does not exist
        n_entries++;
    }
    // Retrieves the position associated with the given key, or -1 if not found
    int get(const std::string& key) const {
        int idx = hash_str(key); // Compute bucket index
        for (const auto& pr : buckets[idx]) {
            if (pr.first == key) return pr.second; // Return position if key matches
        }
        return -1; // Not found
    }
    // Checks if a key exists in the hash table
    bool exists(const std::string& key) const {
        return get(key) != -1;
    }

};

#endif // End of include guard
