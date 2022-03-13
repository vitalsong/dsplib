#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
class ifft_plan_impl
{
public:
    explicit ifft_plan_impl(int n)
      : _fft{n} {
    }

    arr_cmplx solve(const arr_cmplx& x) const {
        const int n = x.size();
        return conj(_fft(conj(x)) / n);
    }

    fft_plan _fft;
};

//-------------------------------------------------------------------------------------------------
ifft_plan::ifft_plan(int n)
  : _d{std::make_shared<ifft_plan_impl>(n)} {
}

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft_plan::operator()(const arr_cmplx& x) const {
    return _d->solve(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft_plan::solve(const arr_cmplx& x) const {
    return _d->solve(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx& x) {
    ifft_plan plan(x.size());
    return plan(x);
}

}   // namespace dsplib
