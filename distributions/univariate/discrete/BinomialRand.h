#ifndef BINOMIALRAND_H
#define BINOMIALRAND_H

#include "DiscreteDistribution.h"
#include "GeometricRand.h"
#include "../continuous/BetaRand.h"

/**
 * @brief The BinomialDistribution class <BR>
 * Abstract class for Binomial distribution
 *
 * Notation: X ~ Bin(n, p)
 *
 * Related distributions:
 * if X ~ Bin(1, p), then X ~ Bernoulli(p)
 */
class RANDLIBSHARED_EXPORT BinomialDistribution : public DiscreteDistribution
{
protected:
    double p = 1; ///< probability of success
    double q = 0; ///< probability of failure
    double logProb = 0; ///< log(p)
    double log1mProb = -INFINITY; ///< log(q)

private:
    int n = 1; ///< number of experiments
    double np = 1; ///< n * p
    double lgammaNp1 = 0; ///< log(n!)

    double delta1{}, delta2{};
    double sigma1{}, sigma2{}, c{};
    double a1{}, a2{}, a3{}, a4{};
    double coefa3{}, coefa4{};

    double minpq = 0; ///< min(p, q)
    double pFloor = 0; ///< [n * min(p, q)] / n
    double logPFloor = -INFINITY; ///< log(pFloor)
    double logQFloor = 0; ///< log(1 - pFloor)
    double pRes = 0; ///< min(p, q) - pFloor
    double npFloor = 0; ///< [n * min(p, q)]
    double nqFloor = n; ///< [n * max(p, q)]
    double logPnpInv = -INFINITY; ///< log(P([npFloor))

    GeometricRand G{};

public:
    BinomialDistribution(int number, double probability);
    SUPPORT_TYPE SupportType() const override { return FINITE_T; }
    int MinValue() const override { return 0; }
    int MaxValue() const override { return n; }

private:
    void SetGeneratorConstants();

protected:
    void SetParameters(int number, double probability);

public:
    inline double GetNumber() const { return n; }
    inline double GetProbability() const { return p; }

private:
    /**
     * @fn logProbFloor
     * @param k
     * @return logarithm of probability to get k if p = pFloor
     */
    double logProbFloor(int k) const;

public:
    double P(const int & k) const override;
    double logP(const int & k) const override;
    double F(const int & k) const override;
    double S(const int & k) const override;

private:
    enum GENERATOR_ID {
        BERNOULLI_SUM,
        WAITING,
        REJECTION
    };

    GENERATOR_ID GetIdOfUsedGenerator() const;

    int variateRejection() const;
    int variateWaiting(int number) const;
    static int variateBernoulliSum(int number, double probability);

public:
    int Variate() const override;
    static int Variate(int number, double probability);
    void Sample(std::vector<int> &outputData) const override;

    double Mean() const override;
    double Variance() const override;
    int Median() const override;
    int Mode() const override;
    double Skewness() const override;
    double ExcessKurtosis() const override;

    inline double GetLogFactorialN() const { return lgammaNp1; }

private:
    std::complex<double> CFImpl(double t) const override;

public:
    /**
     * @fn FitProbabilityMLE
     * Fit probability p with maximum-likelihood estimation
     * @param sample
     */
    void FitProbabilityMLE(const std::vector<int> &sample);

    /**
     * @fn FitProbabilityMM
     * Fit probability p, using method of moments
     * @param sample
     */
    void FitProbabilityMM(const std::vector<int> &sample);

    /**
     * @fn FitProbabilityBayes
     * Fit probability p with prior assumption p ~ Beta(α, β)
     * @param sample
     * @param priorDistribution
     * @return posterior distribution
     */
    BetaRand FitProbabilityBayes(const std::vector<int> &sample, const BetaDistribution & priorDistribution);

    /**
     * @fn FitProbabilityMinimax
     * Fit probability p with minimax estimator
     * @param sample
     * @return posterior distribution
     */
    BetaRand FitProbabilityMinimax(const std::vector<int> &sample);
};


/**
 * @brief The BinomialRand class <BR>
 * Binomial distribution
 */
class RANDLIBSHARED_EXPORT BinomialRand : public BinomialDistribution
{
public:
    BinomialRand(int number, double probability) : BinomialDistribution(number, probability) {}
    std::string Name() const override;
    using BinomialDistribution::SetParameters;
};


#endif // BINOMIALRAND_H
