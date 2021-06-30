#include <dsplib/tuner.h>

#include <map>
#include <vector>
#include <memory>
#include <mutex>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
//TODO: combine tables for multiples freq / fs (example 100/8000 == 5/400)
tuner::sin_table_t tuner::get_table(int fs)
{
    static std::map<int, sin_table_t> fs_table;
    static std::mutex mutex;

    std::lock_guard<std::mutex> lk(mutex);
    if (fs_table.count(fs) == 0) {
        auto tb = std::make_shared<std::vector<double>>();
        tb->resize(fs);
        double* data = tb->data();
        for (size_t i = 0; i < fs; i++) {
            data[i] = sin(2 * M_PI * i / double(fs));
        }
        fs_table[fs] = tb;
    }

    return fs_table[fs];
}

//-------------------------------------------------------------------------------------------------
tuner::tuner(int fs, int freq)
{
    if (abs(freq) > fs / 2) {
        throw std::runtime_error("freq is greater than half of sample rate");
    }
    _fs = fs;
    _freq = freq;
    _phase = 0;
    _tb = get_table(fs);
}

//-------------------------------------------------------------------------------------------------
arr_cmplx tuner::process(const arr_cmplx& x)
{
    if (_freq == 0) {
        return x;
    }

    int iphase, qphase;

    if (_freq < 0) {
        iphase = _phase;
        qphase = (iphase + (_fs / 4)) % _fs;
    } else {
        iphase = _phase;
        qphase = (iphase + (3 * _fs / 4)) % _fs;
    }

    int freq = abs(_freq);

    cmplx_t w;
    arr_cmplx r(x.size());
    const double* tb = _tb->data();
    for (size_t i = 0; i < r.size(); i++) {
        w.re = tb[iphase];
        w.im = tb[qphase];
        r[i] = x[i] * w;
        iphase = (iphase + freq) % _fs;
        qphase = (qphase + freq) % _fs;
    }

    _phase = iphase;

    return r;
}

//-------------------------------------------------------------------------------------------------
void tuner::set_freq(int freq)
{
    if (abs(freq) > _fs / 2) {
        throw std::runtime_error("freq is greater than half of sample rate");
    }

    _freq = freq;
}

//-------------------------------------------------------------------------------------------------
int tuner::freq() const
{
    return _freq;
}

}   // namespace dsplib