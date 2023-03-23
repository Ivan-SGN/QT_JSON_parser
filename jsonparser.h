#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <array>
#include <unordered_map>
#include "fileloader.h"
#include <stack>
#include "parsererror.h"
#include <unordered_set>
#define STATE_SIZE 26


class JsonParser {
private:
    static const size_t START_ = 0;
    static const size_t FINISH_ = 1;
    static const size_t WAIT_V_ = 2;
    static const size_t WAIT_V_IN_PAIR_ = 3;
    static const size_t V_INT_ = 4;
    static const size_t V_FLOAT_ = 5;
    static const size_t V_NEGATIVE_ = 6;
    static const size_t V_STR_ = 7;
    static const size_t V_NULL_1_ = 8;
    static const size_t V_NULL_2_ = 9;
    static const size_t V_NULL_3_ = 10;
    static const size_t V_TRUE_1_ = 11;
    static const size_t V_TRUE_2_ = 12;
    static const size_t V_TRUE_3_ = 13;
    static const size_t V_FALSE_1_ = 14;
    static const size_t V_FALSE_2_ = 15;
    static const size_t V_FALSE_3_ = 16;
    static const size_t V_FALSE_4_ = 17;
    static const size_t WAIT_COMMA_ = 18;
    static const size_t WAIT_COLON_ = 19;
    static const size_t WAIT_K_ = 20;
    static const size_t K_ = 21;
    static const size_t ZERO_ = 22;
    static const size_t AFTER_SLASH_V_ = 23;
    static const size_t AFTER_SLASH_K_ = 24;
    static const size_t V_DOT_ = 25;

    static const std::unordered_set<char> NO_ZERO_DIGIT;
    static const std::unordered_set<char> DIGIT;
    enum class Bracket {
        NO_BRACKET,
        SQUARE,
        CURLE,
        SYMB,
        NO_ZERO_DIGIT,
        DIGIT
    };

    struct Edge{
        char symb;
        Bracket bracket;
        bool operator==(Edge edge) const;
    };

    class Hash {
    public:
        size_t operator()(Edge edge) const;
    };
private:
    std::array<std::unordered_map<Edge, size_t, Hash>, STATE_SIZE> state_machine_;
    std::stack<Bracket> brackets_;
    size_t line_;
    size_t place_;
    size_t state_;
    ParseInfo End();
    ParseInfo End(char symb, size_t line, size_t place);
public:
    JsonParser();
    ParseInfo Check(const FileLoader& file_loader);
};



#endif // JSONPARSER_H
