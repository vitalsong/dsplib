#include <dsplib/tuner.h>

#include <map>
#include <numeric>
#include <vector>
#include <memory>

namespace dsplib {

namespace {

using sin_table_t = std::vector<real_t>;
using sin_table_ptr = std::shared_ptr<sin_table_t>;
using sin_table_wptr = std::weak_ptr<sin_table_t>;

sin_table_ptr _get_table(int fs) {
    thread_local std::map<int, sin_table_wptr> fs_table;
    sin_table_ptr ptr;
    if ((fs_table.count(fs) == 0) || ((fs_table.count(fs) == 1) && fs_table[fs].expired())) {
        ptr = std::make_shared<sin_table_t>(fs);
        real_t* data = ptr->data();
        for (int i = 0; i < fs; i++) {
            data[i] = sin(2 * pi * i / real_t(fs));
        }
        fs_table[fs] = sin_table_wptr(ptr);
    }
    return fs_table[fs].lock();
}

inline int _cycle_idx(int i, int n) noexcept {
    return (i < n) ? i : (i - n);
}

}   // namespace

Tuner::Tuner(int fs, int freq) {
    if (abs(freq) > fs / 2) {
        DSPLIB_THROW("freq is greater than half of sample rate");
    }

    const int m = std::gcd(fs, freq);
    fs /= m;
    freq /= m;

    while (fs % 4 != 0) {
        fs *= 2;
        freq *= 2;
    }

    _fs = fs;
    _freq = freq;

    _iphase = 0;
    if (_freq < 0) {
        _qphase = (_fs / 4) % _fs;
    } else {
        _qphase = (3 * _fs / 4) % _fs;
    }

    _tb = _get_table(fs);
}

arr_cmplx Tuner::process(const arr_cmplx& x) {
    if (_freq == 0) {
        return x;
    }

    const int freq = abs(_freq);
    cmplx_t w;
    arr_cmplx r(x.size());
    const real_t* tb = _tb->data();
    for (int i = 0; i < r.size(); i++) {
        w.re = tb[_iphase];
        w.im = tb[_qphase];
        r[i] = x[i] * w;
        _iphase = _cycle_idx(_iphase + freq, _fs);
        _qphase = _cycle_idx(_qphase + freq, _fs);
    }
    return r;
}

int Tuner::freq() const {
    return _freq;
}

}   // namespace dsplib