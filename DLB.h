#ifndef FILE_COMPRESSOR_DLB_H
#define FILE_COMPRESSOR_DLB_H

#include <string>

class DLB {
public:
    struct Node {
        char c;
        int val;
        Node *right;
        Node *down;
    };

    DLB() = default;

    ~DLB();

    int get(const std::string &key);

    void add(const std::string &key, int val);

    bool is_prefix(const std::string &key);

private:
    Node *root = nullptr;

    void delete_all(Node *node);
};

#endif
