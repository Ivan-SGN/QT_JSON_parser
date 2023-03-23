#include "jsonparser.h"

size_t JsonParser::Hash::operator()(Edge edge)const {
    if (edge.bracket == Bracket::CURLE) {
        return std::hash<char>()(edge.symb);
    }
    return std::hash<char>()(edge.symb) + 256;
}

bool JsonParser::Edge::operator==(Edge edge) const {
    return symb == edge.symb && bracket == edge.bracket;
}

ParseInfo JsonParser::End() {
    brackets_ = std::stack<Bracket>();
    line_ = 1;
    place_ = 0;
    state_ = START_;
    return ParseInfo();
}

ParseInfo JsonParser::End(char symb, size_t line, size_t place) {
    brackets_ = std::stack<Bracket>();
    line_ = 1;
    place_ = 0;
    state_ = START_;
    return ParseInfo(symb, line, place);
}
ParseInfo JsonParser::Check(const FileLoader& file_loader) {
    auto content = file_loader.File();
    for (const auto& symb : content) {
        ++place_;
        if (symb == V_TRUE_3_) {
            continue;
        }
        if (symb == '\t') {
            place_ += 4;
            continue;
        }
        if (state_machine_[state_].find({ symb, Bracket::NO_BRACKET }) != state_machine_[state_].end()) {
            state_ = state_machine_[state_][{ symb, Bracket::NO_BRACKET }];
            if (symb == '[') {
                brackets_.push(Bracket::SQUARE);
            }
            if (symb == '{') {
                brackets_.push(Bracket::CURLE);
            }
            if (symb == '\n') {
                ++line_;
                place_ = 0;
                continue;
            }
            continue;
        }
        auto edge = state_machine_[state_].end();
        if (NO_ZERO_DIGIT.find(symb) != NO_ZERO_DIGIT.end()) {
            edge = state_machine_[state_].find({ ' ', Bracket::NO_ZERO_DIGIT });
            if (edge != state_machine_[state_].end()) {
                state_ = edge->second;
                continue;
            }
        }
        if (DIGIT.find(symb) != DIGIT.end()) {
            edge = state_machine_[state_].find({ ' ', Bracket::DIGIT });
            if (edge != state_machine_[state_].end()) {
                state_ = edge->second;
                continue;
            }
        }
        edge = state_machine_[state_].find({ ' ', Bracket::SYMB });
        if (edge != state_machine_[state_].end()) {
            state_ = edge->second;
            continue;
        }
        if (brackets_.empty()) {
            return End(symb, line_, place_);
        }
        edge = state_machine_[state_].find({ symb, brackets_.top() });
        if (edge == state_machine_[state_].end()) {
            return End(symb, line_, place_);
        }
        state_ = edge->second;
        if (symb == '}' || symb == ']') {
            brackets_.pop();
            if (brackets_.empty()) {
                state_ = FINISH_;
            }
        }
        if (symb == '[') {
            brackets_.push(Bracket::SQUARE);
        }
        if (symb == '{') {
            brackets_.push(Bracket::CURLE);
        }
    }
    if (state_ != FINISH_)
        return End(content.back(), line_, place_);
    return End();
}

JsonParser::JsonParser() : line_(1), place_(1), state_(START_) {
    state_machine_[START_][{' ', Bracket::NO_BRACKET}] = START_;
    state_machine_[START_][{'\n', Bracket::NO_BRACKET}] = START_;
    state_machine_[START_][{'[', Bracket::NO_BRACKET}] = WAIT_V_;
    state_machine_[START_][{'{', Bracket::NO_BRACKET}] = WAIT_K_;

    state_machine_[FINISH_][{' ', Bracket::NO_BRACKET}] = FINISH_;
    state_machine_[FINISH_][{'\n', Bracket::NO_BRACKET}] = FINISH_;

    state_machine_[WAIT_V_][{' ', Bracket::NO_BRACKET}] = WAIT_V_;
    state_machine_[WAIT_V_][{'\n', Bracket::NO_BRACKET}] = WAIT_V_;
    state_machine_[WAIT_V_][{'[', Bracket::NO_BRACKET}] = WAIT_V_;
    state_machine_[WAIT_V_][{'{', Bracket::NO_BRACKET}] = WAIT_K_;
    state_machine_[WAIT_V_][{'-', Bracket::NO_BRACKET}] = V_NEGATIVE_;
    state_machine_[WAIT_V_][{' ', Bracket::NO_ZERO_DIGIT}] = V_INT_;
    state_machine_[WAIT_V_][{'\"', Bracket::NO_BRACKET}] = V_STR_;
    state_machine_[WAIT_V_][{'n', Bracket::NO_BRACKET}] = V_NULL_1_;
    state_machine_[WAIT_V_][{'t', Bracket::NO_BRACKET}] = V_TRUE_1_;
    state_machine_[WAIT_V_][{'f', Bracket::NO_BRACKET}] = V_FALSE_1_;
    state_machine_[WAIT_V_][{']', Bracket::SQUARE}] = WAIT_COMMA_;
    state_machine_[WAIT_V_][{'}', Bracket::CURLE}] = WAIT_COMMA_;
    state_machine_[WAIT_V_][{'0', Bracket::NO_BRACKET}] = ZERO_;

    state_machine_[WAIT_V_IN_PAIR_][{' ', Bracket::NO_BRACKET}] = WAIT_V_IN_PAIR_;
    state_machine_[WAIT_V_IN_PAIR_][{'\n', Bracket::NO_BRACKET}] = WAIT_V_IN_PAIR_;
    state_machine_[WAIT_V_IN_PAIR_][{'[', Bracket::NO_BRACKET}] = WAIT_V_;
    state_machine_[WAIT_V_IN_PAIR_][{'{', Bracket::NO_BRACKET}] = WAIT_K_;
    state_machine_[WAIT_V_IN_PAIR_][{'-', Bracket::NO_BRACKET}] = V_NEGATIVE_;
    state_machine_[WAIT_V_IN_PAIR_][{'0', Bracket::NO_BRACKET}] = ZERO_;
    state_machine_[WAIT_V_IN_PAIR_][{' ', Bracket::NO_ZERO_DIGIT}] = V_INT_;
    state_machine_[WAIT_V_IN_PAIR_][{'\"', Bracket::NO_BRACKET}] = V_STR_;
    state_machine_[WAIT_V_IN_PAIR_][{'n', Bracket::NO_BRACKET}] = V_NULL_1_;
    state_machine_[WAIT_V_IN_PAIR_][{'t', Bracket::NO_BRACKET}] = V_TRUE_1_;
    state_machine_[WAIT_V_IN_PAIR_][{'f', Bracket::NO_BRACKET}] = V_FALSE_1_;

    state_machine_[V_INT_][{' ', Bracket::NO_BRACKET}] = WAIT_COMMA_;
    state_machine_[V_INT_][{'.', Bracket::NO_BRACKET}] = V_DOT_;
    state_machine_[V_INT_][{' ', Bracket::DIGIT}] = V_INT_;
    state_machine_[V_INT_][{',', Bracket::SQUARE}] = WAIT_V_;
    state_machine_[V_INT_][{',', Bracket::CURLE}] = WAIT_K_;
    state_machine_[V_INT_][{']', Bracket::SQUARE}] = WAIT_COMMA_;
    state_machine_[V_INT_][{'}', Bracket::CURLE}] = WAIT_COMMA_;

    state_machine_[V_FLOAT_][{' ', Bracket::NO_BRACKET}] = WAIT_COMMA_;
    state_machine_[V_FLOAT_][{' ', Bracket::DIGIT}] = V_FLOAT_;
    state_machine_[V_FLOAT_][{',', Bracket::SQUARE}] = WAIT_V_;
    state_machine_[V_FLOAT_][{',', Bracket::CURLE}] = WAIT_K_;
    state_machine_[V_FLOAT_][{']', Bracket::SQUARE}] = WAIT_COMMA_;
    state_machine_[V_FLOAT_][{'}', Bracket::CURLE}] = WAIT_COMMA_;

    state_machine_[V_NEGATIVE_][{' ', Bracket::NO_ZERO_DIGIT}] = V_INT_;
    state_machine_[V_NEGATIVE_][{'0', Bracket::NO_BRACKET}] = ZERO_;

    state_machine_[V_STR_][{'\\', Bracket::NO_BRACKET}] = AFTER_SLASH_V_;
    state_machine_[V_STR_][{'\"', Bracket::NO_BRACKET}] = WAIT_COMMA_;
    state_machine_[V_STR_][{' ', Bracket::SYMB}] = V_STR_;

    state_machine_[V_FALSE_1_][{'a', Bracket::NO_BRACKET}] = V_FALSE_2_;
    state_machine_[V_FALSE_2_][{'l', Bracket::NO_BRACKET}] = V_FALSE_3_;
    state_machine_[V_FALSE_3_][{'s', Bracket::NO_BRACKET}] = V_FALSE_4_;
    state_machine_[V_FALSE_4_][{'e', Bracket::NO_BRACKET}] = WAIT_COMMA_;

    state_machine_[V_TRUE_1_][{'r', Bracket::NO_BRACKET}] = V_TRUE_2_;
    state_machine_[V_TRUE_2_][{'u', Bracket::NO_BRACKET}] = V_TRUE_3_;
    state_machine_[V_TRUE_3_][{'e', Bracket::NO_BRACKET}] = WAIT_COMMA_;

    state_machine_[V_NULL_1_][{'u', Bracket::NO_BRACKET}] = V_NULL_2_;
    state_machine_[V_NULL_2_][{'l', Bracket::NO_BRACKET}] = V_NULL_3_;
    state_machine_[V_NULL_3_][{'l', Bracket::NO_BRACKET}] = WAIT_COMMA_;

    state_machine_[WAIT_COMMA_][{' ', Bracket::NO_BRACKET}] = WAIT_COMMA_;
    state_machine_[WAIT_COMMA_][{'\n', Bracket::NO_BRACKET}] = WAIT_COMMA_;
    state_machine_[WAIT_COMMA_][{']', Bracket::SQUARE}] = WAIT_COMMA_;
    state_machine_[WAIT_COMMA_][{'}', Bracket::CURLE}] = WAIT_COMMA_;
    state_machine_[WAIT_COMMA_][{',', Bracket::SQUARE}] = WAIT_V_;
    state_machine_[WAIT_COMMA_][{',', Bracket::CURLE}] = WAIT_K_;

    state_machine_[WAIT_COLON_][{' ', Bracket::NO_BRACKET}] = WAIT_COLON_;
    state_machine_[WAIT_COLON_][{'\n', Bracket::NO_BRACKET}] = WAIT_COLON_;
    state_machine_[WAIT_COLON_][{':', Bracket::NO_BRACKET}] = WAIT_V_IN_PAIR_;

    state_machine_[WAIT_K_][{' ', Bracket::NO_BRACKET}] = WAIT_K_;
    state_machine_[WAIT_K_][{'\n', Bracket::NO_BRACKET}] = WAIT_K_;
    state_machine_[WAIT_K_][{'\"', Bracket::NO_BRACKET}] = K_;
    state_machine_[WAIT_K_][{']', Bracket::SQUARE}] = WAIT_COMMA_;
    state_machine_[WAIT_K_][{'}', Bracket::CURLE}] = WAIT_COMMA_;

    state_machine_[K_][{'\"', Bracket::NO_BRACKET}] = WAIT_COLON_;
    state_machine_[K_][{' ', Bracket::SYMB}] = K_;
    state_machine_[K_][{'\\', Bracket::NO_BRACKET}] = AFTER_SLASH_K_;

    state_machine_[AFTER_SLASH_V_][{' ', Bracket::SYMB}] = V_STR_;

    state_machine_[AFTER_SLASH_K_][{' ', Bracket::SYMB}] = K_;

    state_machine_[ZERO_][{'.', Bracket::NO_BRACKET}] = V_DOT_;
    state_machine_[ZERO_][{',', Bracket::SQUARE}] = WAIT_V_;
    state_machine_[ZERO_][{',', Bracket::CURLE}] = WAIT_K_;
    state_machine_[ZERO_][{']', Bracket::SQUARE}] = WAIT_COMMA_;
    state_machine_[ZERO_][{'}', Bracket::CURLE}] = WAIT_COMMA_;

    state_machine_[V_DOT_][{' ', Bracket::DIGIT}] = V_FLOAT_;
}

const std::unordered_set<char> JsonParser::NO_ZERO_DIGIT = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
const std::unordered_set<char> JsonParser::DIGIT = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

