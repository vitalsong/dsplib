#include "fft/factory.h"

#include <fftw3.h>
#include <mutex>

namespace dsplib::internal {

static std::mutex g_mutex;

namespace {

static_assert(sizeof(real_t) == sizeof(double), "real_t type must be float64");

//------------------------------------------------------------------------------------------------------------------------
//complex FFT/IFFT
class FFTWPlanC : public FftPlanC
{
public:
    explicit FFTWPlanC(int n, bool forward = true)
      : n_{n}
      , scale_(forward ? 1 : (real_t(1) / n_))
      , out_(n) {
        std::lock_guard<std::mutex> lk(g_mutex);
        const int sign = forward ? FFTW_FORWARD : FFTW_BACKWARD;
        in_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * n);
        plan_ = fftw_plan_dft_1d(n, in_, reinterpret_cast<fftw_complex*>(out_.data()), sign,
                                 FFTW_MEASURE | FFTW_DESTROY_INPUT);
    }

    virtual ~FFTWPlanC() {
        std::lock_guard<std::mutex> lk(g_mutex);
        fftw_destroy_plan(plan_);
        fftw_free(in_);
    }

    dsplib::arr_cmplx solve(span_t<cmplx_t> x) const final {
        DSPLIB_ASSERT(x.size() == n_, "input size must be equal `n`");
        std::memcpy(in_, x.data(), n_ * sizeof(x[0]));
        fftw_execute(plan_);
        out_ *= scale_;
        return out_;
    }

    void solve(span_t<cmplx_t> x, mut_span_t<cmplx_t> r) const final {
        r = this->solve(x);
    }

    int size() const noexcept final {
        return n_;
    }

private:
    const int n_;
    real_t scale_{1.0};
    fftw_complex* in_{nullptr};
    mutable dsplib::arr_cmplx out_;
    fftw_plan plan_;
};

//------------------------------------------------------------------------------------------------------------------------
//real FFT
class FFTWPlanR : public FftPlanR
{
public:
    explicit FFTWPlanR(int n)
      : n_{n}
      , out_(n) {
        std::lock_guard<std::mutex> lk(g_mutex);
        in_ = (real_t*)fftw_malloc(sizeof(real_t) * n);
        plan_ =
          fftw_plan_dft_r2c_1d(n, in_, reinterpret_cast<fftw_complex*>(out_.data()), FFTW_MEASURE | FFTW_DESTROY_INPUT);
    }

    virtual ~FFTWPlanR() {
        std::lock_guard<std::mutex> lk(g_mutex);
        fftw_destroy_plan(plan_);
        fftw_free(in_);
    }

    dsplib::arr_cmplx solve(span_t<real_t> x) const final {
        DSPLIB_ASSERT(x.size() == n_, "input size must be equal `n`");
        std::memcpy(in_, x.data(), n_ * sizeof(x[0]));
        fftw_execute(plan_);
        const int n2 = (n_ % 2 == 0) ? (n_ / 2) : (n_ / 2 + 1);
        for (size_t i = 1; i < n2; ++i) {
            out_[n_ - i] = out_[i].conj();
        }
        return out_;
    }

    int size() const noexcept final {
        return n_;
    }

private:
    const int n_;
    real_t* in_{nullptr};
    mutable dsplib::arr_cmplx out_;
    fftw_plan plan_;
};

//------------------------------------------------------------------------------------------------------------------------
//real IFFT
class IFFTWPlanR : public IfftPlanR
{
public:
    explicit IFFTWPlanR(int n)
      : n_{n}
      , scale_{real_t(1) / n_}
      , out_(n) {
        std::lock_guard<std::mutex> lk(g_mutex);
        in_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (n_ / 2 + 1));
        plan_ = fftw_plan_dft_c2r_1d(n, in_, out_.data(), FFTW_MEASURE | FFTW_DESTROY_INPUT);
    }

    virtual ~IFFTWPlanR() {
        std::lock_guard<std::mutex> lk(g_mutex);
        fftw_destroy_plan(plan_);
        fftw_free(in_);
    }

    dsplib::arr_real solve(span_t<cmplx_t> x) const final {
        const int n2 = n_ / 2 + 1;
        DSPLIB_ASSERT((x.size() == n_) || (x.size() == n2), "input size must be equal `n` or `n/2+1`");
        std::memcpy(in_, x.data(), n2 * sizeof(x[0]));
        fftw_execute(plan_);
        out_ *= scale_;
        return out_;
    }

    int size() const noexcept final {
        return n_;
    }

private:
    const int n_;
    const real_t scale_;
    fftw_complex* in_{nullptr};
    mutable dsplib::arr_real out_;
    fftw_plan plan_;
};

}   // namespace

std::shared_ptr<FftPlanC> get_fft_plan(int n) {
    return std::make_shared<FFTWPlanC>(n, true);
}

std::shared_ptr<FftPlanR> get_rfft_plan(int n) {
    return std::make_shared<FFTWPlanR>(n);
}

std::shared_ptr<IfftPlanC> get_ifft_plan(int n) {
    return std::make_shared<FFTWPlanC>(n, false);
}

std::shared_ptr<IfftPlanR> get_irfft_plan(int n) {
    return std::make_shared<IFFTWPlanR>(n);
}

}   // namespace dsplib::internal