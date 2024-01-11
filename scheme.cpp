#include "scheme.h"

bool IsDigit(const std::string &s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) {
                             return !(std::isdigit(c) || c == '-');
                         }) == s.end();
}

std::shared_ptr<Object> AllOperations(std::shared_ptr<Object> list) {
    auto first = As<Cell>(list)->GetFirst();
    if (!first) {
        throw RuntimeError("Nothing to operate");
    }
    if (first->GetString() == "number?") {
        list = As<Cell>(list)->GetSecond();
        auto second = As<Cell>(list)->GetFirst();
        return IsNumber(second).Do();
    } else {
        if (As<Cell>(list)->GetFirst()->GetString()[0] <= 62 &&
            As<Cell>(list)->GetFirst()->GetString()[0] >= 60) {
            return Compare(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "max") {
            return Max(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "min") {
            return Min(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "+" ||
                   As<Cell>(list)->GetFirst()->GetString() == "-" ||
                   As<Cell>(list)->GetFirst()->GetString() == "*" ||
                   As<Cell>(list)->GetFirst()->GetString() == "/") {
            if (As<Cell>(list)->GetSecond()) {
                auto tmp = As<Cell>(list)->GetSecond();
                if (Is<Cell>(tmp)) {
                    auto temp = As<Cell>(tmp);
                    if (!temp->GetFirst() && !temp->GetSecond()) {
                        throw RuntimeError("nothing to +");
                    }
                }
            }
            return MathOperations(list, As<Cell>(list)->GetFirst()->GetString()).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "abs") {
            return Abs(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "boolean?") {
            list = As<Cell>(list)->GetSecond();
            auto second = As<Cell>(list)->GetFirst();
            return IsBool(second).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "not") {
            if (!As<Cell>(list)->GetSecond()) {
                throw RuntimeError("Nothing to NOT");
            }
            list = As<Cell>(list)->GetSecond();
            return Not(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "and") {
            return And(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "quote") {
            return Quote(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "or") {
            return Or(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "pair?") {
            if (!Is<Cell>(list)) {
                throw RuntimeError("pair? isn't good");
            }
            list = As<Cell>(list)->GetSecond();
            auto second = As<Cell>(list)->GetFirst();
            return Pair(second).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "null?") {
            if (!Is<Cell>(list)) {
                throw RuntimeError("null? isn't good");
            }
            list = As<Cell>(list)->GetSecond();
            auto second = As<Cell>(list)->GetFirst();
            return Null(second).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "list?") {
            if (!Is<Cell>(list)) {
                throw RuntimeError("list? isn't good");
            }
            list = As<Cell>(list)->GetSecond();
            auto second = As<Cell>(list)->GetFirst();
            return IsList(second).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "list") {
            return List(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "car") {
            if (!As<Cell>(list)->GetSecond()) {
                throw RuntimeError("Unable to car from empty");
            }
            list = As<Cell>(list)->GetSecond();
            auto second = As<Cell>(list)->GetFirst();
            return Car(As<Cell>(second)->GetSecond()).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "cdr") {
            if (!As<Cell>(list)->GetSecond()) {
                throw RuntimeError("Unable to cdr from empty");
            }
            list = As<Cell>(list)->GetSecond();
            auto second = As<Cell>(list)->GetFirst();
            return Cdr(As<Cell>(second)->GetSecond()).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "cons") {
            if (!As<Cell>(list)->GetSecond()) {
                throw RuntimeError("Unable to cons from empty");
            }
            list = As<Cell>(list)->GetSecond();
            return Cons(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "list-ref") {
            if (!As<Cell>(list)->GetSecond()) {
                throw RuntimeError("Unable to ref from empty");
            }
            list = As<Cell>(list)->GetSecond();
            return Ref(list).Do();
        } else if (As<Cell>(list)->GetFirst()->GetString() == "list-tail") {
            if (!As<Cell>(list)->GetSecond()) {
                throw RuntimeError("Unable to tail from empty");
            }
            list = As<Cell>(list)->GetSecond();
            return Tail(list).Do();
        } else {
            if (IsDigit(first->GetString())) {
                throw RuntimeError("First elem in list cannot be int");
            }
            return first;
        }
    }
}

std::string Interpreter::Run(const std::string &str) {
    std::stringstream my_stream;
    my_stream.str(std::move(str));
    Tokenizer tokenizer{&my_stream};
    if (std::holds_alternative<ConstantToken>(tokenizer.GetToken()) ||
        std::holds_alternative<SymbolToken>(tokenizer.GetToken())) {
        return Read(&tokenizer)->GetString();
    }
    auto list = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Tokenizer isn't end after read");
    }
    if (!list) {
        throw RuntimeError("Empty object");
    }

    auto first = As<Cell>(list)->GetFirst();
    if (Is<Cell>(first)) {
        throw RuntimeError("First element is a cell");
    }

    auto el = AllOperations(list);
    if (!el) {
        return "()";
    }
    return AllOperations(list)->GetString();
}

std::shared_ptr<Object> IsNumber::Do() {
    if (IsDigit(obj_->GetString())) {
        return std::make_shared<Symbol>("#t");
    } else {
        return std::make_shared<Symbol>("#f");
    }
}

std::shared_ptr<Object> Compare::Do() {
    if (As<Cell>(obj_)->GetFirst()->GetString() == "=") {
        obj_ = As<Cell>(obj_)->GetSecond();
        if (!obj_) {
            return std::make_shared<Symbol>("#t");
        }
        while (As<Cell>(obj_)->GetSecond()) {
            auto temp = As<Cell>(obj_)->GetSecond();
            auto first = As<Cell>(temp)->GetFirst();
            auto second = As<Cell>(obj_)->GetFirst();

            if (Is<Cell>(first)) {
                first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
            }

            if (Is<Cell>(second)) {
                second = MathOperations(second, As<Cell>(second)->GetFirst()->GetString()).Do();
            }

            if (!IsDigit(first->GetString()) || !IsDigit(second->GetString())) {
                throw RuntimeError("Unable to get int in comparison");
            }
            if (std::stoi(first->GetString()) != std::stoi(second->GetString())) {
                return std::make_shared<Symbol>("#f");
            }
            obj_ = temp;
        }
        return std::make_shared<Symbol>("#t");
    } else if (As<Cell>(obj_)->GetFirst()->GetString() == ">") {
        obj_ = As<Cell>(obj_)->GetSecond();
        if (!obj_) {
            return std::make_shared<Symbol>("#t");
        }
        while (As<Cell>(obj_)->GetSecond()) {
            auto temp = As<Cell>(obj_)->GetSecond();
            auto first = As<Cell>(temp)->GetFirst();
            auto second = As<Cell>(obj_)->GetFirst();

            if (Is<Cell>(first)) {
                first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
            }

            if (Is<Cell>(second)) {
                second = MathOperations(second, As<Cell>(second)->GetFirst()->GetString()).Do();
            }

            if (!IsDigit(first->GetString()) || !IsDigit(second->GetString())) {
                throw RuntimeError("Unable to get int in comparison");
            }
            if (std::stoi(first->GetString()) >= std::stoi(second->GetString())) {
                return std::make_shared<Symbol>("#f");
            }
            obj_ = temp;
        }
        return std::make_shared<Symbol>("#t");
    } else if (As<Cell>(obj_)->GetFirst()->GetString() == "<") {
        obj_ = As<Cell>(obj_)->GetSecond();
        if (!obj_) {
            return std::make_shared<Symbol>("#t");
        }
        while (As<Cell>(obj_)->GetSecond()) {
            auto temp = As<Cell>(obj_)->GetSecond();
            auto first = As<Cell>(temp)->GetFirst();
            auto second = As<Cell>(obj_)->GetFirst();

            if (Is<Cell>(first)) {
                first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
            }

            if (Is<Cell>(second)) {
                second = MathOperations(second, As<Cell>(second)->GetFirst()->GetString()).Do();
            }

            if (!IsDigit(first->GetString()) || !IsDigit(second->GetString())) {
                throw RuntimeError("Unable to get int in comparison");
            }
            if (std::stoi(first->GetString()) <= std::stoi(second->GetString())) {
                return std::make_shared<Symbol>("#f");
            }
            obj_ = temp;
        }
        return std::make_shared<Symbol>("#t");
    } else if (As<Cell>(obj_)->GetFirst()->GetString() == "<") {
        obj_ = As<Cell>(obj_)->GetSecond();
        if (!obj_) {
            return std::make_shared<Symbol>("#t");
        }
        while (As<Cell>(obj_)->GetSecond()) {
            auto temp = As<Cell>(obj_)->GetSecond();
            auto first = As<Cell>(temp)->GetFirst();
            auto second = As<Cell>(obj_)->GetFirst();

            if (Is<Cell>(first)) {
                first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
            }

            if (Is<Cell>(second)) {
                second = MathOperations(second, As<Cell>(second)->GetFirst()->GetString()).Do();
            }

            if (!IsDigit(first->GetString()) || !IsDigit(second->GetString())) {
                throw RuntimeError("Unable to get int in comparison");
            }
            if (std::stoi(first->GetString()) <= std::stoi(second->GetString())) {
                return std::make_shared<Symbol>("#f");
            }
            obj_ = temp;
        }
        return std::make_shared<Symbol>("#t");
    } else if (As<Cell>(obj_)->GetFirst()->GetString() == "<=") {
        obj_ = As<Cell>(obj_)->GetSecond();
        if (!obj_) {
            return std::make_shared<Symbol>("#t");
        }
        while (As<Cell>(obj_)->GetSecond()) {
            auto temp = As<Cell>(obj_)->GetSecond();
            auto first = As<Cell>(temp)->GetFirst();
            auto second = As<Cell>(obj_)->GetFirst();

            if (Is<Cell>(first)) {
                first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
            }

            if (Is<Cell>(second)) {
                second = MathOperations(second, As<Cell>(second)->GetFirst()->GetString()).Do();
            }

            if (!IsDigit(first->GetString()) || !IsDigit(second->GetString())) {
                throw RuntimeError("Unable to get int in comparison");
            }
            if (std::stoi(first->GetString()) < std::stoi(second->GetString())) {
                return std::make_shared<Symbol>("#f");
            }
            obj_ = temp;
        }
        return std::make_shared<Symbol>("#t");
    } else {
        obj_ = As<Cell>(obj_)->GetSecond();
        if (!obj_) {
            return std::make_shared<Symbol>("#t");
        }
        while (As<Cell>(obj_)->GetSecond()) {
            auto temp = As<Cell>(obj_)->GetSecond();
            auto first = As<Cell>(temp)->GetFirst();
            auto second = As<Cell>(obj_)->GetFirst();

            if (Is<Cell>(first)) {
                first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
            }

            if (Is<Cell>(second)) {
                second = MathOperations(second, As<Cell>(second)->GetFirst()->GetString()).Do();
            }

            if (!IsDigit(first->GetString()) || !IsDigit(second->GetString())) {
                throw RuntimeError("Unable to get int in comparison");
            }
            if (std::stoi(first->GetString()) > std::stoi(second->GetString())) {
                return std::make_shared<Symbol>("#f");
            }
            obj_ = temp;
        }
        return std::make_shared<Symbol>("#t");
    }
}

std::shared_ptr<Object> MathOperations::Do() {
    if (!As<Cell>(obj_)->GetSecond() && (operation_ == "-" || operation_ == "/")) {
        throw RuntimeError("Runtime error");
    }
    if (!As<Cell>(obj_)->GetSecond() && (operation_ == "+")) {
        return std::make_shared<Number>(0);
    }
    if (!As<Cell>(obj_)->GetSecond() && (operation_ == "*")) {
        return std::make_shared<Number>(1);
    }

    int res;

    if (operation_ == "+") {
        res = 0;
        while (As<Cell>(obj_)->GetSecond()) {
            obj_ = As<Cell>(obj_)->GetSecond();
            if (Is<Cell>(obj_)) {
                auto temp = As<Cell>(obj_)->GetFirst();
                if (Is<Cell>(temp)) {
                    auto cell = As<Cell>(obj_)->GetFirst();
                    if (!IsDigit(AllOperations(As<Cell>(obj_)->GetFirst())->GetString())) {
                        throw RuntimeError("unable to math");
                    }
                    res += std::stoi(AllOperations(As<Cell>(obj_)->GetFirst())->GetString());
                } else {
                    if (!As<Cell>(obj_)->GetFirst()) {
                        throw RuntimeError("Unable to math");
                    }
                    if (!IsDigit(As<Cell>(obj_)->GetFirst()->GetString())) {
                        throw RuntimeError("Unable to math");
                    }
                    if (As<Cell>(obj_)->GetFirst()->GetString() == "-") {
                        throw RuntimeError("Unable to math");
                    }
                    res += std::stoi(As<Cell>(obj_)->GetFirst()->GetString());
                }
            } else {
                if (!IsDigit(obj_->GetString())) {
                    throw RuntimeError("Unable to math");
                }
                res += std::stoi(obj_->GetString());
                break;
            }
        }
    }

    if (operation_ == "*") {
        res = 1;
        while (As<Cell>(obj_)->GetSecond()) {
            obj_ = As<Cell>(obj_)->GetSecond();
            auto temp = As<Cell>(obj_)->GetFirst();
            if (Is<Cell>(temp)) {
                auto cell = As<Cell>(obj_)->GetFirst();
                if (!IsDigit(AllOperations(As<Cell>(obj_)->GetFirst())->GetString())) {
                    throw RuntimeError("unable to math");
                }
                res *= std::stoi(AllOperations(As<Cell>(obj_)->GetFirst())->GetString());
            } else {
                if (!IsDigit(As<Cell>(obj_)->GetFirst()->GetString())) {
                    throw RuntimeError("Unable to math");
                }
                res *= std::stoi(As<Cell>(obj_)->GetFirst()->GetString());
            }
        }
    }

    if (operation_ == "-") {
        int cnt = 0;
        while (As<Cell>(obj_)->GetSecond()) {
            obj_ = As<Cell>(obj_)->GetSecond();
            if (Is<Cell>(obj_)) {
                auto temp = As<Cell>(obj_)->GetFirst();
                if (Is<Cell>(temp)) {
                    if (As<Cell>(temp)->GetFirst()->GetString() != "/" &&
                        As<Cell>(temp)->GetFirst()->GetString() != "+" &&
                        As<Cell>(temp)->GetFirst()->GetString() != "-" &&
                        As<Cell>(temp)->GetFirst()->GetString() != "*") {
                        throw RuntimeError("unable to math");
                    }
                    if (!IsDigit(AllOperations(temp)->GetString())) {
                        throw RuntimeError("unable to math");
                    }
                    if (cnt == 0) {
                        res = std::stoi(AllOperations(temp)->GetString());
                    } else {
                        res -= std::stoi(AllOperations(temp)->GetString());
                    }
                } else {
                    if (!As<Cell>(obj_)->GetFirst()) {
                        throw RuntimeError("Unable to math");
                    }
                    if (!IsDigit(As<Cell>(obj_)->GetFirst()->GetString())) {
                        throw RuntimeError("Unable to math");
                    }
                    if (cnt == 0) {
                        res = std::stoi(As<Cell>(obj_)->GetFirst()->GetString());
                    } else {
                        res -= std::stoi(As<Cell>(obj_)->GetFirst()->GetString());
                    }
                }
            } else {
                if (!IsDigit(obj_->GetString())) {
                    throw RuntimeError("Unable to math");
                }
                if (cnt == 0) {
                    res = std::stoi(obj_->GetString());
                } else {
                    res -= std::stoi(obj_->GetString());
                }
                break;
            }
            ++cnt;
        }
    }

    if (operation_ == "/") {
        int cnt = 0;
        while (As<Cell>(obj_)->GetSecond()) {
            obj_ = As<Cell>(obj_)->GetSecond();
            if (Is<Cell>(obj_)) {
                auto temp = As<Cell>(obj_)->GetFirst();
                if (Is<Cell>(temp)) {
                    auto cell = As<Cell>(obj_)->GetFirst();
                    if (!IsDigit(AllOperations(As<Cell>(obj_)->GetFirst())->GetString())) {
                        throw RuntimeError("unable to math");
                    }
                    if (cnt == 0) {
                        res = std::stoi(AllOperations(As<Cell>(obj_)->GetFirst())->GetString());
                    } else {
                        res /= std::stoi(AllOperations(As<Cell>(obj_)->GetFirst())->GetString());
                    }
                } else {
                    if (!IsDigit(As<Cell>(obj_)->GetFirst()->GetString())) {
                        throw RuntimeError("Unable to math");
                    }
                    if (cnt == 0) {
                        res = std::stoi(As<Cell>(obj_)->GetFirst()->GetString());
                    } else {
                        res /= std::stoi(As<Cell>(obj_)->GetFirst()->GetString());
                    }
                }
            } else {
                if (!IsDigit(obj_->GetString())) {
                    throw RuntimeError("unable to /");
                }
                if (cnt == 0) {
                    res = std::stoi(obj_->GetString());
                } else {
                    res /= std::stoi(obj_->GetString());
                }
            }
            ++cnt;
        }
    }
    return std::make_shared<Number>(res);
}

std::shared_ptr<Object> Max::Do() {
    int max_elem;
    obj_ = As<Cell>(obj_)->GetSecond();
    if (!obj_) {
        throw RuntimeError("No");
    }
    auto first = As<Cell>(obj_)->GetFirst();
    if (Is<Cell>(first)) {
        first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
    }
    if (IsDigit(first->GetString())) {
        max_elem = std::stoi(first->GetString());
    } else {
        throw RuntimeError("Unable to Max");
    }
    while (As<Cell>(obj_)->GetSecond()) {
        obj_ = As<Cell>(obj_)->GetSecond();
        first = As<Cell>(obj_)->GetFirst();
        if (Is<Cell>(first)) {
            first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
        }
        if (IsDigit(first->GetString())) {
            max_elem = std::max(std::stoi(first->GetString()), max_elem);
        } else {
            throw RuntimeError("Unable to Max");
        }
    }
    return std::make_shared<Number>(max_elem);
}

std::shared_ptr<Object> Min::Do() {
    int min_elem;
    obj_ = As<Cell>(obj_)->GetSecond();
    if (!obj_) {
        throw RuntimeError("No");
    }
    auto first = As<Cell>(obj_)->GetFirst();
    if (Is<Cell>(first)) {
        first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
    }
    if (IsDigit(first->GetString())) {
        min_elem = std::stoi(first->GetString());
    } else {
        throw RuntimeError("Unable to min");
    }
    while (As<Cell>(obj_)->GetSecond()) {
        obj_ = As<Cell>(obj_)->GetSecond();
        first = As<Cell>(obj_)->GetFirst();
        if (Is<Cell>(first)) {
            first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
        }
        if (IsDigit(first->GetString())) {
            min_elem = std::min(std::stoi(first->GetString()), min_elem);
        } else {
            throw RuntimeError("Unable to min");
        }
    }
    return std::make_shared<Number>(min_elem);
}

std::shared_ptr<Object> Abs::Do() {
    if (!As<Cell>(obj_)->GetSecond()) {
        throw RuntimeError("No");
    }
    obj_ = As<Cell>(obj_)->GetSecond();

    auto first = As<Cell>(obj_)->GetFirst();
    if (Is<Cell>(first)) {
        first = MathOperations(first, As<Cell>(first)->GetFirst()->GetString()).Do();
    }

    if (As<Cell>(obj_)->GetSecond()) {
        throw RuntimeError("No");
    }
    if (!IsDigit(first->GetString())) {
        throw RuntimeError("No");
    }
    return std::make_shared<Number>(std::abs(std::stoi(first->GetString())));
}

std::shared_ptr<Object> IsBool::Do() {
    std::string str;
    if (Is<Cell>(obj_)) {
        obj_ = AllOperations(obj_);
    }
    if (!obj_) {
        str = "()";
    } else {
        str = obj_->GetString();
    }
    if (str.size() != 2) {
        return std::make_shared<Symbol>("#f");
    }
    if (str[0] != '#' || (str[1] != 't' && str[1] != 'f')) {
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Not::Do() {
    if (As<Cell>(obj_)->GetSecond()) {
        throw RuntimeError("error");
    }
    if (IsBool(As<Cell>(obj_)->GetFirst()).Do()->GetString() == "#t" &&
        As<Cell>(obj_)->GetFirst()->GetString() == "#f") {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Quote::Do() {
    obj_ = As<Cell>(obj_)->GetSecond();
    return As<Cell>(obj_)->GetFirst();
}

std::shared_ptr<Object> And::Do() {
    if (!As<Cell>(obj_)->GetSecond()) {
        return std::make_shared<Symbol>("#t");
    }
    bool flag = true;
    obj_ = As<Cell>(obj_)->GetSecond();
    std::shared_ptr<Object> answer;

    while (obj_) {
        auto first = As<Cell>(obj_)->GetFirst();

        if (Is<Cell>(first)) {
            first = AllOperations(first);
        }
        if (first->GetString() == "#f") {
            flag = false;
        }
        answer = first;
        obj_ = As<Cell>(obj_)->GetSecond();
    }

    if (!flag) {
        return std::make_shared<Symbol>("#f");
    }
    return answer;
}

std::shared_ptr<Object> Or::Do() {
    if (!As<Cell>(obj_)->GetSecond()) {
        return std::make_shared<Symbol>("#f");
    }
    bool flag = false;
    obj_ = As<Cell>(obj_)->GetSecond();
    std::shared_ptr<Object> answer;

    while (obj_) {
        auto first = As<Cell>(obj_)->GetFirst();

        if (Is<Cell>(first)) {
            first = AllOperations(first);
        }
        if (first->GetString() == "#t") {
            flag = true;
        }
        answer = first;
        obj_ = As<Cell>(obj_)->GetSecond();
    }

    if (flag) {
        return std::make_shared<Symbol>("#t");
    }
    return answer;
}

std::shared_ptr<Object> Pair::Do() {
    obj_ = AllOperations(obj_);
    if (!obj_ || !Is<Cell>(obj_)) {
        return std::make_shared<Symbol>("#f");
    }
    if (!As<Cell>(obj_)->GetSecond() || !As<Cell>(obj_)->GetFirst()) {
        return std::make_shared<Symbol>("#f");
    }
    obj_ = As<Cell>(obj_)->GetSecond();
    if (Is<Cell>(obj_)) {
        if (!As<Cell>(obj_)->GetFirst() || As<Cell>(obj_)->GetSecond()) {
            return std::make_shared<Symbol>("#f");
        }
        obj_ = As<Cell>(obj_)->GetSecond();
        if (Is<Cell>(obj_)) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Null::Do() {
    obj_ = AllOperations(obj_);
    if (!obj_) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> IsList::Do() {
    obj_ = AllOperations(obj_);
    if (!obj_) {
        return std::make_shared<Symbol>("#t");
    }
    if (!Is<Cell>(obj_)) {
        return std::make_shared<Symbol>("#f");
    }
    auto str = obj_->GetString();
    for (auto elem : str) {
        if (elem == '.') {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> List::Do() {
    return As<Cell>(obj_)->GetSecond();
}

std::shared_ptr<Object> Car::Do() {
    obj_ = AllOperations(obj_);
    return As<Cell>(obj_)->GetFirst();
}

std::shared_ptr<Object> Cdr::Do() {
    obj_ = AllOperations(obj_);
    return As<Cell>(obj_)->GetSecond();
}

std::shared_ptr<Object> Cons::Do() {
    if (!Is<Cell>(obj_)) {
        throw RuntimeError("cannot covert to pair");
    }
    if (!As<Cell>(obj_)->GetFirst() || !As<Cell>(obj_)->GetSecond()) {
        throw RuntimeError("cannot covert to pair");
    }

    std::shared_ptr<Object> first = As<Cell>(obj_)->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(obj_)->GetSecond();
    if (As<Cell>(second)->GetSecond()) {
        throw RuntimeError("cannot covert to pair");
    }
    second = As<Cell>(second)->GetFirst();
    auto new_cell = std::make_shared<Cell>(first, second);
    return new_cell;
}

std::shared_ptr<Object> Ref::Do() {
    auto first = As<Cell>(obj_)->GetFirst();
    first = AllOperations(first);
    auto second = As<Cell>(obj_)->GetSecond();
    second = As<Cell>(second)->GetFirst();

    int cnt = 0;
    if (!IsDigit(second->GetString())) {
        throw RuntimeError("Ref with no int");
    }
    while (cnt != std::stoi(second->GetString())) {
        ++cnt;
        first = As<Cell>(first)->GetSecond();
        if (!first) {
            throw RuntimeError("very big ref");
        }
    }
    return As<Cell>(first)->GetFirst();
}

std::shared_ptr<Object> Tail::Do() {
    auto first = As<Cell>(obj_)->GetFirst();
    first = AllOperations(first);
    auto second = As<Cell>(obj_)->GetSecond();
    second = As<Cell>(second)->GetFirst();

    int cnt = 0;
    if (!IsDigit(second->GetString())) {
        throw RuntimeError("Tail with no int");
    }
    while (cnt != std::stoi(second->GetString())) {
        ++cnt;
        first = As<Cell>(first)->GetSecond();
        if (!first && cnt != std::stoi(second->GetString())) {
            throw RuntimeError("very big tail");
        }
    }
    return first;
}
