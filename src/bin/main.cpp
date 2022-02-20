#include <hdrs/zero_polynomial.hpp>
#include <hdrs/coordinate_descent.hpp>

#include <ctime>

#include <algorithm>
#include <unordered_map>
#include <set>

std::vector<TPolynomial> generateAllElementaryConjuctions(uint32_t dimension) {
    TPolynomial multiplierOne(
        dimension,
        { TElementaryConjuction(dimension, { { 0, EState::Positive } }) }
    );

    std::vector<TPolynomial> result;
    result.reserve(binpow(3, dimension));
    for (uint32_t i = 0; i <= dimension; ++i) {
        for (uint32_t j = 0; j <= dimension - i; ++j) {
            std::vector<EMode> permutation;
            permutation.reserve(dimension);
            auto back_insert_iterator = std::back_inserter(permutation);
            std::fill_n(back_insert_iterator, dimension - i - j, EMode::One);
            std::fill_n(back_insert_iterator, j, EMode::Positive);
            std::fill_n(back_insert_iterator, i, EMode::Negative);
            do {
                TPolynomial polynomial = multiplierOne;
                for (uint32_t k = 0; k < dimension; ++k) {
                    TPolynomial multiplierPositive(
                        dimension,
                        { TElementaryConjuction(dimension, { { k + 1, EState::Positive } }) }
                    );
                    TPolynomial multiplierNegative(
                        dimension,
                        { TElementaryConjuction(dimension, { { k + 1, EState::Negative } }) }
                    );

                    switch (permutation[k]) {
                    case EMode::One:
                        polynomial = polynomial * multiplierOne;
                        break;
                    case EMode::Positive:
                    polynomial = polynomial * multiplierPositive;
                        break;
                    case EMode::Negative:
                        polynomial = polynomial * multiplierNegative;
                    default:
                        break;
                    }
                }
                result.push_back(std::move(polynomial));
            } while(std::next_permutation(permutation.begin(), permutation.end()));
        }
    }
    return result;
}

uint32_t valuesVector(const TPolynomial &pol) {
    uint32_t res = 0;
    for (size_t i = 1 << pol.dimension(); i--; res += pol.evaluate(i)) {
        res <<= 1;
    }
    return res;
}

std::map<uint32_t, std::set<TPolynomial>> generateAllPolynomials(uint32_t dimension) {
    std::unordered_map<uint32_t, std::set<TPolynomial>> result;
    std::vector<TPolynomial> elementary_conjuctions = generateAllElementaryConjuctions(dimension);

    uint32_t quantity = elementary_conjuctions.size();
    for (uint32_t i = 0; i < quantity; ++i) {
        std::vector<bool> isIncluded;
        auto back_insert_iterator = std::back_inserter(isIncluded);
        isIncluded.reserve(quantity);
        std::fill_n(back_insert_iterator, i + 1, true);
        std::fill_n(back_insert_iterator, quantity - i - 1, false);
        do {
            TPolynomial polynomial(dimension);

            for (size_t k = 0; k < isIncluded.size(); ++k) {
                if (isIncluded[k]) {
                    polynomial = polynomial + elementary_conjuctions[k];
                }
            }
            uint32_t values_vector = valuesVector(polynomial);
            result[values_vector].insert(std::move(polynomial));
        } while(std::next_permutation(isIncluded.rbegin(), isIncluded.rend()));
    }
    return result;
}

int main() {
    // std::cout << "Input dimension you want work with:" << std::endl;
    uint32_t dimension;
    std::cin >> dimension;
    // std::cout << "Input polynomial you want to shrink:" << std::endl;
    generateAllPolynomials(dimension);
    return 0;
    TPolynomial polynomial(dimension);
    std::cin >> polynomial;

    std::vector<TPolynomial> zeroBasis = generateZeroBasis(polynomial.dimension(), EMode::Positive);
    for (TPolynomial it : zeroBasis) {
        std::cerr << it << '\n';
    }
    TCoordinateDescentMinimizer minimizer(
        polynomial,
        zeroBasis,
        {.pairs=false, .randomize=true, .seed=0}
    );
    TPolynomial shrunk = minimizer.execute();

    std::cout << "Your polynomial has been shrunk to this form:\n" << shrunk << std::endl;
    std::cerr << "It's length equals to " << shrunk.size() << std::endl;
    return 0;
}
