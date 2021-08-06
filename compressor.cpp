#include <iostream>
#include <getopt.h>
#include <vector>
#include "binaryin.h"
#include "binaryout.h"
#include "constants.h"
#include "DLB.h"

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
 * @param trie DLB trie
 */
void init_trie(DLB &trie) {
    for (int i = 0; i < lzw::R; ++i) trie.add(std::string(1, (char) i), i);
}

/**
 * Initialize symbol table with all 8-bit ASCII values.
 *
 * @param st symbol table
 */
void init_symbol_table(std::vector<std::string> &st) {
    for (int i = 0; i < lzw::R; ++i) st[i] = std::string(1, (char) i);
    st[lzw::R] = "";
}

/**
 * Compresses file using a modified LZW compression algorithm.
 *
 * @param name file name
 * @param reset Whether to reset LZW keys dictionary
 */
void compress(const std::string &name, const bool reset) {
    BinaryIn in(name);
    BinaryOut out(name.substr(0, name.find_last_of('.')) + lzw::EXT_C);

    unsigned char L = lzw::L_MIN;   // Length of codewords
    unsigned int N = 1 << L;        // Number of codewords (2^W)

    DLB trie;
    init_trie(trie);
    int code = lzw::R + 1;  // Value R reserved for EOF

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

            // Extend codewords
            if (code >= N && L < lzw::L_MAX) {
                ++L;
                N <<= 1;
            }

            if (code < N) {
                trie.add(to_add, code++);
            } else if (L >= lzw::L_MAX && reset) {
                // Reset codewords
                L = lzw::L_MIN;
                N = 1 << L;

                // Reset trie
                trie = DLB();
                init_trie(trie);
                code = lzw::R + 1;
            }

            // Reset strings for next match
            match = next;
            to_add = next;
        }
    }

    out.write_bits(trie.get(match), L); // Write final remaining match
    out.write_bits(lzw::R, L);   // Write EOF
    out.close();
    in.close();
}

/**
 * Decompresses file using a modified LZW decompression algorithm.
 *
 * @param name file name
 */
void decompress(const std::string &name) {
    BinaryIn in(name);
    BinaryOut out(name.substr(0, name.find_last_of('.')) + lzw::EXT_D);

    unsigned char L = lzw::L_MIN;   // Length of codewords
    unsigned int N = 1 << L;        // Number of codewords (2^W)

    std::vector<std::string> st(N);
    init_symbol_table(st);
    int i = lzw::R + 1; // Index R reserved for EOF
    bool reset = in.read_bit(); // Flag for reset

    // Input is always one step ahead of output
    int codeword = in.read_bits(L);
    std::string val = st[codeword];
    out.write_str(val);

    for (codeword = in.read_bits(L); codeword != lzw::R;
         codeword = in.read_bits(L)) {
        std::string s;
        if (i == codeword) s = val + val[0];    // Edge case
        else s = st[codeword];

        if (i < N) st[i++] = val + s[0];

        val = s;
        out.write_str(val);

        if (i >= N) {
            if (L < lzw::L_MAX) {
                // Extend codewords
                ++L;
                N <<= 1;
                st.resize(N);
            } else if (reset) {
                // Reset codewords
                L = lzw::L_MIN;
                N = 1 << L;

                // Reset symbol table
                st = std::vector<std::string>(N);
                init_symbol_table(st);
                i = lzw::R + 1;

                // Read extra codeword after reset
                codeword = in.read_bits(L);
                val = st[codeword];
                out.write_str(val);
            }
        }
    }

    out.close();
    in.close();
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

    if (comp) {
        while (optind < argc) {
            std::string file = argv[optind++];
            size_t ext_pos = file.find_last_of('.');
            if (ext_pos != std::string::npos) {
                if (file.substr(ext_pos) == lzw::EXT_C) {
                    std::cout << file << " is already compressed; skipping..."
                              << std::endl; // Skip file if already compressed
                } else {
                    compress(file, reset);
                }
            } else {
                std::cout << "Invalid file name \"" << file << "\"; skipping..."
                          << std::endl; // No extension detected
            }
        }
    } else {
        while (optind < argc) {
            std::string file = argv[optind++];
            size_t ext_pos = file.find_last_of('.');
            if (ext_pos != std::string::npos) {
                if (file.substr(ext_pos) == lzw::EXT_C) {
                    decompress(file);
                } else {
                    std::cout << file << " is not compressed; skipping..."
                              << std::endl; // Skip file if not compressed
                }
            } else {
                std::cout << "Invalid file name \"" << file << "\"; skipping..."
                          << std::endl; // No extension detected
            }
        }
    }

    return 0;
}
