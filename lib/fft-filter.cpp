#include "dsplib/fir.h"

namespace dsplib {

template<typename T>
class FftFilterImpl
{
public:
    explicit FftFilterImpl(span_t<T> h)
      : _nfft{int(1) << nextpow2(2 * h.size())}
      , _m{h.size()}
      , _n{_nfft - h.size() + 1}
      , _x(_nfft)
      , _olap(_m - 1) {
        assert(_n > _m);
        _h = fft(conj(h), _nfft);
    }

    base_array<T> process(span_t<T> x) {
        const int nr = (x.size() + _nx) / _n * _n;
        base_array<T> r(nr);
        auto* pr = r.data();
        for (const auto& val : x) {
            _x[_nx] = val;
            _nx += 1;
            if (_nx == _n) {
                base_array<T> ry;
                if constexpr (std::is_same_v<T, real_t>) {
                    ry = std::move(irfft(fft(_x) * _h));   //TODO: use n/2+1 multiply
                } else {
                    ry = std::move(ifft(fft(_x) * _h));
                }
                for (int i = 0; i < _n; i++) {
                    pr[i] = ry[i];
                }
                for (int i = 0; i < (_m - 1); i++) {
                    pr[i] += _olap[i];
                    _olap[i] = ry[i + _n];
                }
                pr += _n;
                _nx = 0;
            }
        }
        return r;
    }

    [[nodiscard]] int block_size() const {
        return _n;
    }

private:
    int _nx{0};
    int _nfft;
    int _m{0};
    int _n{0};
    base_array<T> _x;
    arr_cmplx _h;
    base_array<T> _olap;
};

template<>
FftFilter<real_t>::FftFilter(span_t<real_t> h) {
    d_ = std::make_shared<FftFilterImpl<real_t>>(h);
}

template<>
FftFilter<cmplx_t>::FftFilter(span_t<cmplx_t> h) {
    d_ = std::make_shared<FftFilterImpl<cmplx_t>>(h);
}

template<>
base_array<real_t> FftFilter<real_t>::process(span_t<real_t> x) {
    return d_->process(x);
}

template<>
base_array<cmplx_t> FftFilter<cmplx_t>::process(span_t<cmplx_t> x) {
    return d_->process(x);
}

template<>
int FftFilter<real_t>::block_size() const {
    return d_->block_size();
}

template<>
int FftFilter<cmplx_t>::block_size() const {
    return d_->block_size();
}

}   // namespace dsplib