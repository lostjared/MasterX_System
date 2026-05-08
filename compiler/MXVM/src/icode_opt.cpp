/**
 * @file icode_opt.cpp
 * @brief Peephole optimizer for generated x86-64 assembly (SysV and Win64)
 * @author Jared Bruni
 */
#include "mxvm/icode.hpp"
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mxvm {

    static std::string join_lines(const std::vector<std::string> &v) {
        std::ostringstream os;
        for (size_t i = 0; i < v.size(); ++i) {
            os << v[i];
            if (i + 1 < v.size())
                os << '\n';
        }
        return os.str();
    }

    /*static inline std::string trim(std::string s) {
        auto issp = [](int c){ return std::isspace(c); };
        while (!s.empty() && issp(s.front())) s.erase(s.begin());
        while (!s.empty() && issp(s.back())) s.pop_back();
        return s;
    }*/

    static bool is_label(const std::string &line) {
        static const std::regex re_label(R"(^\s*(?:[A-Za-z_.$][\w.$]*|\d+)\s*:)");
        return std::regex_search(line, re_label);
    }

    static std::vector<std::string> opt_core_lines(const std::vector<std::string> &lines) {
        std::regex re_mov_reg(R"(^\s*mov[bwlq]?\s+(%[a-z0-9]+)\s*,\s*(%[a-z0-9]+)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_mem_to_reg(R"(^\s*mov[bwlq]?\s+([A-Za-z0-9_]+(?:\(%rip\))?)\s*,\s*(%[a-z0-9]+)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_reg_to_mem(R"(^\s*mov[bwlq]?\s+(%[a-z0-9]+)\s*,\s*([A-Za-z0-9_]+(?:\(%rip\))?)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_modifies_reg(R"(^\s*(?:add|sub|mul|imul|div|idiv|and|or|xor|shl|shr|sal|sar|not|neg)[a-z]*\s+.*,\s*(%[a-z0-9]+)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_call_or_jmp(R"(^\s*(?:call|jmp|je|jne|jz|jnz|jg|jge|jl|jle|ja|jae|jb|jbe)[a-z]*\s+)", std::regex::icase);
        std::regex re_xor_eax(R"(^\s*xorq?\s+%eax\s*,\s*%eax\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_call_func(R"(^\s*call\s+([_A-Za-z][_A-Za-z0-9]*)\s*(?:[#;].*)?$)", std::regex::icase);
        static const std::unordered_set<std::string> variadic_funcs = {
            "printf", "fprintf", "sprintf", "snprintf", "scanf", "fscanf", "sscanf",
            "vprintf", "vfprintf", "vsprintf", "vsnprintf", "vscanf", "vfscanf", "vsscanf"};
        std::vector<std::string> out;
        out.reserve(lines.size());
        struct ValueInfo {
            std::string location;
            bool valid = true;
        };
        std::unordered_map<std::string, ValueInfo> reg_contents;
        std::unordered_map<std::string, ValueInfo> mem_contents;
        auto invalidate_tracking = [&]() {
            reg_contents.clear();
            mem_contents.clear();
        };
        auto clobber_reg = [&](const std::string &reg) {
            reg_contents.erase(reg);
            for (auto it = mem_contents.begin(); it != mem_contents.end();) {
                if (it->second.location == reg)
                    it = mem_contents.erase(it);
                else
                    ++it;
            }
        };
        for (size_t i = 0; i < lines.size(); ++i) {
            const std::string &line = lines[i];

            if (std::regex_match(line, re_xor_eax) && (i + 1 < lines.size())) {
                std::smatch m;
                if (std::regex_match(lines[i + 1], m, re_call_func)) {
                    std::string func_name = m[1].str();
                    if (variadic_funcs.find(func_name) == variadic_funcs.end()) {
                        continue;
                    }
                }
            }
            if (is_label(line) || std::regex_search(line, re_call_or_jmp)) {
                out.push_back(line);
                invalidate_tracking();
                continue;
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_modifies_reg)) {
                    clobber_reg(m[1].str());
                    out.push_back(line);
                    continue;
                }
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_mem_to_reg)) {
                    const std::string mem = m[1].str();
                    const std::string reg = m[2].str();

                    clobber_reg(reg);

                    auto it = mem_contents.find(mem);
                    if (it != mem_contents.end() && it->second.valid) {
                        const std::string &src_reg = it->second.location;
                        if (src_reg != reg)
                            out.push_back("\tmovq " + src_reg + ", " + reg);
                        reg_contents[reg] = {mem, true};
                        continue;
                    }

                    out.push_back(line);
                    reg_contents[reg] = {mem, true};
                    continue;
                }
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_reg_to_mem)) {
                    const std::string reg = m[1].str();
                    const std::string mem = m[2].str();
                    mem_contents[mem] = {reg, true};
                    out.push_back(line);
                    continue;
                }
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_mov_reg)) {
                    const std::string src = m[1].str();
                    const std::string dst = m[2].str();
                    if (src == dst)
                        continue;

                    clobber_reg(dst);

                    auto it_src = reg_contents.find(src);
                    if (it_src != reg_contents.end())
                        reg_contents[dst] = it_src->second;
                    else
                        reg_contents[dst] = {src, true};

                    out.push_back(line);
                    continue;
                }
            }

            invalidate_tracking();
            out.push_back(line);
        }

        return out;
    }

    static std::vector<std::string> x64_opt_core_lines(const std::vector<std::string> &lines) {
        std::regex re_mov_reg(R"(^\s*movq?\s+(%[a-z0-9]+)\s*,\s*(%[a-z0-9]+)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_mem_to_reg(R"(^\s*movq?\s+([A-Za-z0-9_]+(?:\(%rip\))?)\s*,\s*(%[a-z0-9]+)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_reg_to_mem(R"(^\s*movq?\s+(%[a-z0-9]+)\s*,\s*([A-Za-z0-9_]+(?:\(%rip\))?)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_modifies_reg(R"(^\s*(?:add|sub|mul|imul|div|idiv|and|or|xor|shl|shr|sal|sar|not|neg)[a-z]*\s+.*,\s*(%[a-z0-9]+)\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_call_or_jmp(R"(^\s*(?:call|jmp|je|jne|jz|jnz|jg|jge|jl|jle|ja|jae|jb|jbe)[a-z]*\s+)", std::regex::icase);

        std::regex re_xor_eax(R"(^\s*xorq?\s+%eax\s*,\s*%eax\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_call_func(R"(^\s*call\s+([_A-Za-z][_A-ZaZ0-9]*)\s*(?:[#;].*)?$)", std::regex::icase);
        static const std::unordered_set<std::string> variadic_funcs = {
            "printf", "fprintf", "sprintf", "snprintf", "scanf", "fscanf", "sscanf",
            "vprintf", "vfprintf", "vsprintf", "vsnprintf", "vscanf", "vfscanf", "vsscanf"};
        auto local_is_label = [](const std::string &s) {
            static const std::regex re(R"(^\s*(?:[A-Za-z_.$][\w.$]*|\d+)\s*:)");
            return std::regex_search(s, re);
        };
        auto has_call_or_ret = [](const std::string &s) {
            return s.find(" call ") != std::string::npos ||
                   s.find("\tcall ") != std::string::npos ||
                   s.find(" ret") != std::string::npos ||
                   s.find("\tret") != std::string::npos;
        };
        auto touches_rsp = [](const std::string &s) {
            return s.find("%rsp") != std::string::npos || s.find("(%rsp") != std::string::npos;
        };

        std::vector<std::string> out;
        out.reserve(lines.size());
        std::vector<int> frame_bytes;

        std::regex re_sub_rsp(R"(^\s*sub\s+\$([0-9]+)\s*,\s*%rsp\s*(?:[#;].*)?$)", std::regex::icase);
        std::regex re_add_rsp(R"(^\s*add\s+\$([0-9]+)\s*,\s*%rsp\s*(?:[#;].*)?$)", std::regex::icase);

        struct ValueInfo {
            std::string location;
            bool valid = true;
        };
        std::unordered_map<std::string, ValueInfo> reg_contents;
        std::unordered_map<std::string, ValueInfo> mem_contents;

        auto invalidate_tracking = [&]() {
            reg_contents.clear();
            mem_contents.clear();
        };

        auto clobber_reg = [&](const std::string &reg) {
            reg_contents.erase(reg);
            for (auto it = mem_contents.begin(); it != mem_contents.end();) {
                if (it->second.location == reg)
                    it = mem_contents.erase(it);
                else
                    ++it;
            }
        };

        for (size_t i = 0; i < lines.size(); ++i) {
            const std::string &line = lines[i];

            if (std::regex_match(line, re_xor_eax) && (i + 1 < lines.size())) {
                std::smatch m;
                if (std::regex_match(lines[i + 1], m, re_call_func)) {
                    std::string func_name = m[1].str();
                    if (!func_name.empty() && func_name[0] == '_') {
                        func_name = func_name.substr(1);
                    }
                    if (variadic_funcs.find(func_name) == variadic_funcs.end()) {
                        continue;
                    }
                }
            }
            if (local_is_label(line)) {
                out.push_back(line);
                invalidate_tracking();
                continue;
            }
            {
                std::smatch m;
                if (std::regex_match(line, m, re_sub_rsp)) {
                    frame_bytes.push_back(std::stoi(m[1].str()));
                    out.push_back(line);
                    continue;
                }
            }
            if (!frame_bytes.empty()) {
                out.push_back(line);
                std::smatch m;
                if (std::regex_match(line, m, re_add_rsp)) {
                    int n = std::stoi(m[1].str());
                    if (!frame_bytes.empty() && frame_bytes.back() == n)
                        frame_bytes.pop_back();
                }
                continue;
            }
            if (has_call_or_ret(line) || touches_rsp(line) || std::regex_search(line, re_call_or_jmp)) {
                out.push_back(line);
                invalidate_tracking();
                continue;
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_modifies_reg)) {
                    clobber_reg(m[1].str());
                    out.push_back(line);
                    continue;
                }
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_reg_to_mem)) {
                    const std::string reg = m[1].str();
                    const std::string mem = m[2].str();
                    mem_contents[mem] = {reg, true};
                    out.push_back(line);
                    continue;
                }
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_mov_reg)) {
                    const std::string src = m[1].str();
                    const std::string dst = m[2].str();
                    if (src == dst)
                        continue;

                    clobber_reg(dst);

                    auto it_src = reg_contents.find(src);
                    if (it_src != reg_contents.end())
                        reg_contents[dst] = it_src->second;
                    else
                        reg_contents[dst] = {src, true};

                    out.push_back(line);
                    continue;
                }
            }

            {
                std::smatch m;
                if (std::regex_match(line, m, re_mem_to_reg)) {
                    const std::string mem = m[1].str();
                    const std::string reg = m[2].str();

                    clobber_reg(reg);

                    auto it = mem_contents.find(mem);
                    if (it != mem_contents.end() && it->second.valid) {
                        const std::string &src_reg = it->second.location;
                        if (src_reg != reg)
                            out.push_back("\tmovq " + src_reg + ", " + reg);
                        reg_contents[reg] = {mem, true};
                        continue;
                    }

                    out.push_back(line);
                    reg_contents[reg] = {mem, true};
                    continue;
                }
            }

            invalidate_tracking();
            out.push_back(line);
        }

        return out;
    }

    static std::string darwin_prefix_calls(const std::string &line,
                                           const std::unordered_set<std::string> &macos_functions) {
        static const std::regex re_call(R"(\b(call|jmp)\s+([_A-Za-z][_A-Za-z0-9]*)\b)");
        std::string result;
        result.reserve(line.size() + 16);

        size_t last = 0;
        for (std::sregex_iterator it(line.begin(), line.end(), re_call), end; it != end; ++it) {
            const auto &m = *it;
            const auto pos = static_cast<size_t>(m.position());
            const auto len = static_cast<size_t>(m.length());
            const std::string fn = m[2].str();

            result.append(line, last, pos - last);

            std::string piece = m.str();
            if (fn == "main" || macos_functions.count(fn)) {
                const auto fnpos = piece.rfind(fn);
                if (fnpos != std::string::npos)
                    piece.replace(fnpos, fn.size(), "_" + fn);
            }
            result += piece;

            last = pos + len;
        }
        result.append(line, last, std::string::npos);
        return result;
    }

    static std::vector<std::string> opt_darwin_lines(const std::vector<std::string> &lines) {
        std::vector<std::string> out;
        out.reserve(lines.size());

        std::regex re_stdin_access(R"(movq?\s+stdin\(%rip\)\s*,\s*(%\w+))", std::regex::icase);
        std::regex re_stdout_access(R"(movq?\s+stdout\(%rip\)\s*,\s*(%\w+))", std::regex::icase);
        std::regex re_stderr_access(R"(movq?\s+stderr\(%rip\)\s*,\s*(%\w+))", std::regex::icase);
        std::regex re_global_main(R"(^\s*\.(?:global|globl)\s+main\s*$)");
        std::regex re_main_label(R"(^\s*main\s*:\s*$)");
        std::regex re_global_function(R"(^\s*\.(?:global|globl)\s+([_a-zA-Z][_a-zA-Z0-9]*)(?:\s|$))");
        std::regex re_function_label(R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*:\s*$)");

        std::unordered_set<std::string> macos_functions;

        for (const auto &raw : lines) {
            std::string line = raw;

            if (std::regex_search(line, re_global_main)) {
                out.push_back("\t.globl _main");
                continue;
            }
            if (std::regex_search(line, re_main_label)) {
                out.push_back("_main:");
                continue;
            }

            std::smatch m_func;
            if (std::regex_search(line, m_func, re_global_function)) {
                std::string fn = m_func[1].str();
                if (fn != "main") {
                    macos_functions.insert(fn);
                    out.push_back("\t.globl _" + fn);
                }
                continue;
            }
            if (std::regex_search(line, m_func, re_function_label)) {
                std::string fn = m_func[1].str();
                if (fn != "main" && macos_functions.count(fn)) {
                    out.push_back("_" + fn + ":");
                } else {
                    out.push_back(line);
                }
                continue;
            }

            std::smatch m;
            if (std::regex_search(line, m, re_stdin_access)) {
                std::string r = m[1].str();
                out.push_back("\tmovq ___stdinp@GOTPCREL(%rip), %rax");
                out.push_back("\tmovq (%rax), " + r);
                continue;
            }
            if (std::regex_search(line, m, re_stdout_access)) {
                std::string r = m[1].str();
                out.push_back("\tmovq ___stdoutp@GOTPCREL(%rip), %rax");
                out.push_back("\tmovq (%rax), " + r);
                continue;
            }
            if (std::regex_search(line, m, re_stderr_access)) {
                std::string r = m[1].str();
                out.push_back("\tmovq ___stderrp@GOTPCREL(%rip), %rax");
                out.push_back("\tmovq (%rax), " + r);
                continue;
            }

            std::regex re_single_stdout(R"(^\s*movq?\s+_stdout\(%rip\)\s*,\s*(%\w+)\s*(?:[#;].*)?$)", std::regex::icase);
            if (std::regex_search(line, m, re_single_stdout)) {
                std::string r = m[1].str();
                out.push_back("\tmovq ___stdoutp@GOTPCREL(%rip), %rax");
                out.push_back("\tmovq (%rax), " + r);
                continue;
            }
            std::regex re_single_stderr(R"(^\s*movq?\s+_stderr\(%rip\)\s*,\s*(%\w+)\s*(?:[#;].*)?$)", std::regex::icase);
            if (std::regex_search(line, m, re_single_stderr)) {
                std::string r = m[1].str();
                out.push_back("\tmovq ___stderrp@GOTPCREL(%rip), %rax");
                out.push_back("\tmovq (%rax), " + r);
                continue;
            }
            std::regex re_single_stdin(R"(^\s*movq?\s+_stdin\(%rip\)\s*,\s*(%\w+)\s*(?:[#;].*)?$)", std::regex::icase);
            if (std::regex_search(line, m, re_single_stdin)) {
                std::string r = m[1].str();
                out.push_back("\tmovq ___stdinp@GOTPCREL(%rip), %rax");
                out.push_back("\tmovq (%rax), " + r);
                continue;
            }

            line = darwin_prefix_calls(line, macos_functions);
            out.push_back(line);
        }

        return out;
    }

    static std::vector<std::string> opt_x64_windows_lines(const std::vector<std::string> &lines) {
        static const std::regex add_rx(R"(^\s*addq?\s+\$([0-9]+|0x[0-9a-fA-F]+)\s*,\s*%rsp\s*(?:#.*)?$)");
        static const std::regex sub_rx(R"(^\s*subq?\s+\$([0-9]+|0x[0-9a-fA-F]+)\s*,\s*%rsp\s*(?:#.*)?$)");

        auto parse_imm = [](const std::string &s) -> unsigned long long {
            return std::stoull(s, nullptr, 0);
        };

        std::vector<std::string> out;
        out.reserve(lines.size());

        for (size_t i = 0; i < lines.size();) {
            const std::string &a = lines[i];

            std::smatch ma, mb;
            bool a_is_add = std::regex_match(a, ma, add_rx);
            bool a_is_sub = !a_is_add && std::regex_match(a, ma, sub_rx);

            if ((a_is_add || a_is_sub) && (i + 1) < lines.size()) {
                const std::string &b = lines[i + 1];
                bool b_is_add = std::regex_match(b, mb, add_rx);
                bool b_is_sub = !b_is_add && std::regex_match(b, mb, sub_rx);

                if ((a_is_add && b_is_sub) || (a_is_sub && b_is_add)) {
                    unsigned long long ia = parse_imm(ma[1].str());
                    unsigned long long ib = parse_imm(mb[1].str());
                    if (ia == ib) {
                        i += 2;
                        continue;
                    }
                }
            }

            out.push_back(a);
            ++i;
        }

        return out;
    }

    static std::vector<std::string> opt_linux_lines(const std::vector<std::string> &lines) {
        return lines;
    }

    std::string Program::gen_optimize(const std::string &code, const Platform &platform_name) {
        std::vector<std::string> lines;
        {
            std::istringstream stream(code);
            std::string s;
            while (std::getline(stream, s))
                lines.push_back(s);
        }

        if (platform_name == Platform::WINX64) {
            auto core = x64_opt_core_lines(lines);
            auto win = opt_x64_windows_lines(core);
            return join_lines(win);
        } else {
            auto core = opt_core_lines(lines);
            auto os = (platform_name == Platform::DARWIN) ? opt_darwin_lines(core)
                                                          : opt_linux_lines(core);
            return join_lines(os);
        }
    }
} // namespace mxvm
