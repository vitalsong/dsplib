#include <dsplib/detector.h>
#include <dsplib.h>
#include <cassert>

namespace dsplib {

//---------------------------------------------------------------------------------
//fir with impulse = [1, 1, 1, 1, 1, 1 ...] / n
class ave_fir
{
public:
    ave_fir() = default;

    ave_fir(int n)
    {
        _n = n;
        _x = zeros(n);
    }

    arr_real process(const arr_real& x)
    {
        if (_n == 0) {
            return zeros(x.size());
        }

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
    arr_real _x;
    real_t _s{0};
    int _idx{0};
    int _n{0};
};

//---------------------------------------------------------------------------------
struct detector_impl
{
    arr_cmplx delay;
    int lock_time{0};
    real_t current_max{0};
    int release_time{0};
    real_t threshold{1};
    fir_fft fir_match;
    ave_fir fir_pow;
    int detect_size;
    bool triggered{false};
    std::vector<cmplx_t> buffer;
    int block_size{0};
};

//---------------------------------------------------------------------------------
detector::detector(const arr_cmplx& h, real_t threshold, int release)
{
    const int nh = h.size();
    assert(nh > 0);
    _d = std::unique_ptr<detector_impl>(new detector_impl);
    _d->delay = zeros(nh + release);
    _d->fir_match = fir_fft(h / (nh * rms(h)));
    _d->block_size = _d->fir_match.block_size();
    _d->fir_pow = ave_fir(nh);
    _d->release_time = release;
    _d->threshold = threshold * threshold;
    _d->detect_size = nh;
}

//---------------------------------------------------------------------------------
detector::~detector()
{}

//---------------------------------------------------------------------------------
static detector::result _detecting(detector_impl* _d, const arr_cmplx& x)
{
    detector::result res;

    assert(x.size() == _d->block_size);

    //update delay
    auto delay = concatenate(_d->delay, x);
    _d->delay = delay.slice(delay.size() - _d->delay.size(), delay.size());

    //match fir line
    auto y_m = _d->fir_match.process(x);
    auto y_m2 = (real(y_m) ^ 2) + (imag(y_m) ^ 2);

    //power estimate line
    auto x2 = (real(x) ^ 2) + (imag(x) ^ 2);
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
detector::result detector::process(const arr_cmplx& x)
{
    detector::result res;
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
void detector::reset()
{
    _d->lock_time = 0;
    _d->current_max = 0;
    _d->triggered = false;
}

}   // namespace dsplib