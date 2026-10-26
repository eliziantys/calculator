#pragma once

#include "plugin_api.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    void loadFrom(const std::string& directory);
    double call(const std::string& name, double argument) const;
    bool contains(const std::string& name) const;
    std::size_t size() const noexcept;

private:
    struct Library;
    std::vector<std::unique_ptr<Library>> libraries_;
    std::unordered_map<std::string, CalculatorUnaryFunction> functions_;
};
