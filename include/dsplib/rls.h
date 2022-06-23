#pragma once

#include <dsplib/array.h>

namespace dsplib {

//RLS adaptive filter
template<typename T>
class base_rls
{
public:
    base_rls(int filter_len, real_t forget_factor = 0.9, real_t diag_load = 1)

      : _n{filter_len}
      , _mu{forget_factor} {
        //init filter
        _w = dsplib::zeros(_n);
        _u = dsplib::zeros(_n - 1);

        //identity matrix
        _P = dsplib::zeros(_n * _n);
        for (size_t i = 0; i < _n; i++) {
            _P[i * _n + i] = diag_load;
        }
    }

    struct result_t
    {
        base_array<T> y;   //output
        base_array<T> e;   //error
    };

    result_t operator()(const base_array<T>& x, const base_array<T>& d) {
        return this->process(x, d);
    }

    result_t process(const base_array<T>& x, const base_array<T>& d);

    void set_lock_coeffs(bool locked) {
        _locked = locked;
    }

    bool coeffs_locked() const {
        return _locked;
    }

    base_array<T> coeffs() const {
        return flip(_w);
    }

private:
    int _n;
    real_t _mu;
    base_array<T> _u;
    base_array<T> _w;
    base_array<T> _P;
    bool _locked{false};
};

//-----------------------------------------------------------------------------------------------

using rls = base_rls<real_t>;
using crls = base_rls<cmplx_t>;

//-----------------------------------------------------------------------------------------------
template<typename T>
typename base_rls<T>::result_t base_rls<T>::process(const base_array<T>& x, const base_array<T>& d) {
    if (x.size() != d.size()) {
        throw std::runtime_error("vector size error: len(x) != len(d)");
    }

    const int nx = x.size();
    base_array<T> y = zeros(nx);
    base_array<T> e = zeros(nx);
    base_array<T> tu = _u | x;
    base_array<T> g = zeros(_n);

    //update delay
    _u = tu.slice(nx, nx + _n - 1);

    //TODO: use matrix syntax

    base_array<T> Pu = zeros(_n);
    base_array<T> uTP = zeros(_n);
    base_array<T> gu = zeros(_n * _n);
    base_array<T> guP = zeros(_n * _n);
    base_array<T> u = zeros(_n);

    for (size_t idx = 0; idx < nx; idx++) {
        u = tu.slice(idx, idx + _n);
        y[idx] = sum(_w * u);
        e[idx] = d[idx] - y[idx];

        if (_locked) {
            continue;
        }

        //g = (P * u) / (mu + u.H * P * u);
        {
            //Pu = P * u
            std::fill(Pu.begin(), Pu.end(), 0);
            for (size_t i = 0; i < _n; i++) {
                for (size_t k = 0; k < _n; k++) {
                    Pu[i] += _P[i * _n + k] * u[k];
                }
            }

            //u.H * P
            std::fill(uTP.begin(), uTP.end(), 0);
            for (size_t i = 0; i < _n; i++) {
                for (size_t k = 0; k < _n; k++) {
                    uTP[i] += conj(u[k]) * _P[k * _n + i];
                }
            }

            //mu + u.H * P * u
            g = Pu / (_mu + sum(uTP * u));
        }

        //P = (1/mu) * (P - g * u.H * P);
        {
            //gu = g * u.H
            std::fill(gu.begin(), gu.end(), 0);
            for (size_t i = 0; i < _n; i++) {
                gu.slice(i * _n, (i + 1) * _n) = conj(u) * g[i];
            }

            //gu * P
            std::fill(guP.begin(), guP.end(), 0);
            for (int i = 0; i < _n; i++) {
                for (int j = 0; j < _n; j++) {
                    for (int k = 0; k < _n; k++) {
                        guP[i * _n + j] += gu[i * _n + k] * _P[k * _n + j];
                    }
                }
            }

            for (size_t i = 0; i < (_n * _n); i++) {
                _P[i] = (1 / _mu) * (_P[i] - guP[i]);
            }
        }

        _w = _w + conj(g) * e[idx];
    }

    return {y, e};
}

}   // namespace dsplib
