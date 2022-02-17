#include <hdrs/polynomial.hpp>

#include <algorithm>
#include <sstream>

TPolynomial::iterator TPolynomial::begin() noexcept {
    return polynomial_.begin();
}

TPolynomial::iterator TPolynomial::end() noexcept {
    return polynomial_.end();
}

void TPolynomial::clear() {
    polynomial_.clear();
}

void TPolynomial::normalize() {
    std::for_each(begin(), end(), [](TElementaryConjuction &conj) {
        conj.normalize();
        conj.setState(false);
    });
    polynomial_.erase(std::remove(
        begin(),
        end(),
        TElementaryConjuction(dim_, { { 0, EState::Empty } })
    ), end());
    std::sort(begin(), end());

    if (begin() == end()) {
        polynomial_.push_back(TElementaryConjuction(dim_, { { 0, EState::Empty } }));
    } else {
        bool negative = false;
        for (size_t i = 1; i < polynomial_.size(); ++i) {
            if (polynomial_[i - 1].isInversed(polynomial_[i])) {
                // cannot generate zero polynomials cause of that
                // polynomial_[i - 1].setState(true);
                // polynomial_[i].setState(true);
                // negative = !negative;
            } else if (polynomial_[i - 1] == polynomial_[i]) {
                polynomial_[i - 1].setState(true);
                polynomial_[i].setState(true);
            }
        }
        auto it = std::stable_partition(
            begin(),
            end(),
            [] (const TElementaryConjuction &obj) {
                return !obj.getState();
            }
        );
        polynomial_.erase(it, end());
        if (negative) {
            TElementaryConjuction one = TElementaryConjuction(dim_, { { 0, EState::Positive } });
            auto [lhs, rhs] = std::equal_range(
                begin(),
                end(),
                one
            );
            if (lhs != rhs) {
                polynomial_.erase(lhs);
                if (begin() == end()) {
                    polynomial_.push_back(TElementaryConjuction(dim_, { { 0, EState::Empty } }));
                }
            } else {
                polynomial_.insert(lhs, one);
            }
        }
    }
}

TPolynomial::TPolynomial(uint32_t dim) : dim_(dim), polynomial_() { normalize(); }

TPolynomial::TPolynomial(const TPolynomial &obj)
    : dim_(obj.dim_)
    , polynomial_(obj.polynomial_)
{
}

TPolynomial::TPolynomial(TPolynomial &&obj)
    : dim_(obj.dim_)
    , polynomial_(std::move(obj.polynomial_))
{
}

TPolynomial::TPolynomial(uint32_t dim, std::vector<TElementaryConjuction> polyn)
    : dim_(dim)
    , polynomial_(polyn)
{
    normalize();
}

bool TPolynomial::evaluate(uint32_t vector) const {
    vector &= (1 << dim_) - 1;
    bool result = false;
    for (const TElementaryConjuction &it : polynomial_) {
        result ^= it.evaluate(vector);
    }
    return result;
}

std::string TPolynomial::vector() const {
    std::string result;
    result.reserve(1 << dim_);
    for (uint32_t i = 1 << dim_; i--; result.push_back('0' + evaluate(i)));
    return result;
}

TPolynomial& TPolynomial::operator=(const TPolynomial &obj) {
    if (dim_ != obj.dim_) {
        throw std::logic_error("dimensions of operands doesn't match");
    }
    if (this != &obj) {
        polynomial_ = obj.polynomial_;
    }
    return *this;
}

TPolynomial TPolynomial::operator+(const TPolynomial &obj) const {
    if (this == &obj) {
        return TPolynomial(dim_);
    }
    if (dim_ != obj.dim_) {
        throw std::logic_error("dimensions of operands doesn't match");
    }
    TPolynomial result(dim_);
    std::merge(
        begin(),
        end(),
        obj.begin(),
        obj.end(),
        std::back_inserter(result.polynomial_)
    );
    result.normalize();
    return result;
}

bool TPolynomial::operator<(const TPolynomial &obj) const {
    return size() < obj.size();
}

TPolynomial TPolynomial::operator*(const TElementaryConjuction &obj) const {
    TPolynomial result(*this);
    std::for_each(result.begin(), result.end(), [&obj](TElementaryConjuction &it) { it *= obj; });
    return result;
}

TPolynomial TPolynomial::operator*(const TPolynomial &obj) const {
    TPolynomial result(dim_);
    result.polynomial_.reserve(polynomial_.size() * obj.polynomial_.size());
    for (const TElementaryConjuction &conj : obj) {
        std::copy(begin(), end(), std::back_inserter(result.polynomial_));
        std::for_each(
            result.polynomial_.end() - polynomial_.size(),
            result.polynomial_.end(),
            [&conj](TElementaryConjuction &it) { it *= conj; }
        );
    }
    result.normalize();
    return result;
}

TPolynomial::const_iterator TPolynomial::begin() const noexcept {
    return polynomial_.begin();
}

TPolynomial::const_iterator TPolynomial::end() const noexcept {
    return polynomial_.end();
}

uint32_t TPolynomial::dimension() const noexcept {
    return dim_;
}

uint32_t TPolynomial::size() const noexcept {
    return polynomial_.size();
}

std::istream& operator>>(std::istream &is, TPolynomial &obj) {
    std::streamoff pos = (is >> std::ws).tellg();
    if (is.peek() == EOF) {
        is.setstate(std::istream::failbit | std::istream::eofbit);
        return is;
    }
    std::string line;
    std::getline(is, line);
    std::istringstream iss(line);
    obj.clear();
    TElementaryConjuction conj(obj.dim_);
    while (iss >> conj) {
        obj.polynomial_.push_back(std::move(conj));
        if (int32_t ch = (iss >> std::ws).peek(); ch != EOF && ch != '+') {
            is.seekg(pos);
            is.setstate(std::istream::failbit);
            return is;
        }
        iss.ignore(1);
    }
    is.clear();
    if (obj.polynomial_.empty()) {
        is.seekg(pos);
        is.setstate(std::istream::failbit);
    }
    obj.normalize();
    return is;
}

std::ostream& operator<<(std::ostream &os, const TPolynomial &obj) {
    os << obj.polynomial_.front();
    std::for_each(obj.begin() + 1, obj.end(), [&os](const TElementaryConjuction &conj) {
        os << '+' << conj;
    });
    return os;
}
