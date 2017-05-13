#ifndef LOGNORMALRAND_H
#define LOGNORMALRAND_H

#include "ContinuousDistribution.h"
#include "NormalRand.h"

/**
 * @brief The LogNormalRand class <BR>
 * Log-Normal distribution
 *
 * Notation X ~ Log-Normal(μ, σ)
 *
 * Related distributions: <BR>
 * log(X) ~ Normal(μ, σ)
 */
class RANDLIBSHARED_EXPORT LogNormalRand : public ContinuousDistribution
{
    NormalRand X{};
    double expMu = 1; ///< exp(μ)
    double expHalfSigmaSq = 1.6487212707; ///< exp(σ^2 / 2)

public:
    LogNormalRand(double location = 0, double squaredScale = 1);

    std::string Name() const override;
    SUPPORT_TYPE SupportType() const override { return RIGHTSEMIFINITE_T; }
    double MinValue() const override { return 0; }
    double MaxValue() const override { return INFINITY; }

    void SetLocation(double location);
    void SetScale(double scale);
    inline double GetLocation() const { return X.Mean(); }
    inline double GetScale() const { return X.GetScale(); }

    double f(const double & x) const override;
    double logf(const double & x) const override;
    double F(const double & x) const override;
    double S(const double & x) const override;

    static double StandardVariate();
    static double Variate(double location, double scale);
    double Variate() const override;

    double Mean() const override;
    double Variance() const override;
    double Median() const override;
    double Mode() const override;
    double Skewness() const override;
    double ExcessKurtosis() const override;

private:
    double quantileImpl(double p) const override;
    double quantileImpl1m(double p) const override;

    double logAverage(const std::vector<double> &sample);
    double logVariance(const std::vector<double> &sample, double mu);

public:
    /// Method of moments
    void FitLocationMM(const std::vector<double> &sample);
    void FitScaleMM(const std::vector<double> &sample);
    void FitLocationAndScaleMM(const std::vector<double> &sample);

    /// Maximum-likelihod estimation
    void FitLocationMLE(const std::vector<double> &sample);
    void FitScaleMLE(const std::vector<double> &sample);
    void FitLocationAndScaleMLE(const std::vector<double> &sample);

    /// Bayesian estimation
    NormalRand FitLocationBayes(const std::vector<double> &sample, const NormalRand &priorDistribution);
    InverseGammaRand FitScaleBayes(const std::vector<double> &sample, const InverseGammaRand &priorDistribution);
    NormalInverseGammaRand FitLocationAndScaleBayes(const std::vector<double> &sample, const NormalInverseGammaRand &priorDistribution);
};

#endif // LOGNORMALRAND_H
