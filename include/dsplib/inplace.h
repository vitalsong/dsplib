#pragma once

namespace dsplib {

template<class T>
class inplace_t
{
public:
    explicit inplace_t(T& value)
      : value_{value} {
    }

    T& get() noexcept {
        return value_;
    }

private:
    T& value_;
};

//TODO: span convert support, example inplace(std::vector<real_t>&) -> inplace(span<real_t>&)
template<class T>
inplace_t<T> inplace(T& value) {
    return inplace_t<T>(value);
}

}   // namespace dsplib