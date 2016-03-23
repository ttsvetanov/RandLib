#include "WignerSemicircleRand.h"

WignerSemicircleRand::WignerSemicircleRand(double radius)
{
    setRadius(radius);
}

std::string WignerSemicircleRand::name()
{
    return "Wigner Semicircle(" + toStringWithPrecision(getRadius()) + ")";
}

void WignerSemicircleRand::setRadius(double radius)
{
    R = radius;
    if (R <= 0)
        R = 1.0;
    RSq = R * R;
    X.setShapes(1.5, 1.5);
}

double WignerSemicircleRand::f(double x) const
{
    double xSq = x * x;
    if (xSq >= RSq)
        return 0.0;
    double y = RSq - xSq;
    y = std::sqrt(y);
    y *= M_1_PI / RSq;
    return y + y;
}

double WignerSemicircleRand::F(double x) const
{
    if (x <= -R)
        return 0.0;
    if (x >= R)
        return 1.0;
    double y = RSq - x * x;
    y = x * std::sqrt(y) / RSq;
    double z = std::asin(x / R);
    return 0.5 + (y + z) * M_1_PI;
}

double WignerSemicircleRand::variate() const
{
    double x = X.variate();
    x += x - 1;
    return R * x;
}

double WignerSemicircleRand::Mean() const
{
    return 0.0;
}

double WignerSemicircleRand::Variance() const
{
    return 0.25 * RSq;
}

double WignerSemicircleRand::Median() const
{
    return 0.0;
}

double WignerSemicircleRand::Mode() const
{
    return 0.0;
}

double WignerSemicircleRand::Skewness() const
{
    return 0.0;
}

double WignerSemicircleRand::ExcessKurtosis() const
{
    return -1.0;
}

double WignerSemicircleRand::Entropy() const
{
    return std::log(M_PI * R) - 0.5;
}