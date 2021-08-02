#include <iostream>
#include <fstream>
#include <getopt.h>
#include "constants.h"

/**
 * Display help menu
 *
 * @param program Program name (argv[0])
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
 * Get file if it exists
 *
 * @param name File name
 * @return input stream for file
 */
std::ifstream find_file(const std::string &name) {
    std::ifstream file(name);
    if (!file.is_open()) {
        std::cout << ansi::BOLD_RED << "ERROR: file " << name
                  << " not found or could not be opened"
                  << ansi::RESET << std::endl;
        file.close();
        exit(1);
    }

    return file;
}

/**
 * Compress file using a modified LZW compression algorithm
 *
 * @param name File name
 * @param reset Whether to reset LZW keys dictionary
 */
void compress(const std::string &name, const bool reset) {
    std::ifstream file = find_file(name);

    // TODO: Implement LZW compress

    file.close();
}

/**
 * Decompress file using a modified LZW decompression algorithm
 *
 * @param name File name
 */
void decompress(const std::string &name) {
    std::ifstream file = find_file(name);

    // TODO: Implement LZW decompress

    file.close();
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
