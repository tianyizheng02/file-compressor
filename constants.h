#ifndef FILE_COMPRESSOR_CONSTANTS_H
#define FILE_COMPRESSOR_CONSTANTS_H

namespace ansi {
    inline constexpr char RESET[] = "\033[0m";
    inline constexpr char BOLD[] = "\033[1m";
    inline constexpr char UL[] = "\033[4m";
    inline constexpr char BOLD_RED[] = "\033[1;31m";
}

namespace lzw {
    inline constexpr char EXT_C[] = ".lzwc";    // Extension for compressed files
    inline constexpr char EXT_D[] = ".lzwd";    // Extension for decompressed files
    inline constexpr unsigned int R = 256;      // Number of input chars
    inline constexpr unsigned char L_MIN = 9;   // Min length of codewords
    inline constexpr unsigned char L_MAX = 16;  // Max length of codewords
}

#endif //FILE_COMPRESSOR_CONSTANTS_H
