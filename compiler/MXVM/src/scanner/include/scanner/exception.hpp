/**
 * @file exception.hpp
 * @brief Exception class, hex formatting utilities, and terminal color definitions
 * @author Jared Bruni
 */
#ifndef _EXCEPTION_H_1
#define _EXCEPTION_H_1

#include <cstdint>
#include <cstring>
#include <errno.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace mx {

    /** @brief Format a 64-bit value as a hex string with 0x prefix */
    std::string format_hex64(uint64_t value);
    /** @brief Format a 32-bit value as a hex string with 0x prefix */
    std::string format_hex32(uint32_t value);
    /** @brief Format a 16-bit value as a hex string with 0x prefix */
    std::string format_hex16(uint16_t value);
    /** @brief Format an 8-bit value as a hex string with 0x prefix */
    std::string format_hex8(uint8_t value);
    /** @brief Format a signal number as a descriptive string */
    std::string format_signal(uint32_t sig);
    /** @brief Format a 64-bit value as a hex string without 0x prefix */
    std::string format_hex_no_prefix(uint64_t value);

    /**
     * @brief General-purpose exception with errno-aware factory method
     *
     * Extends std::runtime_error. The static error() method appends
     * strerror(errno) to the context string.
     */
    class Exception : public std::runtime_error {
      public:
        /**
         * @brief Construct an exception from a string message
         * @param message Human-readable error description
         */
        explicit Exception(const std::string &message)
            : std::runtime_error(message) {}

        /**
         * @brief Construct an exception from a C string message
         * @param message Human-readable error description
         */
        explicit Exception(const char *message)
            : std::runtime_error(message) {}

        /**
         * @brief Create an exception with errno appended
         * @param context Descriptive context prefix
         * @return Exception with "context: strerror(errno)" message
         */
        static Exception error(const std::string &context) {
            return Exception(context + ": " + std::strerror(errno));
        }
    };

    /** @brief ANSI escape code constants for terminal coloring */
    namespace Color {
        const std::string RESET = "\033[0m";
        const std::string BOLD = "\033[1m";
        const std::string DIM = "\033[2m";
        const std::string BLACK = "\033[30m";
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string BLUE = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN = "\033[36m";
        const std::string WHITE = "\033[37m";
        const std::string BRIGHT_RED = "\033[91m";
        const std::string BRIGHT_GREEN = "\033[92m";
        const std::string BRIGHT_YELLOW = "\033[93m";
        const std::string BRIGHT_BLUE = "\033[94m";
        const std::string BRIGHT_MAGENTA = "\033[95m";
        const std::string BRIGHT_CYAN = "\033[96m";
        const std::string BG_RED = "\033[41m";
        const std::string BG_GREEN = "\033[42m";
        const std::string BG_YELLOW = "\033[43m";
    } // namespace Color
    /** @brief Check whether stdout is a TTY that supports ANSI color */
    inline bool terminal_supports_color() {
        return isatty(STDOUT_FILENO) && getenv("TERM") != nullptr;
    }
    inline bool color_ = terminal_supports_color(); ///< cached color support flag
} // namespace mx

/**
 * @brief Wrap a string with ANSI color codes if terminal supports color
 * @param col The text to colorize
 * @param color The ANSI escape sequence to apply
 * @return Colorized string, or plain text if color is unsupported
 */
inline std::string Col(const std::string &col, std::string color) {
    std::ostringstream o;
    if (mx::color_)
        o << color;
    o << col;
    if (mx::color_)
        o << mx::Color::RESET;
    return o.str();
}

#endif