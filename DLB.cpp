#include "DLB.h"

DLB::~DLB() {
    delete_all(root);
}

/**
 * Frees dynamically allocated memory for node, its sibling, and its child.
 *
 * @param node current node
 */
void DLB::delete_all(Node *node) {
    if (node == nullptr) return;

    delete_all(node->right);
    delete_all(node->down);

    delete node;
}

/**
 * Gets value of key in trie.
 *
 * @param key key
 * @return value of key or -1 if key not found
 */
int DLB::get(const std::string &key) {
    if (root == nullptr || key.length() == 0) return -1;

    Node *curr = root;
    for (int i = 0; i < key.length(); ++i) {
        while (curr != nullptr && curr->c != key[i]) curr = curr->right;

        if (curr == nullptr) return -1;

        // Descend if not last char
        if (i != key.length() - 1) curr = curr->down;
    }

    return curr->val;
}

/**
 * Adds key and its value to trie.
 *
 * @param key key
 * @param val value of key
 */
void DLB::add(const std::string &key, const int val) {
    if (key.length() == 0 || val < 0) return;

    // Add first node to trie if previously empty
    if (root == nullptr) root = new Node{key[0], -1, nullptr, nullptr};

    Node *curr = root;
    for (int i = 0; i < key.length(); ++i) {
        while (curr->c != key[i]) {
            // Add new node to current level if char not found
            if (curr->right == nullptr) {
                curr->right = new Node{key[i], -1, nullptr, nullptr};
                curr = curr->right;
                break;
            }

            curr = curr->right;
        }

        // Descend if not last char
        if (i != key.length() - 1) {
            if (curr->down == nullptr)
                curr->down = new Node{key[i + 1], -1, nullptr, nullptr};
            curr = curr->down;
        }
    }

    curr->val = val;
}

/**
 * Checks if key is prefix to existing key; keys can be their own prefix.
 *
 * @param key key
 * @return true if is prefix and false if otherwise
 */
bool DLB::is_prefix(const std::string &key) {
    if (root == nullptr || key.length() == 0) return false;

    Node *curr = root;
    for (char c : key) {
        while (curr != nullptr && curr->c != c) curr = curr->right;

        if (curr == nullptr) return false;

        curr = curr->down;
    }

    return true;
}
