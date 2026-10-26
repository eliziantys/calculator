#include "parser.h"

#include <iomanip>
#include <iostream>
#include <string>

int main() {
    PluginRegistry plugins;
    plugins.loadFrom("plugins");

    std::cout << "Калькулятор. Операции: + - * / ^ и скобки. Для выхода введите exit.\n";
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;
        try {
            Parser parser(line, plugins);
            std::cout << std::setprecision(15) << parser.evaluate() << '\n';
        } catch (const std::exception& ex) {
            std::cerr << "Ошибка: " << ex.what() << '\n';
        } catch (...) {
            std::cerr << "Ошибка: плагин выбросил неизвестное исключение\n";
        }
    }
    return 0;
}
