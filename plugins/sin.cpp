#include "plugin_api.h"
#include <cmath>
#include <stdexcept>

namespace {
double sineInDegrees(double degrees) {
    constexpr double pi = 3.14159265358979323846;
    return std::sin(degrees * pi / 180.0);
}
const CalculatorPlugin descriptor{CALCULATOR_PLUGIN_ABI_VERSION, "sin", sineInDegrees};
}

extern "C" CALCULATOR_PLUGIN_EXPORT const CalculatorPlugin* calculator_plugin_init() { return &descriptor; }
