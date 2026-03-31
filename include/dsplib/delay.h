#pragma once

#include <dsplib/array.h>

namespace dsplib {

//Delay input signal by fixed samples
//TODO: add fractional delay
template<typename T>
class Delay
{
public:
    explicit Delay(int length)
      : nd_{length}
      , buf_(length) {
    }

    explicit Delay(const dsplib::base_array<T>& initial)
      : nd_{initial.size()}
      , buf_{initial} {
    }

    dsplib::base_array<T> process(span_t<T> x) {
        dsplib::base_array<T> dx(x);
        this->process(inplace(dx));
        return dx;
    }

    void process(inplace_span_t<T> ix) {
        if (nd_ == 0) {
            return;
        }
        auto x = ix.get();
        for (int i = 0; i < x.size(); ++i) {
            std::swap(buf_[pos_], x[i]);
            pos_ = (pos_ + 1) % nd_;
        }
    }

    dsplib::base_array<T> operator()(span_t<T> x) {
        return this->process(x);
    }

    [[nodiscard]] int length() const noexcept {
        return buf_.size();
    }

private:
    int nd_{0};
    int pos_{0};
    dsplib::base_array<T> buf_;
};

using DelayReal = Delay<real_t>;
using DelayCmplx = Delay<cmplx_t>;

}   // namespace dsplib