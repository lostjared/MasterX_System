#include "symbol.hpp"
#include "function.hpp"

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

extern std::ostringstream stream;

namespace symbol {
    
    Value::Value() : text(""), value(0) { }
    Value::Value(const Value &v) : text(v.text), value(v.value) { }
    Value::Value(const std::string &t, const double &v) : text(t), value(v) { }
    
    Variable::Variable() : is_array(false), array_size(0), values(nullptr) {}
    
    Variable::~Variable() {
        free();
#ifdef DEBUG_MODE
        stream << "destructor -> " << name << "\r\n";
#endif
    }
    
    void Variable::free() {
        if(values != nullptr) {
            if(is_array)
                delete [] values;
            else
                delete values;
            values = nullptr;
        }
    }
    
    Variable::Variable(const std::string &n, const Value &value)  {
        values = nullptr;
        create(n, value);
    }
    
    Variable::Variable(const std::string &n, const unsigned long arr_size) {
        values = nullptr;
        createArray(n, arr_size);
    }
    
    bool Variable::operator<(const Variable &v) const {
        if(name < v.name) return true;
        return false;
    }
    
    void Variable::setValue(const Value &v) {
        if(values != nullptr && is_array == false) {
            values->text = v.text;
            values->value = v.value;
        }
    }
    
    Value &Variable::getValue() const {
        return *values;
    }
    
    void Variable::createArray(const std::string &n, const unsigned long arr_size) {
        free();
        array_size = arr_size;
        is_array = true;
        name = n;
        values = new Value[arr_size+1];
    }
    
    void Variable::create(const std::string &n, const Value &v) {
        free();
        name = n;
        values = new Value(v.text, v.value);
        is_array = false;
        array_size = 0;
    }
    
    std::string &Variable::get_text(unsigned long index) {
        return values[index].text;
    }
    
    std::string &Variable::get_text() {
        return values->text;
    }
    
    double &Variable::get_double() {
        return values->value;
    }
    
    double &Variable::get_double(unsigned long index) {
        return values[index].value;
    }
    
    std::ostream &operator<<(std::ostream &out, Variable &v) {
        bool ohex = code.symbols["hex"].get_double()!=0;
        
        if(v.isArray() == false)
            if(ohex == false)
            	out << v.name << " = { \"" << v.get_text() << "\" : " << v.get_double() << " } ";
        	else
                out << v.name << " = { \"" << v.get_text() << "\" : $" << std::uppercase << std::hex << static_cast<int>(v.get_double()) << " } ";
        else {
            for(unsigned int i = 0; i < v.arraySize(); ++i) {
                
                if(ohex == false || v.get_double(i) < 0)
                	out << v.name << "[" << i << "] = { \"" << v.get_text(i) << "\" : " << v.get_double(i) << " }\r\n";
                else
                    out << v.name << " = { \"" << v.get_text() << "\" : $" << std::uppercase << std::hex << static_cast<int>(v.get_double(i)) << " } ";
            }
        }
        return out;
    }
}
