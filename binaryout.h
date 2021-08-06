#ifndef FILE_COMPRESSOR_BINARYOUT_H
#define FILE_COMPRESSOR_BINARYOUT_H

#include <fstream>

class BinaryOut {
public:
    explicit BinaryOut(const std::string &name);

    void close();

    void flush();

    void write_bit(bool b);

    void write_bits(int B, int n);

    void write_byte(char c);

    void write_bytes(int C, int n);

    void write_str(const std::string &str);

private:
    std::ofstream file;
    unsigned char buffer = 0;
    char N = 0;

    void open(const std::string &name);

    void clear_buffer();
};


#endif //FILE_COMPRESSOR_BINARYOUT_H
