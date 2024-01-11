#pragma once

#include <string>
#include <tokenizer.h>
#include <parser.h>
#include <vector>
#include <algorithm>

class Interpreter {
public:
    std::string Run(const std::string& str);
};

class IsNumber : Object {
public:
    IsNumber(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Compare : Object {
public:
    Compare(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Min : Object {
public:
    Min(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Max : Object {
public:
    Max(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Abs : Object {
public:
    Abs(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class MathOperations : Object {
public:
    MathOperations(std::shared_ptr<Object> obj, std::string operation)
        : obj_(obj), operation_(operation) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
    std::string operation_;
};

class IsBool : Object {
public:
    IsBool(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Not : Object {
public:
    Not(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class And : Object {
public:
    And(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Quote : Object {
public:
    Quote(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Or : Object {
public:
    Or(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Pair : Object {
public:
    Pair(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Null : Object {
public:
    Null(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class IsList : Object {
public:
    IsList(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class List : Object {
public:
    List(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Car : Object {
public:
    Car(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Cdr : Object {
public:
    Cdr(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Cons : Object {
public:
    Cons(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Ref : Object {
public:
    Ref(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};

class Tail : Object {
public:
    Tail(std::shared_ptr<Object> obj) : obj_(obj) {
    }
    std::shared_ptr<Object> Do() override;

private:
    std::shared_ptr<Object> obj_;
};