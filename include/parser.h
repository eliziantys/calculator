#pragma once

#include "plugin_loader.h"

#include <string>

class Parser {
public:
    Parser(const std::string& input, const PluginRegistry& plugins);
    double evaluate();

private:
    const std::string& input_;
    const PluginRegistry& plugins_;
    std::size_t position_ = 0;

    double expression();
    double term();
    double unary();
    double power();
    double primary();
    void skipSpaces();
    bool consume(char character);
    void expect(char character);
    [[noreturn]] void error(const std::string& message) const;
};
