#ifndef FILE_COMPRESSOR_DLB_H
#define FILE_COMPRESSOR_DLB_H

#include <string>

class DLB {

public:
    struct Node {
        char c;
        int val;
        Node *next;
        Node *child;
    };

    DLB() = default;

    int get(const std::string &key);

    void add(const std::string &key, int val);

    bool is_prefix(const std::string &key);

private:
    Node *root = nullptr;
};

#endif
