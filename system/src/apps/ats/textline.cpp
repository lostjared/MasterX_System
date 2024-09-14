#include "code.hpp"


namespace interp {
    // Text Line
    
    TextLine::TextLine() : index(0), text("") {}
    
    TextLine::TextLine(int index, std::string text)
    {
        this->index = index;
        this->text = text;
    }
    
    TextLine::TextLine(const TextLine &t) {
        index = t.index;
        text = t.text;
    }
    bool TextLine::operator<(const TextLine &t) const {
        return (index < t.index);
    }
    
    bool TextLine::operator==(const TextLine &t1) const {
        return (index == t1.index);
    }
    
    bool TextLine::operator!=(const TextLine &t1) const {
        return (index != t1.index);
    }
    
    TextLine &TextLine::operator=(const TextLine &t) {
        index = t.index;
        text = t.text;
        return *this;
    }

}
