#include "RandMath.h"

constexpr long double RandMath::factorialTable[];
constexpr double RandMath::logFactorialTable[];

RandMath::RandMath()
{
}

long double RandMath::stirlingFactorial(int n)
{
    long double fact = M_SQRT2PI * std::sqrt(static_cast<double>(n));
    fact *= std::pow(n / M_E, n);
    double secondCoef = 1.0 / (12.0 * n);
    double thirdCoef = 0.5 * secondCoef * secondCoef;
    return std::round((1.0 + secondCoef + thirdCoef) * fact);
}

long double RandMath::stirlingLogFactorial(int n)
{
    long double fact = n * std::log(n) - n;
    double piN = M_PI * n;
    fact += 0.5 * std::log(piN + piN);
    fact += 1.0 / (12.0 * n);
    return std::round(fact);
}

long double RandMath::fastFactorial(int n)
{
    if (n < 0)
        return 0;

    if (n > 255)
        return stirlingFactorial(n);

    int residue = n % 10;
    if (residue <= 5)
    {
        /// go up
        int nPrev = n - residue;
        long double fact = factorialTable[nPrev / 10];
        for (int i = 1; i <= residue; ++i)
            fact *= nPrev + i;
        return fact;
    }
    else
    {
        /// go  down
        int nNext = n - residue + 10;
        double denominator = 1;
        for (int i = 0; i < 10 - residue; ++i)
            denominator *= nNext - i;
        return factorialTable[nNext / 10] / denominator;
    }
}

long double RandMath::fastLogFactorial(int n)
{
    if (n <= 1)
        return 0;

    if (n > 255)
        return stirlingLogFactorial(n);

   return logFactorialTable[n - 2];
}

long double RandMath::fastDoubleFactorial(int n)
{
    long double n_fact = fastFactorial(n);
    if (n & 1) {
        n <<= 1;
        return fastFactorial(n + 1) / (n * n_fact);
    }
    return (1 << n) * n_fact;
}

long double RandMath::binomialCoef(int n, int k)
{
    long double n_fact = fastFactorial(n);
    long double k_fact = fastFactorial(k);
    long double k_n_fact = fastFactorial(n - k);
    return n_fact / (k_fact * k_n_fact);
}

long double RandMath::lowerIncGamma(double a, double x)
{
    double sum = 0;
    double term = 1.0 / a;
    int n = 1;
    while (std::fabs(term) > MIN_POSITIVE)
    {
        sum = sum + term;
        term *= (x / (a + n));
        ++n;
    }
    return std::pow(x, a) * std::exp(-x) * sum;
}

long double RandMath::upperIncGamma(double a, double x)
{
    // TODO: find useful approximation
    return std::tgamma(a) - lowerIncGamma(a, x);
}

long double RandMath::betaFun(double x, double y)
{
    if (x > y)
    {
        long double res = std::tgamma(x);
        res /= std::tgamma(x + y);
        return res * std::tgamma(y);
    }
    else
    {
        long double res = std::tgamma(y);
        res /= std::tgamma(x + y);
        return res * std::tgamma(x);
    }
}

long double RandMath::gammaHalf(int k)
{
    if (k < 0)
        return 0;

    if (k & 1)
    {
        int n = (k - 1) >> 1;
        long double res = fastFactorial(k - 1);
        res /= (fastFactorial(n) * (1 << (n << 1)));
        return res * M_SQRTPI;
    }

    return fastFactorial((k >> 1) - 1);
}

long double RandMath::erfInv(double p)
{
    if (p < 0.5)
        return -erfInv(1 - p);
    if (p <= 0)
        return -INFINITY;
    if (p >= 1)
        return INFINITY;
    double t = M_SQRT2 * std::sqrt(-std::log(p));
    static constexpr double c[] = {2.515517, 0.802853, 0.010328};
    static constexpr double d[] = {1.432788, 0.189269, 0.001308};
    long double numerator = (c[2] * t + c[1]) * t + c[0];
    long double denominator = ((d[2] * t + d[1]) * t + d[0]) * t + 1.0;
    return t - numerator / denominator;
}

long double RandMath::erfcinv(double p)
{
    return erfInv(1 - p);
}

long double RandMath::adaptiveSimpsonsAux(const std::function<double (double)> &funPtr, double a, double b,
                                          double epsilon, double S, double fa, double fb, double fc, int bottom)
{
    // TODO: rewrite recursion into loop
    double c = .5 * (a + b), h = (b - a) / 12.0;
    double d = .5 * (a + c), e = .5 * (c + b);
    double fd = funPtr(d), fe = funPtr(e);
    double Sleft = h * (fa + 4 * fd + fc);
    double Sright = h * (fc + 4 * fe + fb);
    double S2 = Sleft + Sright;
    if (bottom <= 0 || std::fabs(S2 - S) <= 15.0 * epsilon)
        return S2 + (S2 - S) / 15.0;
    epsilon *= .5;
    --bottom;

    return adaptiveSimpsonsAux(funPtr, a, c, epsilon, Sleft, fa, fc, fd, bottom) +
            adaptiveSimpsonsAux(funPtr, c, b, epsilon, Sright, fc, fb, fe, bottom);
}

long double RandMath::integral(const std::function<double (double)> funPtr,
                               double a, double b, double epsilon, int maxRecursionDepth)
{
    double c = .5 * (a + b), h = (b - a) / 6.0;
    double fa = funPtr(a), fb = funPtr(b), fc = funPtr(c);
    double S = h * (fa + 4 * fc + fb);
    return adaptiveSimpsonsAux(funPtr, a, b, epsilon, S, fa, fb, fc, maxRecursionDepth);
}

bool RandMath::findRoot(const std::function<double (double)> &funPtr, double a, double b, double &root, double epsilon)
{
    double fa = funPtr(a);
    if (fa == 0)
    {
        root = a;
        return true;
    }
    double fb = funPtr(b);
    if (fb == 0)
    {
        root = b;
        return true;
    }
    if (fa * fb > 0)
        return false; /// error - the root is not bracketed
    if (std::fabs(fa) < std::fabs(fb))
    {
        SWAP(a, b);
        SWAP(fa, fb);
    }
    double c = a, fc = fa;
    bool mflag = true;
    double s = b, fs = 1, d = 0;
    while (std::fabs(b - a) > epsilon)
    {
        if (std::fabs(fc - fa) > MIN_POSITIVE &&
            std::fabs(fb - fc) > MIN_POSITIVE)
        {
            /// inverse quadratic interpolation
            double numerator = a * fb * fc;
            double denominator = (fa - fb) * (fa - fc);
            s = numerator / denominator;

            numerator = b * fa * fc;
            denominator = (fb - fa) * (fb - fc);
            s += numerator / denominator;

            numerator = c * fa * fb;
            denominator = (fc - fa) * (fc - fb);
            s += numerator / denominator;
        }
        else
        {
            /// secant method
            s = b - fb * (b - a) / (fb - fa);
        }

        if (s < 0.25 * (3 * a + b) || s > b ||
            (mflag && std::fabs(s - b) >= 0.5 * std::fabs(b - c)) ||
            (!mflag && std::fabs(s - b) >= 0.5 * std::fabs(d - c)) ||
            (mflag && std::fabs(b - c) < epsilon) ||
            (!mflag && std::fabs(c - d) < epsilon))
        {
            s = 0.5 * (a + b);
            mflag = true;
        }
        else
            mflag = false;

        fs = funPtr(s);
        if (std::fabs(fs) < epsilon)
        {
            root = s;
            return true;
        }

        d = c;
        c = b;
        fc = fb;

        if (fa * fs < 0)
        {
            b = s;
            fb = fs;
        }
        else
        {
            a = s;
            fa = fs;
        }

        if (std::fabs(fa) < std::fabs(fb))
        {
            SWAP(a, b);
            SWAP(fa, fb);
        }
    }

    root = (std::fabs(fs) < std::fabs(fb)) ? s : b;
    return true;
}

double RandMath::linearInterpolation(double a, double b, double fa, double fb, double x)
{
    if (b == a)
        return fa;

    double fx = x - a;
    fx /= (b - a);
    fx *= (fb - fa);
    return fx + fa;
}


