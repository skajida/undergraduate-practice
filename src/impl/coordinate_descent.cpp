#include <hdrs/coordinate_descent.hpp>

#include <algorithm>
#include <numeric>

TCoordinateDescentMinimizer::TCoordinateDescentMinimizer(
    TPolynomial init,
    const std::vector<TPolynomial> &basis,
    TParams parameters
)   : current_(std::move(init))
    , basis_(basis)
    , traversal_sequence_([&basis, &parameters] () {
        auto &[pairs, randomize, seed] = parameters;
        std::vector<size_t> res(basis.size());
        std::iota(res.begin(), res.end(), 0ul);
        if (randomize) {
            std::mt19937 gen(seed);
            std::shuffle(res.begin(), res.end(), gen);
        }
        return res;
    }())
    , pairs_(parameters.pairs) {}

bool TCoordinateDescentMinimizer::add_singles_lap() {
    bool res = false;
    for (size_t idx : traversal_sequence_) {
        TPolynomial sum(current_ + basis_[idx]);
        if (sum < current_) {
            res = true;
            std::cerr << "MATCH:\t" << basis_[idx] << std::endl;
            current_ = sum;
        }
    }
    return res;
}

bool TCoordinateDescentMinimizer::add_pairs_lap() {
    bool res = false;
    for (size_t i = 0; i < traversal_sequence_.size(); ++i) {
        for (size_t j = i + 1; j < traversal_sequence_.size(); ++j) {
            TPolynomial item(basis_[traversal_sequence_[i]] + basis_[traversal_sequence_[j]]);
            TPolynomial sum(current_ + item);
            if (sum < current_) {
                res = true;
                std::cerr << "DMATCH:\t" << item << std::endl;
                current_ = sum;
                return true;
            }
        }
    }
    return res;
}

TPolynomial TCoordinateDescentMinimizer::execute() {
    while ([this] () {
        bool res = false;
        size_t prev_size = current_.size();
        if (pairs_) {
            while (add_pairs_lap());
            res |= current_.size() < prev_size;
        }
        prev_size = current_.size();
        while (add_singles_lap());
        res |= current_.size() < prev_size;
        return res;
    }());
    return current_;
}
