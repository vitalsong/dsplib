#include "dsplib/fir.h"
#include <dsplib/detector.h>
#include <dsplib.h>
#include <cassert>
#include <memory>

namespace dsplib {

//---------------------------------------------------------------------------------
//fir with impulse = [1, 1, 1, 1, 1, 1 ...] / n
class AveFir
{
public:
    explicit AveFir(int n)
      : _n{n}
      , _x(n) {
    }

    arr_real process(const arr_real& x) {
        if (_n == 0) {
            return zeros(x.size());
        }

        //TODO: potential error accumulation problem
        arr_real r(x.size());
        for (size_t i = 0; i < x.size(); i++) {
            _s -= _x[_idx];
            _x[_idx] = x[i];
            _s += _x[_idx];
            _idx++;
            _idx = (_idx == _n) ? (0) : (_idx);
            r[i] = _s / _n;
        }
        return r;
    }

private:
    int _n{0};
    arr_real _x;
    real_t _s{0};
    int _idx{0};
};

//---------------------------------------------------------------------------------
struct DetectorImpl
{
    arr_cmplx delay;
    int lock_time{0};
    real_t current_max{0};
    int release_time{0};
    real_t threshold{1};
    FftFilter fir_match;
    AveFir fir_pow{1};
    int detect_size{0};
    bool triggered{false};
    std::vector<cmplx_t> buffer;
    int block_size{0};
};

//---------------------------------------------------------------------------------
Detector::Detector(const arr_cmplx& h, real_t threshold, int release) {
    const int nh = h.size();
    assert(nh > 0);
    _d = std::make_shared<DetectorImpl>();
    _d->delay = zeros(nh + release);
    _d->fir_match = FftFilter(flip(h) / (nh * rms(h)));
    _d->block_size = _d->fir_match.block_size();
    _d->fir_pow = AveFir(nh);
    _d->release_time = release;
    _d->threshold = threshold * threshold;
    _d->detect_size = nh;
}

//---------------------------------------------------------------------------------
static Detector::Result _detecting(DetectorImpl* _d, const arr_cmplx& x) {
    Detector::Result res;

    assert(x.size() == _d->block_size);

    //update delay
    auto delay = _d->delay | x;
    _d->delay = delay.slice(delay.size() - _d->delay.size(), delay.size());

    //match fir line
    auto y_m = _d->fir_match.process(x);
    auto y_m2 = pow2(real(y_m)) + pow2(imag(y_m));

    //power estimate line
    auto x2 = pow2(real(x)) + pow2(imag(x));
    auto y_p2 = _d->fir_pow.process(x2);

    //detecting map [0 : 1]
    auto y_r = y_m2 / (y_p2 + 1e-9);

    //update state
    auto max_val = max(y_r);
    if ((max_val > _d->threshold) && !_d->triggered) {
        _d->triggered = true;
        _d->current_max = max_val;
        _d->lock_time = y_r.size() - argmax(y_r);
    } else if (_d->triggered) {
        _d->lock_time += y_r.size();
        if (max_val > _d->current_max) {
            _d->current_max = max_val;
            _d->lock_time = y_r.size() - argmax(y_r);
        }
    }

    //if ready
    if (_d->lock_time >= _d->release_time) {
        res.triggered = true;
        res.level = sqrt(_d->current_max);
        const int t1 = delay.size() - _d->lock_time - _d->detect_size;
        const int t2 = delay.size();
        res.delay = delay.slice(t1, t2);
        _d->lock_time = 0;
        _d->current_max = 0;
        _d->triggered = false;
    }

    return res;
}

//---------------------------------------------------------------------------------
Detector::Result Detector::process(const arr_cmplx& x) {
    Detector::Result res;
    for (size_t i = 0; i < x.size(); i++) {
        _d->buffer.push_back(x[i]);

        if (res.triggered) {
            res.delay.push_back(x[i]);
        }

        if (_d->buffer.size() == _d->block_size) {
            auto dtc = _detecting(_d.get(), arr_cmplx(_d->buffer));
            _d->buffer.clear();
            if (dtc.level > res.level) {
                res = dtc;
            }
        }
    }

    return res;
}

//---------------------------------------------------------------------------------
void Detector::reset() {
    _d->lock_time = 0;
    _d->current_max = 0;
    _d->triggered = false;
}

}   // namespace dsplib