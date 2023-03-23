#include "parsererror.h"

ParserError::ParserError(char symb, size_t line, size_t place) : symb_(symb), line_(line), place_(place) {}

std::string ParserError::Description() const {
    std::string result = "unexpected symbol: ";
    result += symb_;
    result += " line ";
    result += std::to_string(line_);
    result += " place ";
    result += std::to_string(place_);
    return result;
}

ParseInfo::ParseInfo(char symb, size_t line, size_t place) : correct_(false), symb_(symb), line_(line), place_(place) {}

ParseInfo::ParseInfo() : correct_(true) {}

void ParseInfo::Description() const{
    if (!correct_){
        throw ParserError(symb_, line_, place_);
    }
}
