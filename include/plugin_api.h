#pragma once

// Stable C-compatible boundary between the calculator and its plugins.
// Each plugin exports only calculator_plugin_init; its operation stays private.
#ifdef _WIN32
#  ifdef CALCULATOR_PLUGIN_BUILD
#    define CALCULATOR_PLUGIN_EXPORT __declspec(dllexport)
#  else
#    define CALCULATOR_PLUGIN_EXPORT
#  endif
#else
#  define CALCULATOR_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
using CalculatorUnaryFunction = double (*)(double);

struct CalculatorPlugin {
    unsigned int abi_version;
    const char* name;
    CalculatorUnaryFunction function;
};

using CalculatorPluginInit = const CalculatorPlugin* (*)();

CALCULATOR_PLUGIN_EXPORT const CalculatorPlugin* calculator_plugin_init();
}

constexpr unsigned int CALCULATOR_PLUGIN_ABI_VERSION = 1;
