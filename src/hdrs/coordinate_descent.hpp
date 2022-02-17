#pragma once

#include <hdrs/polynomial.hpp>

#include <random>

struct TParams {
    bool pairs;
    bool randomize;
    int64_t seed;
};

class TCoordinateDescentMinimizer {
private:
    TPolynomial current_;
    const std::vector<TPolynomial> &basis_;
    const std::vector<size_t> traversal_sequence_;

    const bool pairs_;

    bool add_singles_lap();
    bool add_pairs_lap();

public:
    TCoordinateDescentMinimizer(
        TPolynomial,
        const std::vector<TPolynomial>&,
        TParams={.pairs=false, .randomize=false, .seed=0}
    );

    TPolynomial execute();
};
