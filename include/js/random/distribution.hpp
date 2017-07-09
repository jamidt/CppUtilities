/*
CppUtility library
Copyright (C) 2016  Jan Schmidt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <array>
#include <iostream>
#include <random>

namespace js {

template <std::size_t, class, class>
class MultinomialDistribution;

namespace detail {

/**@brief Parameter set for multlinomial distribution
 *
 * This class provides the parameter set for the multinomial
 * distribution.
 */
template <std::size_t N, class IntType, class RealType>
class ParamMultinomial {
  private:
    IntType _trials;
    std::array<RealType, N> _prob;
    friend MultinomialDistribution<N, IntType, RealType>;

  public:
    using distribution_type = MultinomialDistribution<N, IntType, RealType>;

    /**@name Constructor
     *@{
     */
    ParamMultinomial() = delete;
    /**@brief Create parameters
     *
     * @param trials Number of trials
     * @param prob Array with probabilities
     */
    ParamMultinomial(IntType trials, const std::array<RealType, N>& prob)
        : _trials(trials), _prob(prob) {}
    /// Copy constructor
    template <class IntType2, class RealType2>
    ParamMultinomial(const ParamMultinomial<N, IntType2, RealType2>& p)
        : _trials(p._trials), _prob(p._prob) {}
    /**@}
     */
    /**@name Operators
     *@{
     */
    /// Assignment operator
    ParamMultinomial<N, IntType, RealType>& operator=(
        ParamMultinomial<N, IntType, RealType>& p) {
        _trials = p._trials;
        _prob = p._prob;
        return *this;
    }
    friend bool operator==(const ParamMultinomial& p1,
                           const ParamMultinomial& p2) {
        return p1._trials == p2._trials && p1._prob == p2._prob;
    }
    friend bool operator!=(const ParamMultinomial& p1,
                           const ParamMultinomial& p2) {
        return !(p1 == p2);
    }
    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(
        std::basic_ostream<CharT, Traits>& os, const ParamMultinomial& p) {
        os << p._trials;
        for (const auto& x : p._prob) {
            os << " " << x;
        }
        return os;
    }
    template <class CharT, class Traits>
    friend std::basic_istream<CharT, Traits>& operator>>(
        std::basic_istream<CharT, Traits>& is, ParamMultinomial& p) {
        is >> p._trials;
        for (std::size_t i = 0; i != N; ++i) {
            is >> std::ws >> p._prob[i];
        }
        return is;
    }
    /**@}
     */
};

}  // end namespace detail

/**@ingroup random
 * @brief Multinomial random number distribution
 *
 * Implementation follows "A review on MC simulation methods [...]" Mode et. al.
 * 2008, Math. Biosci 211 http://dx.doi.org/10.1016/j.mbs.2007.05.015
 *
 * This is supposed to fulfill the random number distribution concept.
 * http://en.cppreference.com/w/cpp/concept/RandomNumberDistribution
 *
 * Note that strictly speaking this distribution cannot fulfill the
 * standard because result_type is not arithmetic but this class should
 * be as close as possible.
 */
template <std::size_t N, class IntType = int, class RealType = double>
class MultinomialDistribution {
  public:
    using param_type = detail::ParamMultinomial<N, IntType, RealType>;
    using result_type = std::array<IntType, N>;

  private:
    param_type _p;

  public:
    /**@name Constructor
     *@{
     */
    /// Construct via trials and probability array
    MultinomialDistribution(IntType trials, const std::array<RealType, N>& prob)
        : _p(trials, prob) {}
    /// Default constructor
    MultinomialDistribution()
        : MultinomialDistribution(
              1, std::array<RealType, N>{}.fill(1 / static_cast<RealType>(N))) {
    }
    MultinomialDistribution(const param_type& p) : _p(p) {}
    /// Copy constructor
    MultinomialDistribution(const MultinomialDistribution& dist2)
        : _p(dist2._p) {}
    /// Copy assignment
    MultinomialDistribution& operator=(const MultinomialDistribution& dist2) {
        _p = dist2._p;
        return *this;
    }
    /**@}
     */

    /**@name Parameter
     *@{
     */
    param_type param() const { return _p; }
    void param(const param_type& p) { _p = p; }
    void reset() {}
    /**@}*/
    ///@name Get random numbers
    /**@{*/
    /**@brief Get random numbers from generator and parameters
     *
     * @param g Random number generator fulfilling uniform random bit
     * generator concept
     * @param p Parameters
     *
     * The input parameters are used for the calculation and do not
     * change the internal parameter set of the class.
     */
    template <class UniformRandomBitGenerator>
    result_type operator()(UniformRandomBitGenerator& g,
                           const param_type& p) const {
        result_type output;
        IntType trials_next = p._trials;
        RealType prob_used = 1;
        for (std::size_t i = 0; i != N; ++i) {
            std::binomial_distribution<IntType> binom_dist(
                trials_next, p._prob[i] / prob_used);
            output[i] = binom_dist(g);
            prob_used -= p._prob[i];
            trials_next -= output[i];
        }
        return output;
    }
    /**@brief Get random numbers from generator
     *
     * @param g Random number generator fulfilling uniform random bit
     *
     * The parameters of the class are used for the calculation.
     */
    template <class UniformRandomBitGenerator>
    result_type operator()(UniformRandomBitGenerator& g) {
        return operator()(g, _p);
    }
    /**@}*/

    /**@name Characteristics
     * @{
     */
    /**@brief Get minimal values in array
     *
     * Note that for calculations this should not be used. It is mereley
     * a function to comply with the standard. `min_val()` should be
     * used instead.
     */
    result_type min() const {
        std::array<IntType, N> out;
        out.fill(std::binomial_distribution<IntType>{}.min());
        return out;
    }
    /// Get minimal value as IntType
    IntType min_val() const {
        return std::binomial_distribution<IntType>{}.min();
    }
    /**@brief Get maximal values in array
     *
     * Note that for calculations this should not be used. It is mereley
     * a function to comply with the standard. `max_val()` should be
     * used instead.
     */
    result_type max() const {
        std::array<IntType, N> out;
        out.fill(std::binomial_distribution<IntType>{}.max());
        return out;
    }
    /// Get maximal value as IntType
    IntType max_val() const {
        return std::binomial_distribution<IntType>{}.max();
    }
    /**@}
     */

    /**@name Comparators
     * @{
     */
    friend bool operator==(const MultinomialDistribution& dist1,
                           const MultinomialDistribution& dist2) {
        return dist1._p == dist2._p;
    }
    friend bool operator!=(const MultinomialDistribution& dist1,
                           const MultinomialDistribution& dist2) {
        return dist1._p != dist2._p;
    }
    /**@}
     */

    /**@name Streamoperator
     * @{
     */
    /// Put parameters
    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(
        std::basic_ostream<CharT, Traits>& os,
        const MultinomialDistribution& dist) {
        os << dist._p;
        return os;
    }
    /// Get parameters
    template <class CharT, class Traits>
    friend std::basic_istream<CharT, Traits>& operator>>(
        std::basic_istream<CharT, Traits>& is, MultinomialDistribution& dist) {
        is >> dist._p;
        return is;
    }
    /**@}
     */
};

}  // End namespace js
