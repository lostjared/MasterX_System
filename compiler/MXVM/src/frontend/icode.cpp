/**
 * @file icode.cpp
 * @brief Pascal-to-MXVM code generator — CodeGenVisitor visit methods and assembly output
 * @author Jared Bruni
 */
#include "icode.hpp"
#include <algorithm>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace pascal {
    static inline std::string rtrim_comment(const std::string &line) {
        auto h = line.find('#');
        auto s = line.find("//");
        size_t cut = std::min(h == std::string::npos ? line.size() : h,
                              s == std::string::npos ? line.size() : s);
        std::string out = line.substr(0, cut);
        while (!out.empty() && (unsigned char)out.back() <= ' ')
            out.pop_back();
        return out;
    }
    static inline std::string trim(const std::string &s) {
        size_t a = 0, b = s.size();
        while (a < b && (unsigned char)s[a] <= ' ')
            ++a;
        while (b > a && (unsigned char)s[b - 1] <= ' ')
            --b;
        return s.substr(a, b - a);
    }
    static void locate_sections(const std::vector<std::string> &lines,
                                size_t &dataStart, size_t &dataEnd,
                                size_t &codeStart, size_t &codeEnd) {
        dataStart = dataEnd = codeStart = codeEnd = 0;
        for (size_t i = 0; i < lines.size(); ++i) {
            if (!dataStart && lines[i].find("section data {") != std::string::npos) {
                dataStart = i + 1;
                for (size_t j = dataStart; j < lines.size(); ++j) {
                    if (lines[j].find('}') != std::string::npos) {
                        dataEnd = j - 1;
                        break;
                    }
                }
            }
            if (!codeStart && lines[i].find("section code {") != std::string::npos) {
                codeStart = i + 1;
                for (size_t j = codeStart; j < lines.size(); ++j) {
                    if (lines[j].find('}') != std::string::npos) {
                        codeEnd = j - 1;
                        break;
                    }
                }
            }
            if (dataStart && dataEnd && codeStart && codeEnd)
                break;
        }
    }
    static std::unordered_set<std::string> collect_used(const std::vector<std::string> &lines,
                                                        size_t codeStart, size_t codeEnd) {
        std::string code;
        for (size_t i = codeStart; i <= codeEnd && i < lines.size(); ++i) {
            code += lines[i];
            code.push_back('\n');
        }
        std::unordered_set<std::string> used = {"rax", "fmt_int", "fmt_str", "fmt_chr", "fmt_float", "newline"};
        std::regex varUsagePattern("\\b([a-zA-Z_][a-zA-Z0-9_]*)\\b");
        for (auto it = std::sregex_iterator(code.begin(), code.end(), varUsagePattern);
             it != std::sregex_iterator(); ++it)
            used.insert((*it).str(1));
        return used;
    }
    static void sweep_unused_data(std::vector<std::string> &lines) {
        size_t dS = 0, dE = 0, cS = 0, cE = 0;
        locate_sections(lines, dS, dE, cS, cE);
        if (!dS || !cS)
            return;
        auto used = collect_used(lines, cS, cE);
        std::regex decl("^\\s*(export)?\\s*(int|string|ptr|float|double)\\s+([A-Za-z_][A-Za-z0-9_]*)");
        std::vector<size_t> erase;
        for (size_t i = dS; i <= dE && i < lines.size(); ++i) {
            std::smatch m;
            if (std::regex_search(lines[i], m, decl)) {
                bool isExport = m[1].length() > 0;
                std::string name = m[3];
                if (!isExport && !used.count(name))
                    erase.push_back(i);
            }
        }
        std::sort(erase.begin(), erase.end(), std::greater<size_t>());
        for (auto idx : erase)
            if (idx < lines.size())
                lines.erase(lines.begin() + idx);
    }

    static std::vector<std::pair<size_t, std::string>> nextNonEmpty(
        const std::vector<std::string> &code, size_t start, size_t n) {
        std::vector<std::pair<size_t, std::string>> out;
        for (size_t i = start; i < code.size() && out.size() < n; ++i) {
            std::string t = trim(rtrim_comment(code[i]));
            if (!t.empty())
                out.push_back({i, t});
        }
        return out;
    }

    static std::string invertJump(const std::string &j) {
        if (j == "jg")
            return "jle";
        else if (j == "jle")
            return "jg";
        else if (j == "jl")
            return "jge";
        else if (j == "jge")
            return "jl";
        else if (j == "je")
            return "jne";
        else if (j == "jne")
            return "je";
        else if (j == "ja")
            return "jbe";
        else if (j == "jbe")
            return "ja";
        else if (j == "jb")
            return "jae";
        else if (j == "jae")
            return "jb";
        return "";
    }

    static bool parse2(const std::string &line, const std::string &op,
                       std::string &a, std::string &b) {
        if (line.size() <= op.size() || line.compare(0, op.size(), op) != 0 ||
            line[op.size()] != ' ')
            return false;
        std::string rest = line.substr(op.size() + 1);
        auto c = rest.find(',');
        if (c == std::string::npos)
            return false;
        a = trim(rest.substr(0, c));
        b = trim(rest.substr(c + 1));
        return !a.empty() && !b.empty();
    }
    static bool parse1(const std::string &line, const std::string &op, std::string &a) {
        if (line.size() <= op.size() || line.compare(0, op.size(), op) != 0 ||
            line[op.size()] != ' ')
            return false;
        a = trim(line.substr(op.size() + 1));
        return !a.empty();
    }
    static bool parseJump(const std::string &line, std::string &jop, std::string &label) {
        if (line.size() < 3 || line[0] != 'j')
            return false;
        auto sp = line.find(' ');
        if (sp == std::string::npos)
            return false;
        jop = line.substr(0, sp);
        label = trim(line.substr(sp + 1));
        return !label.empty() && jop != "jmp";
    }
    static bool isLabelDef(const std::string &line, const std::string &label) {
        return line == label + ":";
    }

    static void foldCmpTest(std::vector<std::string> &code) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (size_t i = 0; i < code.size(); ++i) {
                auto mat = nextNonEmpty(code, i, 7);
                if (mat.size() < 7)
                    break;

                std::string cmpA, cmpB;
                bool isFcmp = parse2(mat[0].second, "fcmp", cmpA, cmpB);
                if (!isFcmp && !parse2(mat[0].second, "cmp", cmpA, cmpB))
                    continue;
                std::string cmpOp = isFcmp ? "fcmp" : "cmp";

                std::string jCC, trueLabel;
                if (!parseJump(mat[1].second, jCC, trueLabel))
                    continue;

                std::string reg, zero;
                if (!parse2(mat[2].second, "mov", reg, zero) || zero != "0")
                    continue;

                std::string endLabel;
                if (!parse1(mat[3].second, "jmp", endLabel))
                    continue;

                if (!isLabelDef(mat[4].second, trueLabel))
                    continue;

                std::string r5, one;
                if (!parse2(mat[5].second, "mov", r5, one) || r5 != reg || one != "1")
                    continue;

                if (!isLabelDef(mat[6].second, endLabel))
                    continue;

                auto rest = nextNonEmpty(code, mat[6].first + 1, 30);
                bool found = false;
                for (size_t j = 0; j + 1 < rest.size(); ++j) {
                    if (rest[j].second.back() == ':')
                        break;
                    std::string wdst, wsrc;
                    if (parse2(rest[j].second, "mov", wdst, wsrc) && wdst == reg)
                        break;

                    std::string rt, zt;
                    if (parse2(rest[j].second, "cmp", rt, zt) && rt == reg && zt == "0") {
                        std::string jTest, target;
                        if (j + 1 < rest.size() && parseJump(rest[j + 1].second, jTest, target) &&
                            (jTest == "je" || jTest == "jne")) {
                            std::string newJ = (jTest == "je") ? invertJump(jCC) : jCC;
                            if (newJ.empty())
                                break;

                            bool labelUsedElsewhere = false;
                            for (size_t k = 0; k < code.size(); ++k) {
                                if (k == mat[1].first || k == rest[j + 1].first)
                                    continue;
                                std::string t = trim(rtrim_comment(code[k]));
                                if (!t.empty() && t.back() != ':' && t.find(trueLabel) != std::string::npos) {
                                    labelUsedElsewhere = true;
                                    break;
                                }
                            }
                            if (labelUsedElsewhere)
                                break;

                            code[mat[0].first] = "\t\t" + cmpOp + " " + cmpA + ", " + cmpB;
                            code[mat[1].first] = "\t\t" + newJ + " " + target;
                            for (int k = 2; k <= 6; ++k)
                                code[mat[k].first] = "";
                            code[rest[j].first] = "";
                            code[rest[j + 1].first] = "";

                            found = true;
                            changed = true;
                        }
                        break;
                    }
                }
                if (found)
                    break;
            }
        }
    }

    static void foldAndTest(std::vector<std::string> &code) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (size_t i = 0; i < code.size(); ++i) {
                auto seq = nextNonEmpty(code, i, 11);
                if (seq.size() < 11)
                    break;

                std::string r1, z0;
                if (!parse2(seq[0].second, "cmp", r1, z0) || z0 != "0")
                    continue;
                std::string j1, zeroLabel;
                if (!parse1(seq[1].second, "je", zeroLabel))
                    continue;
                std::string r2, z2;
                if (!parse2(seq[2].second, "cmp", r2, z2) || z2 != "0")
                    continue;
                std::string j3, zl3;
                if (!parse1(seq[3].second, "je", zl3) || zl3 != zeroLabel)
                    continue;
                std::string r3, one;
                if (!parse2(seq[4].second, "mov", r3, one) || one != "1")
                    continue;
                std::string endLabel;
                if (!parse1(seq[5].second, "jmp", endLabel))
                    continue;
                if (!isLabelDef(seq[6].second, zeroLabel))
                    continue;
                std::string r7, z7;
                if (!parse2(seq[7].second, "mov", r7, z7) || r7 != r3 || z7 != "0")
                    continue;
                if (!isLabelDef(seq[8].second, endLabel))
                    continue;
                std::string r9, z9;
                if (!parse2(seq[9].second, "cmp", r9, z9) || r9 != r3 || z9 != "0")
                    continue;
                std::string target;
                if (!parse1(seq[10].second, "je", target))
                    continue;

                code[seq[0].first] = "\t\tcmp " + r1 + ", 0";
                code[seq[1].first] = "\t\tje " + target;
                code[seq[2].first] = "\t\tcmp " + r2 + ", 0";
                code[seq[3].first] = "\t\tje " + target;
                for (int k = 4; k <= 10; ++k)
                    code[seq[k].first] = "";
                changed = true;
                break;
            }
        }
    }

    static void foldOrTest(std::vector<std::string> &code) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (size_t i = 0; i < code.size(); ++i) {
                auto seq = nextNonEmpty(code, i, 11);
                if (seq.size() < 11)
                    break;

                std::string r1, z0;
                if (!parse2(seq[0].second, "cmp", r1, z0) || z0 != "0")
                    continue;
                std::string oneLabel;
                if (!parse1(seq[1].second, "jne", oneLabel))
                    continue;
                std::string r2, z2;
                if (!parse2(seq[2].second, "cmp", r2, z2) || z2 != "0")
                    continue;
                std::string ol3;
                if (!parse1(seq[3].second, "jne", ol3) || ol3 != oneLabel)
                    continue;
                std::string r3, z4;
                if (!parse2(seq[4].second, "mov", r3, z4) || z4 != "0")
                    continue;
                std::string endLabel;
                if (!parse1(seq[5].second, "jmp", endLabel))
                    continue;
                if (!isLabelDef(seq[6].second, oneLabel))
                    continue;
                std::string r7, o7;
                if (!parse2(seq[7].second, "mov", r7, o7) || r7 != r3 || o7 != "1")
                    continue;
                if (!isLabelDef(seq[8].second, endLabel))
                    continue;
                std::string r9, z9;
                if (!parse2(seq[9].second, "cmp", r9, z9) || r9 != r3 || z9 != "0")
                    continue;
                std::string target;
                if (!parse1(seq[10].second, "je", target))
                    continue;

                code[seq[0].first] = "\t\tcmp " + r1 + ", 0";
                code[seq[1].first] = "\t\tjne " + oneLabel;
                code[seq[2].first] = "\t\tcmp " + r2 + ", 0";
                code[seq[3].first] = "\t\tje " + target;
                code[seq[4].first] = "\t" + oneLabel + ":";
                for (int k = 5; k <= 10; ++k)
                    code[seq[k].first] = "";
                changed = true;
                break;
            }
        }
    }

    /**
     * @brief Forward declaration — checks whether a name is a hardware register.
     *
     * Used by the peephole optimizer to distinguish register temporaries from
     * global/local variable names when deciding whether a store is dead.
     */
    static bool isRegisterName(const std::string &s);

    /**
     * @brief Copy-propagation peephole pass.
     *
     * Folds sequences like `mov reg, src ; op reg, rhs` into `op src, rhs`
     * when the intermediate register is not referenced again.  The scan
     * respects `ret` / `done` boundaries so that stores to global
     * variables visible to the caller are never eliminated.
     */
    static void copyPropagation(std::vector<std::string> &code) {
        static const std::unordered_set<std::string> twoOpInstructions = {
            "add", "sub", "mul", "div", "mod", "cmp", "fcmp", "mov", "and", "or", "xor"};
        bool changed = true;
        while (changed) {
            changed = false;
            for (size_t i = 0; i < code.size(); ++i) {
                std::string t0 = trim(rtrim_comment(code[i]));
                if (t0.empty())
                    continue;

                std::string movDst, movSrc;
                if (!parse2(t0, "mov", movDst, movSrc))
                    continue;
                if (movDst == movSrc) {
                    code[i] = "";
                    changed = true;
                    break;
                }

                size_t j = i + 1;
                while (j < code.size() && trim(rtrim_comment(code[j])).empty())
                    ++j;
                if (j >= code.size())
                    continue;

                std::string t1 = trim(rtrim_comment(code[j]));
                if (!t1.empty() && t1.back() == ':')
                    continue;

                auto sp = t1.find(' ');
                if (sp == std::string::npos)
                    continue;
                std::string op = t1.substr(0, sp);

                if (twoOpInstructions.find(op) == twoOpInstructions.end())
                    continue;

                std::string arg1, arg2;
                if (!parse2(t1, op, arg1, arg2))
                    continue;
                if (arg1 != movDst)
                    continue;
                if (arg2 == movDst)
                    continue;

                bool usedLater = false;
                std::string pattern = "\\b" + movDst + "\\b";
                std::regex wordPat(pattern);
                for (size_t k = j + 1; k < code.size(); ++k) {
                    std::string tk = rtrim_comment(code[k]);
                    if (tk.empty())
                        continue;
                    if (std::regex_search(tk, wordPat)) {
                        usedLater = true;
                        break;
                    }
                    if (!tk.empty() && tk.find("function ") != std::string::npos)
                        break;
                    // A ret/done ends the current procedure/program.
                    // Non-register stores before a return are observable
                    // by the caller, so treat them as live.
                    std::string ttk = trim(tk);
                    if ((ttk == "ret" || ttk == "done") && !isRegisterName(movDst)) {
                        usedLater = true;
                        break;
                    }
                }

                if (usedLater)
                    continue;

                if (op == "cmp" || op == "fcmp") {
                    code[j] = "\t\t" + op + " " + movSrc + ", " + arg2;
                    code[i] = "";
                    changed = true;
                    break;
                }

                if (op == "mov") {
                    code[i] = "";
                    changed = true;
                    break;
                }

                // Arithmetic ops require a variable as operand 1 (it's the destination).
                // Don't propagate constants into that position.
                if (!movSrc.empty() && (std::isdigit((unsigned char)movSrc[0]) || movSrc[0] == '-' || movSrc[0] == '"' || movSrc[0] == '\''))
                    continue;

                bool srcUsedLater = false;
                std::string srcPattern = "\\b" + movSrc + "\\b";
                std::regex srcPat(srcPattern);
                for (size_t k = j + 1; k < code.size(); ++k) {
                    std::string tk = rtrim_comment(code[k]);
                    if (tk.empty())
                        continue;
                    if (std::regex_search(tk, srcPat)) {
                        srcUsedLater = true;
                        break;
                    }
                    if (!tk.empty() && tk.find("function ") != std::string::npos)
                        break;
                }

                if (!srcUsedLater) {
                    code[j] = "\t\t" + op + " " + movSrc + ", " + arg2;
                    code[i] = "";
                    changed = true;
                    break;
                }
            }
        }
    }

    static bool isRegisterName(const std::string &s) {
        static const std::unordered_set<std::string> regs = {
            "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
        if (regs.count(s))
            return true;
        if (s.size() >= 4 && s.compare(0, 3, "xmm") == 0)
            return true;
        return false;
    }

    static void foldMovArithMov(std::vector<std::string> &code) {
        static const std::unordered_set<std::string> arithOps = {
            "add", "sub", "mul", "div", "mod", "and", "or", "xor"};
        bool changed = true;
        while (changed) {
            changed = false;
            for (size_t i = 0; i + 2 < code.size(); ++i) {
                std::string t0 = trim(rtrim_comment(code[i]));
                if (t0.empty())
                    continue;
                std::string mDst, mSrc;
                if (!parse2(t0, "mov", mDst, mSrc))
                    continue;
                if (!isRegisterName(mDst))
                    continue;

                size_t j = i + 1;
                while (j < code.size() && trim(rtrim_comment(code[j])).empty())
                    ++j;
                if (j >= code.size())
                    continue;
                std::string t1 = trim(rtrim_comment(code[j]));
                if (t1.empty() || t1.back() == ':')
                    continue;
                auto sp = t1.find(' ');
                if (sp == std::string::npos)
                    continue;
                std::string op = t1.substr(0, sp);
                if (arithOps.find(op) == arithOps.end())
                    continue;
                std::string a1, a2;
                if (!parse2(t1, op, a1, a2))
                    continue;
                if (a1 != mDst)
                    continue;
                if (a2 == mDst)
                    continue;

                size_t k = j + 1;
                while (k < code.size() && trim(rtrim_comment(code[k])).empty())
                    ++k;
                if (k >= code.size())
                    continue;
                std::string t2 = trim(rtrim_comment(code[k]));
                std::string m2Dst, m2Src;
                if (!parse2(t2, "mov", m2Dst, m2Src))
                    continue;
                if (m2Src != mDst)
                    continue;
                if (isRegisterName(m2Dst))
                    continue;

                bool usedAfter = false;
                std::string pattern = "\\b" + mDst + "\\b";
                std::regex wordPat(pattern);
                for (size_t w = k + 1; w < code.size(); ++w) {
                    std::string tw = rtrim_comment(code[w]);
                    if (tw.empty())
                        continue;
                    if (std::regex_search(tw, wordPat)) {
                        usedAfter = true;
                        break;
                    }
                    if (tw.find("function ") != std::string::npos)
                        break;
                }
                if (usedAfter)
                    continue;

                code[i] = "\t\tmov " + m2Dst + ", " + mSrc;
                code[j] = "\t\t" + op + " " + m2Dst + ", " + a2;
                code[k] = "";
                changed = true;
                break;
            }
        }
    }

    static void peepholeOpt(std::vector<std::string> &code) {
        copyPropagation(code);
        foldMovArithMov(code);
        foldAndTest(code);
        foldOrTest(code);
        foldCmpTest(code);
        foldCmpTest(code);
        copyPropagation(code);
    }

    std::string mxvmOpt(const std::string &text) {
        std::vector<std::string> lines;
        {
            std::istringstream is(text);
            std::string ln;
            while (std::getline(is, ln))
                lines.push_back(ln);
        }

        size_t dataStart = 0, dataEnd = 0, codeStart = 0, codeEnd = 0;
        locate_sections(lines, dataStart, dataEnd, codeStart, codeEnd);
        if (!codeStart) {
            std::string out;
            for (auto &ln : lines)
                out += ln + "\n";
            return out;
        }

        sweep_unused_data(lines);
        locate_sections(lines, dataStart, dataEnd, codeStart, codeEnd);
        if (!codeStart) {
            std::string out;
            for (auto &ln : lines)
                out += ln + "\n";
            return out;
        }

        size_t cStart = codeStart, cEnd = std::min(codeEnd, lines.size() ? lines.size() - 1 : 0);
        std::vector<std::string> code;
        code.reserve(cEnd - cStart + 1);
        for (size_t i = cStart; i <= cEnd; ++i)
            code.push_back(lines[i]);

        std::vector<std::string> pass1;
        pass1.reserve(code.size());
        std::regex movPat("^\\s*(\\s*)mov\\s+([^,\\s]+)\\s*,\\s*([^\\s#;]+)\\s*(?:[;#].*)?$", std::regex::icase);

        for (size_t i = 0; i < code.size();) {
            std::string raw0 = code[i];
            std::string noCom0 = rtrim_comment(raw0);
            std::smatch m0;
            if (std::regex_match(noCom0, m0, movPat)) {
                std::string indent = m0[1].str();
                std::string dst0 = trim(m0[2].str());
                std::string src0 = trim(m0[3].str());
                if (dst0 == src0) {
                    ++i;
                    continue;
                }
                size_t k = i + 1;
                while (k < code.size() && rtrim_comment(code[k]).empty())
                    ++k;
                if (k < code.size()) {
                    std::string raw1 = code[k];
                    std::string noCom1 = rtrim_comment(raw1);
                    std::smatch m1;
                    if (std::regex_match(noCom1, m1, movPat)) {
                        std::string dst1 = trim(m1[2].str());
                        std::string src1 = trim(m1[3].str());
                        if (dst1 == src0 && src1 == dst0) {
                            pass1.push_back("\t\tmov " + dst0 + ", " + src0);
                            i = k + 1;
                            continue;
                        }
                    }
                }
                pass1.push_back(raw0);
                ++i;
                continue;
            }
            pass1.push_back(raw0);
            ++i;
        }

        std::vector<std::string> pass2;
        pass2.reserve(pass1.size());
        std::regex movDstPat("^\\s*mov\\s+([A-Za-z_][A-Za-z0-9_]*)\\s*,", std::regex::icase);

        for (size_t i = 0; i < pass1.size(); ++i) {
            std::string raw = pass1[i];
            std::string noCom = rtrim_comment(raw);
            std::smatch mm;
            if (std::regex_match(noCom, mm, movDstPat)) {
                std::string dst = mm[1].str();
                bool usedLater = false;
                std::regex word("\\b" + dst + "\\b");
                for (size_t j = i + 1; j < pass1.size(); ++j) {
                    std::string nxt = rtrim_comment(pass1[j]);
                    if (!nxt.empty() && std::regex_search(nxt, word)) {
                        usedLater = true;
                        break;
                    }
                    // A ret/done ends the current procedure/program.
                    // Non-register stores before a return are observable
                    // by the caller, so treat them as live.
                    if (!nxt.empty()) {
                        std::string tnxt = trim(nxt);
                        if ((tnxt == "ret" || tnxt == "done") && !isRegisterName(dst)) {
                            usedLater = true;
                            break;
                        }
                    }
                }
                if (!usedLater)
                    continue;
            }
            pass2.push_back(raw);
        }

        peepholeOpt(pass2);

        std::vector<std::string> pass3;
        pass3.reserve(pass2.size());
        for (auto &ln : pass2)
            if (!ln.empty())
                pass3.push_back(ln);

        std::vector<std::string> finalLines;
        finalLines.insert(finalLines.end(), lines.begin(), lines.begin() + cStart);
        finalLines.insert(finalLines.end(), pass3.begin(), pass3.end());
        if (cEnd + 1 < lines.size())
            finalLines.insert(finalLines.end(), lines.begin() + cEnd + 1, lines.end());

        sweep_unused_data(finalLines);

        std::string result;
        for (auto &ln : finalLines)
            result += ln + "\n";
        return result;
    }
} // namespace pascal
