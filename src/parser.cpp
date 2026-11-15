#include "parser.h"

#include <cmath>
#include <cctype>
#include <cstdlib>
#include <stdexcept>

Parser::Parser(const std::string& input, const PluginRegistry& plugins) : input_(input), plugins_(plugins) {}

double Parser::evaluate() {
    position_ = 0;
    double value = expression();
    skipSpaces();
    if (position_ != input_.size()) error("Лишний символ");
    if (!std::isfinite(value)) error("Результат не является конечным числом");
    return value;
}

double Parser::expression() {
    double value = term();
    while (true) {
        if (consume('+')) value += term();
        else if (consume('-')) value -= term();
        else return value;
    }
}

double Parser::term() {
    double value = unary();
    while (true) {
        if (consume('*')) value *= unary();
        else if (consume('/')) {
            double divisor = unary();
            if (divisor == 0.0) error("Деление на ноль");
            value /= divisor;
        } else return value;
    }
}

double Parser::unary() {
    if (consume('+')) return unary();
    if (consume('-')) return -unary();
    return power();
}

double Parser::power() {
    double base = primary();
    if (consume('^')) {
        double exponent = unary();
        base = std::pow(base, exponent);
        if (!std::isfinite(base)) error("Некорректное возведение в степень");
    }
    return base;
}

double Parser::primary() {
    skipSpaces();
    if (consume('(')) {
        double value = expression();
        expect(')');
        return value;
    }
    skipSpaces();
    if (position_ < input_.size() && (std::isdigit(static_cast<unsigned char>(input_[position_])) || input_[position_] == '.')) {
        const char* start = input_.c_str() + position_;
        char* end = nullptr;
        double value = std::strtod(start, &end);
        if (end == start) error("Ожидалось число");
        position_ += static_cast<std::size_t>(end - start);
        if (!std::isfinite(value)) error("Число вне допустимого диапазона");
        return value;
    }
    if (position_ < input_.size() && std::isalpha(static_cast<unsigned char>(input_[position_]))) {
        std::size_t begin = position_++;
        while (position_ < input_.size() && (std::isalnum(static_cast<unsigned char>(input_[position_])) || input_[position_] == '_')) ++position_;
        std::string name = input_.substr(begin, position_ - begin);
        expect('(');
        double argument = expression();
        expect(')');
        if (!plugins_.contains(name)) error("Неизвестная функция: " + name);
        double result = plugins_.call(name, argument);
        if (!std::isfinite(result)) error("Функция " + name + " вернула некорректный результат");
        return result;
    }
    error("Ожидалось число, функция или скобка");
}

void Parser::skipSpaces() {
    while (position_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[position_]))) ++position_;
}

bool Parser::consume(char character) {
    skipSpaces();
    if (position_ < input_.size() && input_[position_] == character) { ++position_; return true; }
    return false;
}

void Parser::expect(char character) {
    if (!consume(character)) error(std::string("Ожидался символ '") + character + "'");
}

[[noreturn]] void Parser::error(const std::string& message) const {
    throw std::runtime_error(message + " (позиция " + std::to_string(position_ + 1) + ")");
}
