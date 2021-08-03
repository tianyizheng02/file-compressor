#ifndef FILE_COMPRESSOR_BINARYIN_H
#define FILE_COMPRESSOR_BINARYIN_H

#include <fstream>

class BinaryIn {
public:
    explicit BinaryIn(const std::string &name);

    void close();

    bool is_empty() const;

    bool read_bit();

    int read_bits(int n);

    char read_byte();

    int read_bytes(int n);

private:
    std::ifstream file;
    unsigned char buffer{}; // 1-byte buffer (must be unsigned for right shifts)
    char N{};   // Number of bits in buffer
    bool empty = false;

    void open(const std::string &name);

    void fill_buffer();
};


#endif //FILE_COMPRESSOR_BINARYIN_H
