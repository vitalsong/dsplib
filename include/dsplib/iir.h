#pragma once

#include <dsplib/array.h>
#include <dsplib/utils.h>
#include <dsplib/inplace.h>
#include <memory>

namespace dsplib {

//------------------------------------------------------------------------------------
//TODO: naming
enum class IIRStructure
{
    DirectForm1,     //Direct form I
    DirectForm2,     //Direct form II
    DirectForm1Tr,   //Direct form I (transposed)
    DirectForm2Tr    //Direct form II (transposed)
};

//------------------------------------------------------------------------------------
//Infinite impulse response filter
template<typename T>
class IIRFilter
{
public:
    explicit IIRFilter(const arr_real& num, const arr_real& den)
      : num_{num}
      , den_{den}
      , n_{den_.size()} {
        if (den_.size() != num_.size()) {
            DSPLIB_THROW("IIR num/den size is not equal");
        }
        //TODO: num_ /= den_[0], den_ /= den_[0]
        dx_ = dsplib::zeros(n_);
        dy_ = dsplib::zeros(n_ - 1);
    }

    base_array<T> operator()(const base_array<T>& x) {
        return this->process(x);
    }

    T operator()(const T& x) {
        return this->process(x);
    }

    base_array<T> process(const base_array<T>& x) {
        auto y = zeros(x.size());
        for (int i = 0; i < x.size(); ++i) {
            y[i] = process(x[i]);
        }
        return y;
    }

    T process(T x) {
        //TODO: cyclic buffer?
        dx_.slice(1, n_) = dx_.slice(0, n_ - 1);
        dx_[0] = x;
        T y = 0;
        for (int i = 0; i < n_; ++i) {
            y += num_[i] * dx_[i];
        }
        for (int i = 0; i < n_ - 1; ++i) {
            y -= den_[i + 1] * dy_[i];
        }
        dy_.slice(1, n_ - 1) = dy_.slice(0, n_ - 2);
        dy_[0] = y;
        return y;
    }

private:
    arr_real num_;
    arr_real den_;
    base_array<T> dx_;
    base_array<T> dy_;
    int n_;
};

//------------------------------------------------------------------------------------
class BaseBiquadSection
{
public:
    explicit BaseBiquadSection(real_t b0, real_t b1, real_t b2, real_t a1, real_t a2)
      : b0_{b0}
      , b1_{b1}
      , b2_{b2}
      , a1_{a1}
      , a2_{a2} {
    }

    virtual ~BaseBiquadSection() {
    }

    virtual void process(arr_real& x) = 0;

protected:
    const real_t b0_;
    const real_t b1_;
    const real_t b2_;
    const real_t a1_;
    const real_t a2_;
};

//------------------------------------------------------------------------------------
class BiquadSection1 : public BaseBiquadSection
{
public:
    explicit BiquadSection1(real_t b0, real_t b1, real_t b2, real_t a1, real_t a2)
      : BaseBiquadSection(b0, b1, b2, a1, a2) {
    }

    real_t process(const real_t& x) noexcept {
        const real_t y = (x * b0_) + (x1_ * b1_) + (x2_ * b2_) - (y1_ * a1_) - (y2_ * a2_);
        x2_ = x1_;
        x1_ = x;
        y2_ = y1_;
        y1_ = y;
        return y;
    }

    void process(arr_real& x) noexcept final {
        for (auto& v : x) {
            v = process(v);
        }
    }

private:
    real_t x1_{0};
    real_t x2_{0};
    real_t y1_{0};
    real_t y2_{0};
};

//------------------------------------------------------------------------------------
class BiquadSection1Tr : public BaseBiquadSection
{
public:
    explicit BiquadSection1Tr(real_t b0, real_t b1, real_t b2, real_t a1, real_t a2)
      : BaseBiquadSection(b0, b1, b2, a1, a2) {
    }

    real_t process(const real_t& x) noexcept {
        const real_t u0 = (x - x1_);
        const real_t y = u0 * b0_ + y1_;
        x1_ = x2_ + u0 * a1_;
        x2_ = u0 * a2_;
        y1_ = y2_ + u0 * b1_;
        y2_ = u0 * b2_;
        return y;
    }

    void process(arr_real& x) noexcept final {
        for (auto& v : x) {
            v = process(v);
        }
    }

private:
    real_t x1_{0};
    real_t x2_{0};
    real_t y1_{0};
    real_t y2_{0};
};

//------------------------------------------------------------------------------------
class BiquadSection2 : public BaseBiquadSection
{
public:
    explicit BiquadSection2(real_t b0, real_t b1, real_t b2, real_t a1, real_t a2)
      : BaseBiquadSection(b0, b1, b2, a1, a2) {
    }

    real_t process(const real_t& x) noexcept {
        const real_t u0 = x - u2_ * a2_ - u1_ * a1_;
        const real_t y = u0 * b0_ + u1_ * b1_ + u2_ * b2_;
        u2_ = u1_;
        u1_ = u0;
        return y;
    }

    void process(arr_real& x) noexcept final {
        for (auto& v : x) {
            v = process(v);
        }
    }

private:
    real_t u1_{0};
    real_t u2_{0};
};

//------------------------------------------------------------------------------------
class BiquadSection2Tr : public BaseBiquadSection
{
public:
    explicit BiquadSection2Tr(real_t b0, real_t b1, real_t b2, real_t a1, real_t a2)
      : BaseBiquadSection(b0, b1, b2, a1, a2) {
    }

    real_t process(const real_t& x) noexcept {
        const real_t y = u1_ + (b0_ * x);
        u1_ = (b1_ * x) - (a1_ * y) + u2_;
        u2_ = (b2_ * x) - (a2_ * y);
        return y;
    }

    void process(arr_real& x) noexcept final {
        for (auto& v : x) {
            v = process(v);
        }
    }

private:
    real_t u1_{0};
    real_t u2_{0};
};

//------------------------------------------------------------------------------------
//IIR filter using biquadratic structures
class BiquadSection
{
public:
    explicit BiquadSection(real_t b0, real_t b1, real_t b2, real_t a1, real_t a2,
                           IIRStructure structure = IIRStructure::DirectForm2Tr) {
        switch (structure) {
        case dsplib::IIRStructure::DirectForm1:
            d_ = std::make_unique<BiquadSection1>(b0, b1, b2, a1, a2);
            break;
        case dsplib::IIRStructure::DirectForm1Tr:
            d_ = std::make_unique<BiquadSection1Tr>(b0, b1, b2, a1, a2);
            break;
        case dsplib::IIRStructure::DirectForm2:
            d_ = std::make_unique<BiquadSection2>(b0, b1, b2, a1, a2);
            break;
        case dsplib::IIRStructure::DirectForm2Tr:
            d_ = std::make_unique<BiquadSection2Tr>(b0, b1, b2, a1, a2);
            break;
        };
    }

    explicit BiquadSection(const arr_real& num, const arr_real& den)
      : BiquadSection{num[0], num[1], num[2], den[1], den[2]} {
        DSPLIB_ASSERT((den.size() == 3) && (num.size() == 3), "biquad coeffs must have length 3");
    }

    void process(arr_real& x) noexcept {
        d_->process(x);
    }

private:
    std::unique_ptr<BaseBiquadSection> d_;
};

//------------------------------------------------------------------------------------
//Second-order section biquadratic IIR filter structures
//TODO: rename to BiquadFilter?
//TODO: use BiquadIIR filter (fixed sizes algorithm)
//TODO: use matrix instead vector<array>
//TODO: base_array<T> implementation
//TODO: inplace process
class SOSFilter
{
public:
    explicit SOSFilter(const std::vector<arr_real>& num, const std::vector<arr_real>& den, arr_real scale)
      : scale_{scale} {
        DSPLIB_ASSERT((num.size() == den.size()) && (num[0].size() == den[0].size()),
                      "'num' and 'den' arrays have different lengths");
        DSPLIB_ASSERT(scale.size() == num.size() + 1, "'scale' must be scalar or array[n+1]");

        for (int i = 0; i < num.size(); ++i) {
            iirs_.emplace_back(BiquadSection(num[i], den[i]));
        }
    }

    explicit SOSFilter(const std::vector<arr_real>& num, const std::vector<arr_real>& den, real_t scale = 1.0)
      : SOSFilter{num, den, _scale_array(num.size(), scale)} {
    }

    explicit SOSFilter(const arr_real& num, const arr_real& den, real_t scale = 1.0)
      : SOSFilter{{num}, {den}, _scale_array(1, scale)} {
    }

    void operator()(inplace_t<arr_real> x) {
        this->process(x);
    }

    [[nodiscard]] arr_real operator()(const arr_real& x) {
        return this->process(x);
    }

    //TODO: inplace span<real_t>
    void process(inplace_t<arr_real> x) {
        arr_real& y = x.get();
        y *= scale_[0];
        for (int k = 0; k < iirs_.size(); ++k) {
            iirs_[k].process(y);
            y *= scale_[k + 1];
        }
    }

    [[nodiscard]] arr_real process(const arr_real& x) {
        arr_real y(x);
        process(inplace(y));
        return y;
    }

    //TODO: get params
    //...

private:
    const arr_real scale_;
    std::vector<BiquadSection> iirs_;

    static arr_real _scale_array(int nsect, real_t scale) {
        arr_real r = ones(nsect + 1);
        r[0] = scale;
        return r;
    }
};

//TODO: isstable

}   // namespace dsplib