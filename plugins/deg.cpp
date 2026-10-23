#include "plugin_api.h"
#include <cmath>
#include <stdexcept>

namespace {
double radiansToDegrees(double radians) {
    constexpr double pi = 3.14159265358979323846;
    if (!std::isfinite(radians)) throw std::domain_error("deg ожидает конечное число");
    return radians * 180.0 / pi;
}
const CalculatorPlugin descriptor{CALCULATOR_PLUGIN_ABI_VERSION, "deg", radiansToDegrees};
}

extern "C" CALCULATOR_PLUGIN_EXPORT const CalculatorPlugin* calculator_plugin_init() { return &descriptor; }
