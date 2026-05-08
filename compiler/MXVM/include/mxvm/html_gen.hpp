/**
 * @file html_gen.hpp
 * @brief HTML documentation generator from MXVM source code
 * @author Jared Bruni
 */
#ifndef __HTML__GEN___H_
#define __HTML__GEN___H_

#include <iosfwd>
#include <string>

namespace mxvm {

    /** @brief Generates syntax-highlighted HTML output from MXVM source code */
    class HTMLGen {
      public:
        /**
         * @brief Construct an HTML generator from MXVM source text
         * @param src MXVM source code string
         */
        HTMLGen(const std::string &src);

        /**
         * @brief Write the generated HTML to an output stream
         * @param out Output stream to write HTML to
         * @param filename Source filename used in the HTML title
         */
        void output(std::ostream &out, std::string filename);

      private:
        std::string source;  ///< the MXVM source text to convert
    };

    /**
     * @brief Generates syntax-highlighted HTML output from Pascal (.pas) source code
     *
     * Supports all keywords, types, and built-ins recognised by the MXVM Pascal
     * compiler (from both the parser and validator keyword sets), plus Pascal
     * comment styles (@c { }, @c (* *), @c //), single-quoted string literals
     * with @c '' escape, hex literals (@c $FF), and char literals (@c #65 / @c #$41).
     *
     * The generated HTML links to an external stylesheet instead of embedding
     * styles inline, so colours and fonts can be customised by editing that file.
     * Call @c defaultCss() to obtain the bundled default stylesheet text.
     */
    class PasHTMLGen {
      public:
        /** @brief Construct from Pascal source text */
        explicit PasHTMLGen(const std::string &src);

        /**
         * @brief Write generated HTML to an output stream
         * @param out      Output stream
         * @param filename Source filename (used in the HTML @c \<title\>)
         * @param cssHref  @c href for the linked stylesheet (default: @c "mxvm-highlight.css")
         */
        void output(std::ostream &out, const std::string &filename,
                    const std::string &cssHref = "mxvm-highlight.css");

        /**
         * @brief Return the default stylesheet text
         *
         * Covers all span classes used by both HTMLGen (.mxvm) and PasHTMLGen (.pas)
         * output.  Write this string to @c mxvm-highlight.css alongside the HTML
         * files and edit it freely to change the colour scheme.
         */
        static std::string defaultCss();

      private:
        std::string source;
    };
} // namespace mxvm

#endif