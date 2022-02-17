#include <hdrs/zero_polynomial.hpp>

#include <algorithm>
#include <numeric>
#include <random>
#include <ctime>

// debug function
// void print_eval(uint32_t dim, const TPolynomial &pol) {
//     for (size_t i = 1 << dim; i--; std::cout << pol.evaluate(i));
//     std::cout << std::endl;
// }

/*
 * improvements
 * <test4   seed:6 size:5
 * <test8   seed:0 size:8
*/

struct TParams {
    bool randomize;
    int64_t seed;
};

TPolynomial minimize_polynom(
    TPolynomial initial,
    const std::vector<TPolynomial> &zeroBasis,
    TParams parameters={.randomize=false, .seed=0}
) {
    auto [randomize, seed] = parameters;
    std::vector<size_t> traversal_sequence(zeroBasis.size());
    std::iota(traversal_sequence.begin(), traversal_sequence.end(), 0ul);

    std::mt19937 generator(seed);
    if (randomize) {
        std::shuffle(traversal_sequence.begin(), traversal_sequence.end(), generator);
    }

    bool flag;
    do {
        flag = false;
        for (size_t idx : traversal_sequence) {
            TPolynomial sum(initial + zeroBasis[idx]);
            if (sum < initial) {
                flag = true;
                // std::cerr << "MATCH:\t" << zeroBasis[idx] << std::endl;
                initial = sum;
            }
        }
    } while (flag);

    return initial;
}

int main() {
    std::cout << "Input dimension you want work with:" << std::endl;
    uint32_t dimension;
    std::cin >> dimension;
    std::cout << "Input polynomial you want to shrink:" << std::endl;
    TPolynomial polynomial(dimension);
    std::cin >> polynomial;

    std::vector<TPolynomial> basis = generateZeroBasis(polynomial.dimension(), EMode::Negative);
    size_t min_size = 100;
    for (ssize_t seed = 0; 2 < min_size; ++seed) {
        TPolynomial shrunk = minimize_polynom(polynomial, basis, {.randomize=true, .seed=seed});
        if (shrunk.size() < min_size) {
            min_size = shrunk.size();
            std::cerr << "seed:" << seed << "\tsize:" << min_size << std::endl;
        }
    }

    std::cout << "Your polynomial has been shrunk to this form:\n" << shrunk << std::endl;
    std::cerr << "It's length equals to " << shrunk.size() << std::endl;
    return 0;
}
