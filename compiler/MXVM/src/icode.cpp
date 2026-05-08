/**
 * @file icode.cpp
 * @brief Base and Program class core methods — instruction storage, variable management, and standard library registration
 * @author Jared Bruni
 */
#include "mxvm/icode.hpp"
#include "mxvm/parser.hpp"
#include "scanner/exception.hpp"
#include <algorithm>
#include <cstdint>
#include <dlfcn.h>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_set>

namespace mxvm {

    std::unordered_map<std::string, Program *> Base::object_map;

    std::string Program::getMangledName(const std::string &var) {
        auto dot_pos = var.find('.');
        if (dot_pos != std::string::npos) {
            std::string sym = var;
            std::replace(sym.begin(), sym.end(), '.', '_');
            if (platform == Platform::DARWIN && !sym.empty() && sym[0] != '_') {
                if (var == "stdout" || var == "stdin" || var == "stderr") {
                    return var;
                }
                return sym;
            } else {
                return sym;
            }
        }

        if (!isVariable(var))
            return var;
        Variable &v = getVariable(var);
        if (!v.obj_name.empty()) {
            if (platform == Platform::DARWIN) {
                return "_" + v.obj_name + "_" + var;
            } else {
                return v.obj_name + "_" + var;
            }
        }

        if (platform == Platform::DARWIN && !var.empty() && var[0] != '-') {
            return "_" + var;
        } else {
            return var;
        }
    }

    std::string Program::getMangledName(const Operand &op) {
        if (op.op.find('.') == std::string::npos) {
            auto it = labels.find(op.op);
            if (it != labels.end()) {
                return name + "_" + op.op;
            }
        }
        if (!op.object.empty() && !op.label.empty()) {
            if (op.op == "stdout" || op.op == "stderr" || op.op == "stdin") {
                return op.op;
            }
            std::string base = op.object + "_" + op.label;
            return (platform == Platform::DARWIN) ? ("_" + base) : base;
        }
        return getMangledName(op.op);
    }

    Base::Base(Base &&other) noexcept
        : name(std::move(other.name)), inc(std::move(other.inc)), vars(std::move(other.vars)), labels(std::move(other.labels)), external(std::move(other.external)), external_functions(std::move(other.external_functions)) {
    }

    Base &Base::operator=(Base &&other) noexcept {
        if (this != &other) {
            name = std::move(other.name);
            inc = std::move(other.inc);
            vars = std::move(other.vars);
            labels = std::move(other.labels);
            external = std::move(other.external);
            external_functions = std::move(other.external_functions);
        }
        return *this;
    }

    std::unordered_map<std::string, Variable> Base::allocated;

    void Base::add_allocated(const std::string &name, Variable &v) {
        auto it = allocated.find(name);
        if (it == allocated.end())
            allocated[name] = v;
    }

    Program::Program() : pc(0), running(false) {

        add_extern("main", "strlen", true);
        add_extern("main", "printf", true);
        add_extern("main", "calloc", true);
        add_extern("main", "free", true);
        add_extern("main", "exit", true);
        add_extern("main", "atol", true);
        add_extern("main", "atof", true);
    }

    void Program::add_standard() {
        Variable vstdout, vstdin, vstderr;
#ifndef __APPLE__
        vstdout.setExtern("stdout", stdout);
        vstdin.setExtern("stdin", stdin);
        vstderr.setExtern("stderr", stderr);
        add_variable(name + "." + std::string("stdout"), vstdout);
        add_variable(name + "." + std::string("stdin"), vstdin);
        add_variable(name + "." + std::string("stderr"), vstderr);
#else

        vstdout.setExtern("stdout", stdout);
        vstdin.setExtern("stdin", stdin);
        vstderr.setExtern("stderr", stderr);
        add_variable(name + "." + std::string("stdout"), vstdout);
        add_variable(name + "." + std::string("stdin"), vstdin);
        add_variable(name + "." + std::string("stderr"), vstderr);

#endif
    }

    void Program::setArgs(const std::vector<std::string> &argv) {
        std::copy(argv.begin(), argv.end(), std::back_inserter(args));
    }

    void Program::setObject(bool obj) {
        object = obj;
    }

    void Base::add_object(const std::string &name, Program *prog) {
        if (base != nullptr) {
            auto it = base->object_map.find(name);
            if (it == base->object_map.end())
                base->object_map[name] = prog;
        }
    }

    Program::~Program() {
        std::unordered_set<void *> freed_ptrs;
        for (auto &i : vars) {
            if (i.second.var_value.ptr_value != nullptr && i.second.var_value.owns) {
                if (freed_ptrs.find(i.second.var_value.ptr_value) == freed_ptrs.end()) {
                    freed_ptrs.insert(i.second.var_value.ptr_value);
                    free(i.second.var_value.ptr_value);
                    if (debug_mode) {
                        std::cerr << Col("MXVM: Warning ", mx::Color::RED) << "Possible Memory Leak, Pointer: " << name << "." << i.first << "\n";
                    }
                }
                i.second.var_value.ptr_value = nullptr;
                i.second.var_value.owns = false;
            }
        }

        for (auto &h : RuntimeFunction::handles) {
            if (h.second) {
                dlclose(h.second);
                h.second = nullptr;
                char *errf = dlerror();
                if (errf != nullptr) {
                    std::cerr << "Error releaseing module: " << errf << "\n";
                } else {
                    if (mxvm::debug_mode) {
                        std::cout << "Released module: " << h.first << "\n";
                    }
                }
            }
        }
    }
    bool Program::isFunctionValid(const std::string &label) {
        auto dot_pos = label.find('.');
        if (dot_pos != std::string::npos) {
            std::string obj_name = label.substr(0, dot_pos);
            std::string func_name = label.substr(dot_pos + 1);

            auto it = object_map.find(obj_name);
            if (it != object_map.end() && it->second != nullptr) {
                Program *obj_prog = it->second;
                auto label_it = obj_prog->labels.find(func_name);
                return label_it != obj_prog->labels.end() && label_it->second.second; // second==true means function
            }
            return false;
        }

        auto it = labels.find(label);
        if (it != labels.end() && it->second.second) {
            return true;
        }

        return false;
    }

    std::unordered_map<std::string, void *> RuntimeFunction::handles;
    std::string Base::root_name;

    RuntimeFunction::RuntimeFunction(const std::string &mod, const std::string &name) {
        fname = name;
        handle = nullptr;
        if (handles.find(mod) == handles.end()) {
            handle = dlopen(mod.c_str(), RTLD_LAZY);
            handles[mod] = handle;
        } else {
            handle = handles[mod];
        }
        if (handle == nullptr) {
            throw mx::Exception("Error could not open module: " + mod + " try using --path to point to module path");
        }
        func = (void *)dlsym(handle, name.c_str());
        if (func == nullptr) {
            char *dl_e = dlerror();
            throw mx::Exception("Error could not find symbol: " + name + " in: " + mod + " Error: " + ((dl_e != nullptr) ? dl_e : ""));
        }
        char *dl_err = dlerror();
        if (dl_err != nullptr) {
            throw mx::Exception("Error: " + std::string(dl_err));
        }
    }

    void RuntimeFunction::call(Program *program, std::vector<Operand> &operands) {
        if (!func || !handle) {
            throw mx::Exception("RuntimeFunction: function: " + this->fname + " pointer is null: " + this->mod_name);
        }
        using FuncType = void (*)(Program *program, std::vector<Operand> &);
        FuncType f = reinterpret_cast<FuncType>(func);
        if (f != nullptr)
            f(program, operands);
    }

    Base *Base::base = nullptr;
    std::vector<std::string> Base::filenames;

    void Base::add_instruction(const Instruction &i) {
        inc.push_back(i);
    }

    void Base::add_label(const std::string &name, uint64_t address, bool f) {
        if (labels.find(name) == labels.end())
            labels[name] = std::make_pair(address, f);
    }

    void Base::add_variable(const std::string &name, const Variable &v) {
        if (vars.find(name) == vars.end()) {
            vars[name] = v;
        } else {
            throw mx::Exception("Duplciate variable name: " + name);
        }
    }

    void Base::add_global(const std::string &objname, const std::string &name, const Variable &v) {
    }

    void Base::add_filename(const std::string &fname) {
        if (base != nullptr) {
            auto it = std::find(base->filenames.begin(), base->filenames.end(), fname);
            if (it == base->filenames.end()) {
                base->filenames.push_back(fname);
            }
        }
    }

    void Base::add_extern(const std::string &mod, const std::string &name, bool module) {
        ExternalFunction f = {name, mod, module};
        auto it = std::find(external.begin(), external.end(), f);
        if (it == external.end())
            external.push_back({name, mod, module});
    }

    void Base::add_runtime_extern(const std::string &mod_name, const std::string &mod, const std::string &func_name, const std::string &name) {
        if (mod_name.empty() || mod.empty() || func_name.empty() || name.empty()) {
            throw mx::Exception("External function missing information.");
        }

        if (base != nullptr) {
            if (base->external_functions.find(name) == base->external_functions.end()) {
                base->external_functions[name] = RuntimeFunction(mod, func_name);
                base->external_functions[name].mod_name = mod_name;
            }
        }
    }
    Program *Program::getObjectByName(const std::string &searchName) {
        if (this->name == searchName)
            return this;
        for (auto &obj : objects) {
            Program *found = obj->getObjectByName(searchName);
            if (found)
                return found;
        }
        return nullptr;
    }

    std::string Program::escapeNewLines(const std::string &input) {
        std::string result;
        for (char c : input) {
            switch (c) {
            case '\t':
                result += "\\t";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            default:
                result += c;
                break;
            }
        }
        return result;
    }
    void Program::memoryDump(std::ostream &out) {
        out << "Current working directory: ";
        try {
            out << std::filesystem::current_path().string() << "\n";
        } catch (const std::exception &e) {
            out << "(unable to retrieve: " << e.what() << ")\n";
        }

        std::function<void(Program *, std::ostream &, int)> dumpProgram;
        dumpProgram = [&](Program *prog, std::ostream &out, int depth = 0) {
            std::string indent(depth * 2, ' ');
            out << indent << "=== MEMORY DUMP for " << prog->name << " ===\n";
            if (!prog->labels.empty()) {
                out << indent << "  Labels:\n";
                out << indent << "    " << std::left << std::setw(20) << "Name"
                    << std::setw(15) << "Address"
                    << std::setw(12) << "IsFunc"
                    << "\n";
                out << indent << "    " << std::string(42, '-') << "\n";
                for (const auto &label : prog->labels) {
                    out << indent << "    " << std::setw(20) << label.first
                        << std::setw(15) << label.second.first
                        << std::setw(10) << (label.second.second ? "yes" : "no")
                        << "\n";
                }
            }
            if (prog->vars.empty()) {
                out << indent << "  (no variables)\n";
            } else {
                out << indent << std::left << std::setw(25) << "Name"
                    << std::setw(18) << "Type"
                    << std::setw(30) << "Value"
                    << std::setw(18) << "PtrSize"
                    << std::setw(18) << "PtrCount"
                    << std::setw(12) << "Owns"
                    << "\n";
                out << indent << std::string(121, '-') << "\n";
                for (const auto &var : prog->vars) {
                    out << indent << std::setw(25) << var.first;
                    std::string typeStr;
                    switch (var.second.type) {
                    case VarType::VAR_INTEGER:
                        typeStr = "int";
                        break;
                    case VarType::VAR_FLOAT:
                        typeStr = "float";
                        break;
                    case VarType::VAR_STRING:
                        typeStr = "string";
                        break;
                    case VarType::VAR_POINTER:
                        typeStr = "ptr";
                        break;
                    case VarType::VAR_LABEL:
                        typeStr = "label";
                        break;
                    case VarType::VAR_EXTERN:
                        typeStr = "external";
                        break;
                    case VarType::VAR_ARRAY:
                        typeStr = "array";
                        break;
                    case VarType::VAR_BYTE:
                        typeStr = "byte";
                        break;
                    default:
                        typeStr = "unknown";
                        break;
                    }
                    out << std::setw(18) << typeStr;
                    switch (var.second.type) {
                    case VarType::VAR_INTEGER:
                    case VarType::VAR_BYTE:
                        out << std::setw(30) << var.second.var_value.int_value;
                        break;
                    case VarType::VAR_FLOAT:
                        out << std::setw(30) << std::fixed << std::setprecision(6)
                            << var.second.var_value.float_value;
                        break;
                    case VarType::VAR_STRING:
                        out << std::setw(30) << ("\"" + Program::escapeNewLines(var.second.var_value.str_value) + "\"");
                        break;
                    case VarType::VAR_POINTER:
                    case VarType::VAR_EXTERN:
                        if (var.second.var_value.ptr_value == nullptr)
                            out << std::setw(30) << "null";
                        else {
                            std::ostringstream ptr_oss;
                            ptr_oss << var.second.var_value.ptr_value;
                            out << std::setw(30) << ptr_oss.str() << std::dec;
                        }
                        break;
                    case VarType::VAR_LABEL:
                        out << std::setw(30) << var.second.var_value.label_value;
                        break;
                    default:
                        out << std::setw(30) << var.second.var_value.int_value;
                        break;
                    }
                    if (var.second.type == VarType::VAR_POINTER) {
                        out << std::setw(18) << var.second.var_value.ptr_size
                            << std::setw(18) << var.second.var_value.ptr_count
                            << std::setw(12) << (var.second.var_value.owns ? "yes" : "no");
                    } else {
                        out << std::setw(18) << "-"
                            << std::setw(18) << "-"
                            << std::setw(12) << "-";
                    }
                    out << "\n";
                }
            }
            out << "\n";
            for (const auto &obj : prog->objects) {
                if (obj)
                    dumpProgram(obj.get(), out, depth + 1);
            }
        };
        dumpProgram(this, out, 0);
    }

    void except_assert(std::string reason, bool value) {
        if (value == false) {
            throw mx::Exception(reason);
        }
    }
} // namespace mxvm
