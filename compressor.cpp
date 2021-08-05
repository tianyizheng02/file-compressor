#include <iostream>
#include <getopt.h>
#include "binaryin.h"
#include "binaryout.h"
#include "constants.h"
#include "DLB.h"

const unsigned int R = 256;     // Number of input chars
const unsigned char L_MIN = 9;  // Min length of codewords
const unsigned char L_MAX = 16; // Max length of codewords
unsigned char L = L_MIN;        // Length of codewords
unsigned int N = 1 << L;        // Number of codewords (2^W)

/**
 * Displays help menu.
 *
 * @param program program name (argv[0])
 */
void show_help(const std::string &program) {
    std::cout << ansi::BOLD << "USAGE: " << program << ansi::RESET
              << " [" << ansi::UL << "option" << ansi::RESET << "] "
              << ansi::UL << "file-name" << ansi::RESET << "\n" << std::endl;

    std::cout << std::left << std::setw(4) << "-d"
              << std::setw(16) << "--decompress"
              << "Decompress a file" << "\n"
              << std::setw(4) << "-h"
              << std::setw(16) << "--help"
              << "Open this help menu (requires no file name)" << "\n"
              << std::setw(4) << "-r"
              << std::setw(16) << "--reset"
              << "Reset the keys dictionary during compression" << std::endl;
}

/**
 * Initialize DLB trie with all 8-bit ASCII values.
 *
 * @return initialized DLB
 */
DLB init_trie() {
    DLB trie;
    for (int i = 0; i < R; ++i) trie.add(std::string(1, (char) i), i);
    return trie;
}

/**
 * Increase codeword length by 1 and double number of codewords.
 */
void extend_codes() {
    ++L;
    N <<= 1;
}

/**
 * Reset codeword length and number of codewords to default min values.
 */
void reset_codes() {
    L = L_MIN;
    N = 1 << L;
}

/**
 * Compresses file using a modified LZW compression algorithm.
 *
 * @param name file name
 * @param reset Whether to reset LZW keys dictionary
 */
void compress(const std::string &name, const bool reset) {
    BinaryIn in(name);
    BinaryOut out(name + ".lzw");

    DLB trie = init_trie();
    int code = R + 1;   // Value R reserved for EOF

    out.write_bit(reset);   // Flag for reset
    std::string match;
    std::string to_add;

    while (!in.is_empty()) {
        char next = in.read_byte();
        to_add += next;

        if (trie.is_prefix(to_add)) {
            match += next;  // Longest prefix not found yet
        } else {
            out.write_bits(trie.get(match), L);
            if (code >= N && L < L_MAX) extend_codes();

            if (code < N) {
                trie.add(to_add, code++);
            } else if (L >= L_MAX && reset) {
                trie = init_trie();
                reset_codes();
                code = R + 1;
            }

            // Reset for next match
            match = next;
            to_add = next;
        }
    }

    out.write_bits(trie.get(match), L); // Write final remaining match
    out.write_bits(R, L);   // Write EOF
    out.close();
    in.close();
}

/**
 * Decompresses file using a modified LZW decompression algorithm.
 *
 * @param name file name
 */
void decompress(const std::string &name) {
    // TODO: Implement LZW decompress
}

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << ansi::BOLD_RED << "ERROR: No arguments detected"
                  << ansi::RESET << "\n" << std::endl;
        show_help(argv[0]);
        return 1;
    }

    bool comp = true;   // Compress or decompress
    bool reset = false; // Reset LZW keys dictionary or not

    opterr = 0; // Don't output default error messages for invalid options
    struct option options[] = {
            {"decompress", no_argument, nullptr, 'd'},
            {"help",       no_argument, nullptr, 'h'},
            {"reset",      no_argument, nullptr, 'r'},
            {nullptr,      no_argument, nullptr, 0}
    };

    switch (getopt_long(argc, argv, "dhr", options, nullptr)) {
        case -1:    // "argv[0] ..."
            break;
        case 'd':   // "argv[0] -d ..."
            comp = false;
            break;
        case 'r':   // "argv[0] -r ..."
            reset = true;
            break;
        case 'h':   // "argv[0] -h ..."
            show_help(argv[0]);
            return 0;
        default:
            std::cout << ansi::BOLD_RED << "ERROR: Unknown option detected"
                      << ansi::RESET << "\n" << std::endl;
            show_help(argv[0]);
            return 1;
    }

    // Multiple options
    if (getopt_long(argc, argv, "dhr", options, nullptr) != -1) {
        std::cout << ansi::BOLD_RED << "ERROR: multiple options detected"
                  << ansi::RESET << "\n" << std::endl;
        show_help(argv[0]);
        return 1;
    }

    // Option -d or -r with no file name
    if (optind == argc) {
        std::cout << ansi::BOLD_RED << "ERROR: No file detected"
                  << ansi::RESET << "\n" << std::endl;
        show_help(argv[0]);
        return 1;
    }

    if (comp) while (optind < argc) compress(argv[optind++], reset);
    else while (optind < argc) decompress(argv[optind++]);

    return 0;
}
