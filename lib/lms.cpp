#include <dsplib/lms.h>
#include <dsplib/utils.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
lms::lms(int len, double step_size) {
    _len = len;
    _u = zeros(len - 1);
    _w = zeros(len);
    _m = step_size;
}

//-------------------------------------------------------------------------------------------------
lms::result lms::process(const arr_real& x, const arr_real& d) {
    if (x.size() != d.size()) {
        throw std::runtime_error("vector size error: len(x) != len(d)");
    }

    int nx = x.size();
    auto y = zeros(nx);
    auto e = zeros(nx);
    auto tu = _u | x;

    //update delay
    _u = tu.slice(nx, nx + _len - 1);

    for (size_t k = 0; k < nx; k++) {
        //y(n) = w(n-1).T * u(n-1)
        for (size_t i = 0; i < _len; i++) {
            y[k] += _w[i] * tu[i + k];
        }
        e[k] = d[k] - y[k];
        if (!_locked) {
            //w(n) = w(n-1) + mu * e(n) * u(n-1)
            for (size_t i = 0; i < _len; i++) {
                _w[i] += _m * e[k] * tu[i + k];
            }
        }
    }

    return {y, e};
}

}   // namespace dsplib
