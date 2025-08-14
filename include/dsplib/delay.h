#pragma once

#include <dsplib/array.h>

namespace dsplib {

//Delay input signal by fixed samples
template<typename T>
class Delay
{
public:
    explicit Delay(int length)
      : _buffer(length) {
    }

    explicit Delay(const dsplib::base_array<T>& initial)
      : _buffer{initial} {
    }

    dsplib::base_array<T> process(span_t<T> x) {
        //TODO: use cycle buffer
        const int nd = _buffer.size();
        const auto tmp = concatenate(_buffer, x);
        _buffer.slice(0, nd) = tmp.slice(tmp.size() - nd, tmp.size());
        return tmp.slice(0, x.size());
    }

    dsplib::base_array<T> operator()(span_t<T> x) {
        return this->process(x);
    }

private:
    dsplib::base_array<T> _buffer;
};

using DelayReal = Delay<real_t>;
using DelayCmplx = Delay<cmplx_t>;

}   // namespace dsplib