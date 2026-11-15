#include "plugin_api.h"
#include <cmath>
#include <stdexcept>

namespace {
double exponential(double value) {
    const double result = std::exp(value);
    if (!std::isfinite(result)) throw std::overflow_error("exp: результат вне диапазона");
    return result;
}
const CalculatorPlugin descriptor{CALCULATOR_PLUGIN_ABI_VERSION, "exp", exponential};
}

extern "C" CALCULATOR_PLUGIN_EXPORT const CalculatorPlugin* calculator_plugin_init() { return &descriptor; }
