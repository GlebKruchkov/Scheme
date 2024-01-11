#pragma once

#include <variant>
#include <optional>
#include <istream>
#include <sstream>
#include <error.h>
#include <string>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return this->name == other.name;
    };
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    };
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    };
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return this->value == other.value;
    };
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in) {
        string_ = in;
        Next();
    };

    bool IsEnd() {
        return end_;
    };

    void Next() {
        std::string str;

        bool flag = true;
        char c = string_->get();
        if (c == EOF) {
            end_ = true;
            return;
        } else if (c == ' ') {
            flag = false;
        } else if (c == '\n') {
            flag = false;
        } else if (c >= 123 || (c >= 91 && c <= 95) || c == 64 || (c == 58 || c == 59) || c <= 31) {
            throw SyntaxError("error");
        } else {
            str += c;
        }

        if (flag) {
            if (str[0] == '(') {
                token_ = BracketToken::OPEN;
            } else if (str[0] == ')') {
                token_ = BracketToken::CLOSE;
            } else if (str[0] == '\'') {
                token_ = QuoteToken();
            } else if (str[0] == '.') {
                token_ = DotToken();
            } else if (str[0] == '-' || str[0] == '+') {
                while (flag) {
                    c = static_cast<char>(string_->peek());
                    if (c == EOF) {
                        flag = false;
                    } else if (c == ' ') {
                        flag = false;
                    } else if (c == '\n') {
                        flag = false;
                    } else if (!isdigit(c)) {
                        flag = false;
                    } else {
                        string_->get();
                        str += c;
                    }
                }
                if (str.size() == 1) {
                    token_ = SymbolToken{str};
                } else {
                    token_ = ConstantToken{std::stoi(str)};
                }
            } else if (isdigit(str[0])) {
                while (flag) {
                    c = static_cast<char>(string_->peek());
                    if (c == EOF) {
                        flag = false;
                    } else if (c == ' ') {
                        flag = false;
                    } else if (c == '\n') {
                        flag = false;
                    } else if (!isdigit(c)) {
                        flag = false;
                    } else {
                        string_->get();
                        str += c;
                    }
                }
                token_ = ConstantToken{std::stoi(str)};
            } else {
                while (flag) {
                    c = static_cast<char>(string_->peek());
                    if (c == EOF) {
                        flag = false;
                    } else if (c == ' ') {
                        flag = false;
                    } else if (c == '\n') {
                        flag = false;
                    } else if (c == '(') {
                        flag = false;
                    } else if (c == ')') {
                        flag = false;
                    } else if (c == '\'') {
                        flag = false;
                    } else if (c == '.') {
                        flag = false;
                    } else if (c >= 123 || (c >= 91 && c <= 95) || c == 64 ||
                               (c >= 58 && c <= 59) || c <= 31) {
                        throw SyntaxError("error");
                    } else {
                        string_->get();
                        str += c;
                    }
                }
                token_ = SymbolToken{str};
            }
        } else {
            Next();
        }
    };

    Token GetToken() {
        return token_;
    };

private:
    std::istream* string_;
    bool end_ = false;
    Token token_;
};
