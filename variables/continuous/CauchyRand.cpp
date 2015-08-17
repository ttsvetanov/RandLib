﻿#include "CauchyRand.h"

CauchyRand::CauchyRand(double location, double scale)
{
    setLocation(location);
    setScale(scale);
}

void CauchyRand::setName()
{
    nameStr = "Cauchy(" + toStringWithPrecision(getLocation()) + ", " + toStringWithPrecision(getScale()) + ")";
}

void CauchyRand::setLocation(double location)
{
    x0 = location;
    setName();
}

void CauchyRand::setScale(double scale)
{
    gamma = std::max(scale, MIN_POSITIVE);
    gammaInv = 1.0 / gamma;
    setName();
}

double CauchyRand::f(double x) const
{
    double y = x - x0; /// x - x0
    y *= y; /// (x - x0) ^ 2
    y *= gammaInv; /// (x - x0) ^ 2 / gamma
    y += gamma; /// gamma + (x - x0) ^ 2 / gamma
    return M_1_PI / y; /// gamma / (pi * (gamma ^ 2 + (x - x0) ^ 2))
}

double CauchyRand::F(double x) const
{
    double y = x - x0; /// x - x0
    y *= gammaInv; /// (x - x0) / gamma
    y = std::atan(y); /// atan((x - x0) / gamma)
    y *= M_1_PI; /// atan((x - x0) / gamma) / pi
    return y + .5; /// atan((x - x0) / gamma) / pi + 0.5
}

double CauchyRand::variate() const
{
    double x, y;
    do {
        x = UniformRand::variate(-1, 1);
        y = UniformRand::variate(-1, 1);
    } while (x * x + y * y > 1.0 || y == 0.0);
    return x0 + gamma * x / y;
}
