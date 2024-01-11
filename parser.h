#pragma once
#include <memory>
#include "object.h"
#include <tokenizer.h>
inline std::shared_ptr<Object> Read(Tokenizer* tokenizer);

inline std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);

inline std::shared_ptr<Object> ReadQuote(Tokenizer* tokenizer) {
    tokenizer->Next();
    std::shared_ptr<Object> expr = Read(tokenizer);
    auto nul = std::shared_ptr<Object>(nullptr);
    std::shared_ptr<Object> temp = std::make_shared<Cell>(expr, nul);
    return std::make_shared<Cell>(std::make_shared<Symbol>("quote"), temp);
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("error");
    }
    if (std::holds_alternative<ConstantToken>(tokenizer->GetToken())) {
        int val = std::get<ConstantToken>(tokenizer->GetToken()).value;
        tokenizer->Next();
        return std::make_shared<Number>(val);
    }
    if (std::holds_alternative<SymbolToken>(tokenizer->GetToken())) {
        std::string val = std::get<SymbolToken>(tokenizer->GetToken()).name;
        tokenizer->Next();
        return std::make_shared<Symbol>(val);
    }
    if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
        tokenizer->Next();
        return ReadList(tokenizer);
    }
    if (std::holds_alternative<QuoteToken>(tokenizer->GetToken())) {
        return ReadQuote(tokenizer);
    }
    throw SyntaxError("error");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        tokenizer->Next();
        return nullptr;
    } else {
        if (tokenizer->IsEnd()) {
            throw SyntaxError("error");
        }
        std::shared_ptr<Object> head = Read(tokenizer);
        if (tokenizer->IsEnd()) {
            throw SyntaxError("error");
        }
        std::shared_ptr<Object> tail;
        if (std::holds_alternative<DotToken>(tokenizer->GetToken())) {
            tokenizer->Next();
            if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
                throw SyntaxError("error");
            }
            if (std::holds_alternative<ConstantToken>(tokenizer->GetToken())) {
                int val = std::get<ConstantToken>(tokenizer->GetToken()).value;
                tokenizer->Next();
                tail = std::make_shared<Number>(val);
                if (std::holds_alternative<ConstantToken>(tokenizer->GetToken())) {
                    throw SyntaxError("error");
                }
            } else {
                tail = Read(tokenizer);
            }
            if (tokenizer->IsEnd()) {
                throw SyntaxError("error");
            }
            tokenizer->Next();
        } else {
            tail = ReadList(tokenizer);
        }
        return std::make_shared<Cell>(head, tail);
    }
}