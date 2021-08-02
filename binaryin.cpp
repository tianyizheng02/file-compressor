#include <iostream>
#include "binaryin.h"
#include "constants.h"

BinaryIn::BinaryIn(const std::string &name) {
    open(name);
    fill_buffer();
}

/**
 * Opens input file if it exists.
 *
 * @param name file name
 */
void BinaryIn::open(const std::string &name) {
    file.open(name, std::ifstream::binary);
    if (!file.is_open()) {
        std::cout << ansi::BOLD_RED << "ERROR: input file " << name
                  << " not found or could not be opened"
                  << ansi::RESET << std::endl;
        exit(1);
    }
}

/**
 * Closes input file stream if open.
 */
void BinaryIn::close() {
    if (!file.is_open()) {
        std::cout << ansi::BOLD_RED << "ERROR: no open file to close"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    file.close();
}

/**
 * Checks whether input file stream is empty.
 *
 * @return true if file stream is empty and false if otherwise
 */
bool BinaryIn::is_empty() const {
    return empty;
}

/**
 * Fills buffer with next char in input file stream.
 */
void BinaryIn::fill_buffer() {
    int x = file.get();
    if (x == EOF) {
        empty = true;
        return;
    }

    buffer = (unsigned char) x;
    N = 8;
}

/**
 * Reads bit from input file stream.
 *
 * @return bit from input file stream as bool
 */
bool BinaryIn::read_bit() {
    if (is_empty()) {
        std::cout << ansi::BOLD_RED << "ERROR: EOF reached while reading bit"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    bool b = (buffer >> --N) & 1;   // Leftmost bit in buffer
    if (N == 0) fill_buffer();

    return b;
}

/**
 * Reads bits from input file stream.
 *
 * @param B number of bits
 * @return bits from input file stream as int
 */
int BinaryIn::read_bits(const int B) {
    if (is_empty()) {
        std::cout << ansi::BOLD_RED << "ERROR: EOF reached while reading bits"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    if (B < 1 || B > 32) {
        std::cout << ansi::BOLD_RED
                  << "ERROR: Invalid number of bits while reading bits"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    if (B % 8 == 0) return read_bytes(B / 8);   // Read whole bytes

    // Read bits one at a time
    int x = 0;
    for (int i = 0; i < B; ++i) {
        x <<= 1;
        x |= read_bit();
    }

    return x;
}

/**
 * Reads byte from input file stream.
 *
 * @return byte from input file stream as byte
 */
char BinaryIn::read_byte() {
    if (is_empty()) {
        std::cout << ansi::BOLD_RED << "ERROR: EOF reached while reading byte"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    // Buffer contains exactly 1 byte
    if (N == 8) {
        char c = (char) buffer;
        fill_buffer();
        return c;
    }

    char c = (char) (buffer << (8 - N));    // Left-align bits of buffer
    char temp_N = N;

    fill_buffer();
    if (is_empty()) {
        std::cout << ansi::BOLD_RED << "ERROR: EOF reached while reading char"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    // N bits of buffer + (8 - N) bits of next char
    return (char) (c | (buffer >> temp_N));
}

/**
 * Reads bytes from input file stream.
 *
 * @param B number of bytes
 * @return bytes from input file stream as bytes
 */
int BinaryIn::read_bytes(const int B) {
    if (is_empty()) {
        std::cout << ansi::BOLD_RED << "ERROR: EOF reached while reading bytes"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    if (B < 0 || B > 4) {
        std::cout << ansi::BOLD_RED
                  << "ERROR: Invalid number of bytes while reading bytes"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    if (B == 1) return read_byte();

    // Read bytes one at a time
    int x = 0;
    for (int i = 0; i < B; ++i) {
        x <<= 8;
        x |= read_byte();
    }

    return x;
}
