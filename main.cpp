#include "tree.hpp"
#include <iostream>
using namespace std;

int main() {
    try {
        // 1. Create root version
        TreeNode root(0, "Root version");
        cout << "Root created: ID=" << root.get_version_id()
             << ", content=\"" << root.get_content() << "\"\n";

        // 2. Create a child version
        TreeNode child1(1, "Child v1", &root);
        root.add_child(&child1);
        cout << "Child1 created under root: ID=" << child1.get_version_id()
             << ", content=\"" << child1.get_content() << "\"\n";

        // 3. Update content before snapshot
        child1.update_content("Child v1 updated");
        cout << "Child1 after update: content=\"" << child1.get_content() << "\"\n";

        // 4. Snapshot the child
        child1.snapshot("First stable version");
        cout << "Child1 snapshotted with message: \"" << child1.get_message() << "\"\n";

        // 5. Try to update after snapshot (should throw exception)
        try {
            child1.update_content("Another update");
        } catch (const exception& e) {
            cout << "Expected error on update after snapshot: " << e.what() << "\n";
        }

        // 6. Traverse parent/child
        cout << "Child1's parent ID: " << child1.get_parent()->get_version_id() << "\n";
        cout << "Root's first child ID: " << root.get_children()[0]->get_version_id() << "\n";

        // 7. Print creation time
        cout << "Root created at: " << ctime(&root.get_created_time());
        cout << "Child1 snapshot at: " << ctime(&child1.get_snapshot_time());

    } catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << "\n";
    }

    return 0;
}
