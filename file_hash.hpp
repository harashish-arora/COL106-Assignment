#ifndef FILE_HASH_HPP // Prevents multiple inclusion of this header file
#define FILE_HASH_HPP

#include "file.hpp"      // Includes File class definition
#include <vector>        // For std::vector
#include <string>        // For std::string
#include <stdexcept>     // For exception handling

// FileHash class provides a hash table mapping file names to File pointers
class FileHash {
private:
    std::vector<std::vector<std::pair<std::string, File*>>> buckets; // Buckets for hash table, each bucket is a vector of (key, File*) pairs
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
    FileHash(int b = 1009) {
        bucket_count = b;
        n_entries = 0;
        buckets.resize(bucket_count); // Resize buckets vector
    }
    // Inserts a (key, File*) pair into the hash table
    void put(const std::string& key, File* file) {
        int idx = hash_str(key); // Compute bucket index
        buckets[idx].emplace_back(key, file); // Add pair to bucket
        n_entries++;
    }
    // Retrieves the File* associated with the given key, or nullptr if not found
    File* get(const std::string& key) const {
        int idx = hash_str(key); // Compute bucket index
        for (const auto& pr : buckets[idx]) {
            if (pr.first == key) return pr.second; // Return File* if key matches
        }
        return nullptr; // Not found
    }
    // Checks if a key exists in the hash table
    bool exists(const std::string& key) const {
        return get(key) != nullptr;
    }

};

#endif // End of include guard
