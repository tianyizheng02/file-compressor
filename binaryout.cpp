#include <iostream>
#include "binaryout.h"
#include "constants.h"

BinaryOut::BinaryOut(const std::string &name) {
    open(name);
}

/**
 * Opens output file if it exists.
 *
 * @param name file name
 */
void BinaryOut::open(const std::string &name) {
    file.open(name, std::ifstream::binary);
    if (!file.is_open()) {
        std::cout << ansi::BOLD_RED << "ERROR: output file " << name
                  << " not found or could not be created"
                  << ansi::RESET << std::endl;
        exit(1);
    }
}

/**
 * Closes output file stream if open.
 */
void BinaryOut::close() {
    if (!file.is_open()) {
        std::cout << ansi::BOLD_RED << "ERROR: no open output file to close"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    flush();
    file.close();
}

/**
 * Flushes output file stream if open.
 */
void BinaryOut::flush() {
    if (!file.is_open()) {
        std::cout << ansi::BOLD_RED << "ERROR: no open output file to flush"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    clear_buffer();
    file.flush();
}

/**
 * Writes bits in buffer to output file stream.
 */
void BinaryOut::clear_buffer() {
    if (N == 0) return;

    file.put((char) (buffer <<= (8 - N)));  // Left-align bits in buffer
    buffer = 0;
    N = 0;
}

/**
 * Writes bit to output file stream.
 *
 * @param b bit to write as bool
 */
void BinaryOut::write_bit(const bool b) {
    buffer = (buffer << 1) | b;
    if (++N == 8) clear_buffer();
}

/**
 * Writes bits to output file stream.
 *
 * @param B bits to write as int
 * @param n number of bits to write
 */
void BinaryOut::write_bits(const int B, const int n) {
    if (n < 1 || n > 32) {
        std::cout << ansi::BOLD_RED
                  << "ERROR: Invalid number of bits while writing bits"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    // Write whole bytes
    if (n % 8 == 0) {
        write_bytes(B, n / 8);
        return;
    }

    // Write bits one at a time
    for (int i = n - 1; i >= 0; --i) write_bit((B >> i) & 1);
}

/**
 * Writes byte to output file stream.
 *
 * @param c byte to write as char
 */
void BinaryOut::write_byte(const char c) {
    // Buffer is empty, so write byte directly
    if (N == 0) {
        file.put(c);
        return;
    }

    // Write bits one at a time
    for (int i = 7; i >= 0; --i) write_bit((c >> i) & 1);
}

/**
 * Writes bytes to output file stream.
 *
 * @param C bytes to write as int
 * @param n number of bytes to write
 */
void BinaryOut::write_bytes(const int C, const int n) {
    if (n < 1 || n > 4) {
        std::cout << ansi::BOLD_RED
                  << "ERROR: Invalid number of bytes while writing bytes"
                  << ansi::RESET << std::endl;
        exit(1);
    }

    // Write bytes one at a time
    for (int i = (n - 1) * 8; i >= 0; i -= 8)
        write_byte((char) ((C >> i) & 0xFF));
}
