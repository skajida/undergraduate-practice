#include <hdrs/zero_polynomial.hpp>

#include <algorithm>

int32_t binpow(int32_t base, int32_t exp) {
   int32_t res = 1;
   while (exp) {
       if (exp & 1) {
           res *= base;
       }
       base *= base;
       exp >>= 1;
   }
   return res;
}

std::vector<TPolynomial> generateZeroBasis(uint32_t dimension, EMode expand) {
    TPolynomial multiplierOne(
        dimension,
        { TElementaryConjuction(dimension, { { 0, EState::Positive } }) }
    );

    std::vector<TPolynomial> result;
    bool mask = expand == EMode::Negative;
    result.reserve(binpow(3 + mask, dimension) - binpow(2 + mask, dimension));
    for (uint32_t i = 0; i < dimension; ++i) {
        for (uint32_t j = 0; j < dimension - i; ++j) {
            for (uint32_t l = 0; l < (mask ? dimension - i - j : 1); ++l) {
                std::vector<EMode> permutation;
                permutation.reserve(dimension);
                auto back_insert_iterator = std::back_inserter(permutation);
                std::fill_n(back_insert_iterator, i + 1, EMode::Zero);
                std::fill_n(back_insert_iterator, dimension - i - 1 - j - mask * l, EMode::One);
                std::fill_n(back_insert_iterator, j, EMode::Positive);
                std::fill_n(back_insert_iterator, mask * l, EMode::Negative);
                do {
                    TPolynomial polynomial = multiplierOne;

                    for (uint32_t k = 0; k < dimension; ++k) {
                        TPolynomial multiplierZero(
                            dimension,
                            {
                                TElementaryConjuction(dimension, { { 0, EState::Positive } }),
                                TElementaryConjuction(dimension, { { k + 1, EState::Positive } }),
                                TElementaryConjuction(dimension, { { k + 1, EState::Negative } })
                            }
                        );
                        TPolynomial multiplierPositive(
                            dimension,
                            { TElementaryConjuction(dimension, { { k + 1, EState::Positive } }) }
                        );
                        TPolynomial multiplierNegative(
                            dimension,
                            { TElementaryConjuction(dimension, { { k + 1, EState::Negative } }) }
                        );

                        switch (permutation[k]) {
                        case EMode::Zero:
                            polynomial = polynomial * multiplierZero;
                            break;
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
                } while (std::next_permutation(permutation.begin(), permutation.end()));
            }
        }
    }
    return result;
}
