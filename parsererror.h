#ifndef PARSERERROR_H
#define PARSERERROR_H

#include <exception>
#include <string>
#include<cstring>

class ParserError {
private:
    char symb_;
    size_t line_;
    size_t place_;
public:
    ParserError(char symb, size_t line, size_t place);
    std::string Description() const;
};

class ParseInfo {
private:
    bool correct_;
    char symb_;
    size_t line_;
    size_t place_;
public:
    ParseInfo(char symb, size_t line, size_t place);
    ParseInfo();
    void Description() const;
};


#endif // PARSERERROR_H
