#include <hdrs/elementary_conjuction.hpp>

#include <algorithm>

TElementaryConjuction::iterator TElementaryConjuction::begin() noexcept {
    return conjuction_.begin();
}

TElementaryConjuction::iterator TElementaryConjuction::end() noexcept {
    return conjuction_.end();
}

void TElementaryConjuction::clear() {
    conjuction_.clear();
}

void TElementaryConjuction::normalize() {
    if (begin() == end()) {
        conjuction_.push_back({ 0, EState::Empty });
    } else {
        conjuction_.erase(std::remove(begin(), end(), TVariable{ 0, EState::Positive }), end());
        std::sort(begin(), end());
        conjuction_.erase(std::unique(begin(), end()), end());
        for (auto it = begin(); it + 1 < end(); ++it) {
            if ((it->number == 0 && it->state == EState::Empty) || (it->number == (it + 1)->number
            && it->state == EState::Positive && (it + 1)->state == EState::Negative))
            {
                clear();
                return;
            }
        }
        if (conjuction_.empty()) {
            conjuction_.push_back({ 0, EState::Positive });
        }
    }
}

TElementaryConjuction::TElementaryConjuction(uint32_t dim) : dim_(dim), conjuction_(), excess_() {}

TElementaryConjuction::TElementaryConjuction(const TElementaryConjuction &obj)
    : dim_(obj.dim_)
    , conjuction_(obj.conjuction_)
    , excess_(obj.excess_)
{
}

TElementaryConjuction::TElementaryConjuction(TElementaryConjuction &&obj)
    : dim_(obj.dim_)
    , conjuction_(std::move(obj.conjuction_))
    , excess_(obj.excess_)
{
}

TElementaryConjuction::TElementaryConjuction(uint32_t dim, std::vector<TVariable> conj)
    : dim_(dim)
    , conjuction_(conj)
    , excess_()
{
}

bool TElementaryConjuction::evaluate(uint32_t vector) const {
    // vector &= (1 << dim_) - 1;
    if (!conjuction_.front().number) {
        return conjuction_.front().state == EState::Positive;
    }
    bool result = true;
    for (const TVariable &it : conjuction_) {
        result &= (vector >> (it.number - 1) & 1) ^ (it.state == EState::Negative);
    }
    return result;
}

bool TElementaryConjuction::isInversed(const TElementaryConjuction &obj) const {
    if (dim_ != obj.dim_) {
        throw std::logic_error("dimensions of operands doesn't match");
    }
    return (conjuction_.size() == 1 && obj.conjuction_.size() == 1 &&
        conjuction_[0].number == obj.conjuction_[0].number &&
        static_cast<int32_t>(conjuction_[0].state) + static_cast<int32_t>(obj.conjuction_[0].state)
        == static_cast<int32_t>(EState::Positive) + static_cast<int32_t>(EState::Negative)
    );
}

void TElementaryConjuction::setState(bool exc) const {
    excess_ = exc;
}

bool TElementaryConjuction::getState() const {
    return excess_;
}

bool TElementaryConjuction::empty() const {
    return conjuction_.empty();
}

bool TElementaryConjuction::operator<(const TElementaryConjuction &obj) const {
    if (conjuction_.size() != obj.conjuction_.size()) {
        return conjuction_.size() < obj.conjuction_.size();
    }
    return std::lexicographical_compare(
        begin(),
        end(),
        obj.begin(),
        obj.end()
    );
}

bool TElementaryConjuction::operator==(const TElementaryConjuction &obj) const {
    if (conjuction_.size() != obj.conjuction_.size()) {
        return false;
    }
    return std::equal(begin(), end(), obj.begin());
}

TElementaryConjuction& TElementaryConjuction::operator=(const TElementaryConjuction &obj) {
    if (dim_ != obj.dim_) {
        throw std::logic_error("dimensions of operands doesn't match");
    }
    if (this != &obj) {
        conjuction_ = obj.conjuction_;
    }
    return *this;
}

TElementaryConjuction& TElementaryConjuction::operator*=(const TElementaryConjuction &obj) {
    conjuction_.reserve(conjuction_.size() + obj.conjuction_.size());
    std::copy(obj.begin(), obj.end(), std::back_inserter(conjuction_));
    normalize();
    return *this;
}

TElementaryConjuction::const_iterator TElementaryConjuction::begin() const noexcept {
    return conjuction_.begin();
}

TElementaryConjuction::const_iterator TElementaryConjuction::end() const noexcept {
    return conjuction_.end();
}

std::istream& operator>>(std::istream &is, TElementaryConjuction &obj) {
    std::streamoff pos = (is >> std::ws).tellg();
    if (is.peek() == EOF) {
        is.setstate(std::istream::failbit | std::istream::eofbit);
        return is;
    }
    obj.conjuction_.clear();
    TVariable var;
    while (!isspace(is.peek()) && is >> var) {
        if (var.number > obj.dim_) {
            is.seekg(pos);
            is.setstate(std::istream::failbit);
            return is;
        }
        obj.conjuction_.push_back(var);
    }
    is.clear();
    if (obj.conjuction_.empty()) {
        is.seekg(pos);
        is.setstate(std::istream::failbit);
    }
    obj.normalize();
    return is;
}

std::ostream& operator<<(std::ostream &os, const TElementaryConjuction &obj) {
    std::for_each(obj.begin(), obj.end(), [&os](const TVariable &var) { os << var; });
    return os;
}
