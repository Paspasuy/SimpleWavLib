#include <cmath>

namespace shapes {

    double (*sine)(double) = sin;

    double triangle(double x) {
        x = std::fmod(x, 2 * M_PI);
        if (x < M_PI) {
            return x / M_PI;
        }
        return 2 - x / M_PI;
    }

    double square(double x) {
        x = std::fmod(x, 2 * M_PI);
        if (x < M_PI) {
            return 0;
        }
        return 1;
    }
};