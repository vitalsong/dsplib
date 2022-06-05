#include <dsplib/rls.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
rls::rls(int filter_len, real_t forget_factor, real_t diag_load)
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

//-------------------------------------------------------------------------------------------------
rls::result rls::process(const arr_real& x, const arr_real& d) {
    if (x.size() != d.size()) {
        throw std::runtime_error("vector size error: len(x) != len(d)");
    }

    const int nx = x.size();
    auto y = zeros(nx);
    auto e = zeros(nx);
    auto tu = _u | x;
    auto g = zeros(_n);

    //update delay
    _u = tu.slice(nx, nx + _n - 1);

    //TODO: use matrix syntax

    arr_real Pu = zeros(_n);
    arr_real uTP = zeros(_n);
    arr_real gu = zeros(_n * _n);
    arr_real guP = zeros(_n * _n);
    arr_real u = zeros(_n);

    for (size_t idx = 0; idx < nx; idx++) {
        u = tu.slice(idx, idx + _n);
        y[idx] = sum(_w * u);
        e[idx] = d[idx] - y[idx];

        if (_locked) {
            continue;
        }

        //g = (P * u) / (mu + u.T * P * u);
        {
            //Pu = P * u
            std::fill(Pu.begin(), Pu.end(), 0);
            for (size_t i = 0; i < _n; i++) {
                for (size_t k = 0; k < _n; k++) {
                    Pu[i] += _P[i * _n + k] * u[k];
                }
            }

            //u.T * P
            std::fill(uTP.begin(), uTP.end(), 0);
            for (size_t i = 0; i < _n; i++) {
                for (size_t k = 0; k < _n; k++) {
                    uTP[i] += u[k] * _P[k * _n + i];
                }
            }

            //mu + u.T * P * u
            g = Pu / (_mu + sum(uTP * u));
        }

        //P = (1/mu) * (P - g * u.T * P);
        {
            //gu = g * u.T
            std::fill(gu.begin(), gu.end(), 0);
            for (size_t i = 0; i < _n; i++) {
                gu.slice(i * _n, (i + 1) * _n) = g[i] * u;
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

        _w = _w + e[idx] * g;
    }

    return {y, e};
}

}   // namespace dsplib
