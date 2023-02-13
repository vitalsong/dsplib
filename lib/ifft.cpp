#include <dsplib/ifft.h>
#include <dsplib/fft.h>
#include <dsplib/math.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
class IfftPlanImpl
{
public:
    explicit IfftPlanImpl(int n)
      : _fft{n} {
    }

    arr_cmplx solve(const arr_cmplx& x) const {
        const int n = x.size();
        return conj(_fft(conj(x)) / n);
    }

    FftPlan _fft;
};

//-------------------------------------------------------------------------------------------------
IfftPlan::IfftPlan(int n)
  : _d{std::make_shared<IfftPlanImpl>(n)} {
}

//-------------------------------------------------------------------------------------------------
arr_cmplx IfftPlan::operator()(const arr_cmplx& x) const {
    return _d->solve(x);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx IfftPlan::solve(const arr_cmplx& x) const {
    return _d->solve(x);
}

//-------------------------------------------------------------------------------------------------
int IfftPlan::size() const noexcept {
    return _d->_fft.size();
}

//-------------------------------------------------------------------------------------------------
arr_cmplx ifft(const arr_cmplx& x) {
    IfftPlan plan(x.size());
    return plan(x);
}

}   // namespace dsplib
