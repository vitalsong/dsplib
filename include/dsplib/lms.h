#pragma once

#include <dsplib/array.h>
#include <dsplib/utils.h>
#include <dsplib/math.h>

namespace dsplib {

enum class LmsType
{
    LMS,
    NLMS
};

//LMS adaptive filter
template<typename T>
class LmsFilter
{
public:
    //len: length of FIR filter weights
    //step_size: adaptation step size
    //method: method to calculate filter weights
    //leak: leakage factor
    explicit LmsFilter(int len, real_t step_size, LmsType method = LmsType::LMS, real_t leak = 1)
      : _u(len - 1)
      , _w(len)
      , _mu{step_size}
      , _len{len}
      , _method{method}
      , _lk{leak} {
    }

    struct Result
    {
        base_array<T> y;   //output
        base_array<T> e;   //error
    };

    Result operator()(const base_array<T>& x, const base_array<T>& d) {
        return this->process(x, d);
    }

    Result process(const base_array<T>& x, const base_array<T>& d) {
        if (x.size() != d.size()) {
            DSPLIB_THROW("vector size error: len(x) != len(d)");
        }

        int nx = x.size();
        base_array<T> y(nx);
        base_array<T> e(nx);
        base_array<T> tu = _u | x;
        arr_real tu2 = (_method == LmsType::NLMS) ? abs2(tu) : arr_real{};

        //update delay
        _u = tu.slice(nx, nx + _len - 1);

        for (int k = 0; k < nx; k++) {
            //y(n) = w.T(n) * u(n)
            for (int i = 0; i < _len; i++) {
                y[k] += _w[i] * tu[i + k];
            }

            e[k] = d[k] - y[k];

            if (_locked) {
                continue;
            }

            if (_method == LmsType::LMS) {
                //TODO: dont use cycles
                //w(n) = w(n-1) * a + mu * e(n) * u(n)
                for (int i = 0; i < _len; i++) {
                    _w[i] = (_w[i] * _lk) + (_mu * e[k] * conj(tu[i + k]));
                }
            } else if (_method == LmsType::NLMS) {
                //pu = u.T(n) * u(n)
                //TODO: use recurrent sum
                real_t pu = 0;
                for (int i = 0; i < _len; i++) {
                    pu += tu2[i + k];
                }

                //w(n) = w(n-1) * a + mu * e(n) * u(n) / norm
                const real_t norm = pu + eps();
                for (int i = 0; i < _len; i++) {
                    _w[i] = (_w[i] * _lk) + (_mu * e[k] * conj(tu[i + k]) / norm);
                }
            }
        }

        return {y, e};
    }

    void set_lock_coeffs(bool locked) {
        _locked = locked;
    }

    [[nodiscard]] bool coeffs_locked() const {
        return _locked;
    }

    base_array<T> coeffs() const {
        return flip(_w);
    }

    //TODO: reset()
    //TODO: step_size control

private:
    base_array<T> _u;
    base_array<T> _w;
    real_t _mu;
    int _len;
    bool _locked{false};
    LmsType _method;
    real_t _lk;
};

using LmsFilterR = LmsFilter<real_t>;
using LmsFilterC = LmsFilter<cmplx_t>;

}   // namespace dsplib
