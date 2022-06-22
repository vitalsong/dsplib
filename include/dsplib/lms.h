#pragma once

#include <dsplib/array.h>

namespace dsplib {

enum class lms_type
{
    LMS,
    NLMS
};

//LMS adaptive filter
template<typename T>
class base_lms
{
public:
    //len: length of FIR filter weights
    //step_size: adaptation step size
    //method: method to calculate filter weights
    //leak: leakage factor
    explicit base_lms(int len, real_t step_size, lms_type method = lms_type::LMS, real_t leak = 1)
      : _len{len}
      , _method{method}
      , _lk{leak} {
        _u = zeros(len - 1);
        _w = zeros(len);
        _mu = step_size;
    }

    struct result_t
    {
        base_array<T> y;   //output
        base_array<T> e;   //error
    };

    result_t operator()(const base_array<T>& x, const base_array<T>& d) {
        return this->process(x, d);
    }

    result_t process(const base_array<T>& x, const base_array<T>& d) {
        if (x.size() != d.size()) {
            throw std::runtime_error("vector size error: len(x) != len(d)");
        }

        int nx = x.size();
        auto y = zeros(nx);
        auto e = zeros(nx);
        auto tu = _u | x;

        auto tu2 = (_method == lms_type::NLMS) ? abs2(tu) : tu;

        //update delay
        _u = tu.slice(nx, nx + _len - 1);

        for (size_t k = 0; k < nx; k++) {
            //y(n) = w.T(n) * u(n)
            for (size_t i = 0; i < _len; i++) {
                y[k] += conj(_w[i]) * tu[i + k];
            }

            e[k] = d[k] - y[k];

            if (_locked) {
                continue;
            }

            if (_method == lms_type::LMS) {
                //TODO: dont use cycles
                //w(n) = w(n-1) * a + mu * e(n) * u(n)
                for (size_t i = 0; i < _len; i++) {
                    _w[i] = (_w[i] * _lk) + (_mu * e[k] * tu[i + k]);
                }
            } else if (_method == lms_type::NLMS) {
                //pu = u.T(n) * u(n)
                //TODO: use recurrent sum
                real_t pu = 0;
                for (size_t i = 0; i < _len; i++) {
                    pu += tu2[i + k];
                }

                //w(n) = w(n-1) * a + mu * e(n) * u(n) / norm
                const real_t norm = pu + eps();
                for (size_t i = 0; i < _len; i++) {
                    _w[i] = (_w[i] * _lk) + (_mu * e[k] * tu[i + k] / norm);
                }
            }
        }

        return {y, e};
    }

    void set_lock_coeffs(bool locked) {
        _locked = locked;
    }

    bool coeffs_locked() const {
        return _locked;
    }

    base_array<T> coeffs() const {
        return _w;
    }

    //TODO: reset()
    //TODO: step_size control

private:
    base_array<T> _u;
    base_array<T> _w;
    real_t _mu;
    int _len;
    bool _locked{false};
    lms_type _method;
    real_t _lk;
};

//real implementation
using lms = base_lms<real_t>;

//complex implementation
using clms = base_lms<cmplx_t>;

}   // namespace dsplib
