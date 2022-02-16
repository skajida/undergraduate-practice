#include <hdrs/zero_polynomial.hpp>

#include <algorithm>
#include <sstream>

void print_eval(uint32_t dim, const TPolynomial &pol) {
    for (size_t i = 1 << dim; i--; std::cout << pol.evaluate(i));
    std::cout << std::endl;
}

int main(int argc, char const *argv[]) {
    // std::cout << "Input dimension you want work with:" << std::endl;
    if (argc < 2) {
        return 0;
    }
    std::istringstream in(argv[1]);
    uint32_t dimension;
    in >> dimension;
    // std::cout << "Input polynomial you want to shrink:" << std::endl;
    // TPolynomial polynomial(dimension);
    // std::cin >> polynomial;

    std::vector<TPolynomial> basis = generateZeroBasis(dimension);
    std::cerr << basis.size() << std::endl;
    // bool flag;
    // do {
    //     flag = false;
    //
    //     // if (argc == 1) {
    //     //     for (size_t i = 0; i < basis.size(); ++i) {
    //     //         for (size_t j = i + 1; j < basis.size(); ++j) {
    //     //             TPolynomial item(basis[i] + basis[j]);
    //     //             TPolynomial sum(polynomial + item);
    //     //             if (sum < polynomial) {
    //     //                 flag = true;
    //     //                 std::cerr << "DMATCH:\t" << item << std::endl;
    //     //                 polynomial = sum;
    //     //             }
    //     //         }
    //     //     }
    //     // }
    //
    //     for (const TPolynomial &item : basis) {
    //         TPolynomial sum(polynomial + item);
    //         if (sum < polynomial) {
    //             flag = true;
    //             std::cerr << "MATCH:\t" << item << std::endl;
    //             polynomial = sum;
    //         }
    //     }
    // } while (flag);
    // std::cout << "Your polynomial has been shrinken to this form:\n" << polynomial << std::endl;
    return 0;
}
