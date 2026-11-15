#include "plugin_api.h"
#include <cmath>
#include <stdexcept>

namespace {
double naturalLogarithm(double value) {
    if (value <= 0.0) throw std::domain_error("ln определён только для положительных чисел");
    return std::log(value);
}
const CalculatorPlugin descriptor{CALCULATOR_PLUGIN_ABI_VERSION, "ln", naturalLogarithm};
}

extern "C" CALCULATOR_PLUGIN_EXPORT const CalculatorPlugin* calculator_plugin_init() { return &descriptor; }
