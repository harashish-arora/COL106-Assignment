#ifndef HEAP_HPP // Prevents multiple inclusion of this header file
#define HEAP_HPP

#include "file.hpp"         // Includes File class definition
#include "heap_pos_map.hpp" // Includes HeapPos for position mapping
#include <string>           // For std::string
#include <vector>           // For std::vector
#include <stdexcept>        // For exception handling

// MaxHeap class implements a max-heap for File* objects using a custom comparator
class MaxHeap {
private:
    std::vector<File*> heap;           // Internal heap storage
    bool (*cmp)(File*, File*);         // Comparator function for heap ordering
    HeapPos pos;                       // Maps filename to position in heap

    // Returns parent index of node i
    int parent(int i) const {return (i - 1) / 2;}
    // Returns left child index of node i
    int left(int i) const {return 2 * i + 1;}
    // Returns right child index of node i
    int right(int i) const {return 2 * i + 2;}

    // Swaps two nodes in the heap and updates their positions
    void swap_nodes(int i, int j) {
        std::swap(heap[i], heap[j]);
        pos.put(heap[i] -> get_filename(), i);
        pos.put(heap[j] -> get_filename(), j);
    }

    // Moves node at index i up to restore heap property
    void bubble_up(int i) {
        while (i > 0 && cmp(heap[i], heap[parent(i)])) {
            swap_nodes(i, parent(i));
            i = parent(i);
        }
    }

    // Moves node at index i down to restore heap property
    void bubble_down(int i) {
        int n = heap.size();
        while (true) {
            int l = left(i);
            int r = right(i);
            int largest = i;
            
            if (l < n && cmp(heap[l], heap[largest])) largest = l;
            if (r < n && cmp(heap[r], heap[largest])) largest = r;
            
            if (largest != i) {
                swap_nodes(i, largest);
                i = largest;
            }
            else break;
        }
    }

public:
    // Constructor: initializes heap with comparator and position map
    MaxHeap(bool (*cmp_func)(File*, File*)) : cmp(cmp_func), pos(1009) {}

    // Returns true if heap is empty
    bool empty() const {return heap.empty();}
    // Returns number of elements in heap
    int size() const {return heap.size();}

    // Returns pointer to max element (root of heap)
    File* peek() const {
        if (heap.empty()) throw std::out_of_range("Heap is empty");
        return heap[0];
    }

    // Inserts a File* into the heap
    void insert(File* f) {
        heap.push_back(f);
        int idx = heap.size() - 1;
        pos.put(f -> get_filename(), idx);
        bubble_up(idx);
    }

    // Removes and returns the max element from the heap
    File* extract_max() {
        if (heap.empty()) throw std::out_of_range("Heap is empty");
        File* maxVal = heap[0];
        swap_nodes(0, heap.size() - 1);
        heap.pop_back();
        if (!heap.empty()) bubble_down(0);
        return maxVal;
    }

    // Updates the position of a File* in the heap after its value changes
    void update(File* f) {
        int idx = pos.get(f -> get_filename());
        if (idx == -1) throw std::invalid_argument("File not found in heap");
        bubble_up(idx);
        bubble_down(idx);
    }
};

// Comparator for most recently modified files
bool cmp_recent(File* a, File* b) {
    return a -> get_last_modified() > b -> get_last_modified();
}

// Comparator for files with most versions
bool cmp_biggest(File* a, File* b) {
    return a -> get_total_versions() > b -> get_total_versions();
}

#endif // End of include guard
