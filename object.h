#pragma once

#include <memory>
#include <string>
#include <error.h>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::string GetString() {
        return "";
    }
    virtual std::shared_ptr<Object> Do() {
        return nullptr;
    }
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int value) : value_(value) {
    }
    std::string GetString() override {
        return std::to_string(value_);
    }
    int GetValue() const {
        return value_;
    };

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol(std::string str) : str_(str) {
    }
    std::string GetString() override {
        return str_;
    }
    const std::string& GetName() const {
        return str_;
    };

private:
    const std::string str_;
};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
    }
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    };

    std::string GetString() override {
        if (!second_ && !first_) {
            return "(())";
        }
        if (!second_) {
            std::string answer;
            answer += "(";
            answer += first_->GetString();
            answer += ")";
            return answer;
        }
        if (!std::dynamic_pointer_cast<Cell>(second_)) {
            std::string str;
            str += "(";
            str += first_->GetString();
            str += " . ";
            str += second_->GetString();
            str += ")";
            return str;
        }
        auto f = first_;
        auto s = second_;
        std::string ans;
        ans += "(";
        ans += f->GetString();
        ans += " ";
        while (s != nullptr) {
            if (!std::dynamic_pointer_cast<Cell>(s)) {
                ans += ". ";
                ans += s->GetString();
                ans += " ";
                break;
            } else {
                f = std::dynamic_pointer_cast<Cell>(s)->GetFirst();
                ans += f->GetString();
                ans += " ";
                s = std::dynamic_pointer_cast<Cell>(s)->GetSecond();
            }
        }
        ans.pop_back();
        ans += ")";
        return ans;
    }

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return As<T>(obj) != nullptr;
}