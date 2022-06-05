#pragma once

#include <dsplib/array.h>

namespace dsplib {

//RLS adaptive filter
class rls
{
public:
    rls(int filter_len, real_t forget_factor = 0.9, real_t diag_load = 1);

    struct result
    {
        arr_real y;   //output
        arr_real e;   //error
    };

    result operator()(const arr_real& x, const arr_real& d) {
        return this->process(x, d);
    }

    result process(const arr_real& x, const arr_real& d);

    void set_lock_coeffs(bool locked) {
        _locked = locked;
    }

    bool coeffs_locked() const {
        return _locked;
    }

    arr_real coeffs() const {
        return _w;
    }

private:
    int _n;
    real_t _mu;
    arr_real _u;
    arr_real _w;
    arr_real _P;
    bool _locked{false};
};

}   // namespace dsplib
