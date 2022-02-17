#include <hdrs/zero_polynomial.hpp>
#include <hdrs/coordinate_descent.hpp>

#include <ctime>

int main() {
    // std::cout << "Input dimension you want work with:" << std::endl;
    uint32_t dimension;
    std::cin >> dimension;
    // std::cout << "Input polynomial you want to shrink:" << std::endl;
    TPolynomial polynomial(dimension);
    std::cin >> polynomial;

    std::vector<TPolynomial> zeroBasis = generateZeroBasis(polynomial.dimension(), EMode::Negative);
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
