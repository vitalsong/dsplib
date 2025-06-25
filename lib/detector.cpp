#include <dsplib/detector.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>
#include <dsplib/fir.h>
#include <dsplib/ma-filter.h>

#include <cassert>

namespace dsplib {

namespace {

bool _is_valid(const real_t& value) noexcept {
    return !(std::isinf(value) || std::isnan(value));
}

//cyclic buffer based delay
template<typename T>
class CDelay
{
public:
    explicit CDelay(int size)
      : _size{size}
      , _buf(size) {
    }

    void push(const T& v) noexcept {
        _buf[_idx] = v;
        _idx = (_idx + 1);
        _idx = (_idx == _size) ? 0 : _idx;
    }

    std::vector<T> extract() const noexcept {
        int p = _idx;
        std::vector<T> result(_size);
        for (int i = 0; i < _size; ++i) {
            result[i] = _buf[p];
            p = (p + 1) % _size;
        }
        return result;
    }

    void reset() noexcept {
        _idx = 0;
        std::fill(_buf.begin(), _buf.end(), T());
    }

private:
    int _idx{0};
    int _size{0};
    std::vector<T> _buf;
};

}   // namespace

//---------------------------------------------------------------------------------------------------------------
class PreambleDetectorImpl
{
public:
    explicit PreambleDetectorImpl(const arr_cmplx& h, real_t threshold)
      : _corr_flt{_convert_impulse(h)}
      , _pow_flt{h.size()}
      , _threshold{threshold * threshold}
      , _delay{h.size()} {
    }

    std::optional<PreambleDetector::Result> process(const arr_cmplx& sig) {
        if (sig.size() % frame_len() != 0) {
            DSPLIB_THROW("Frame len not supported");
        }

        const auto cx = _corr_flt.process(sig);
        const auto pwx = _pow_flt.process(abs2(sig));
        const auto corr = abs2(cx) / (pwx + eps());
        for (int i = 0; i < corr.size(); ++i) {
            _delay.push(sig[i]);
            if ((corr[i] > _threshold) && _is_valid(corr[i])) {
                PreambleDetector::Result res;
                res.offset = i;
                res.preamble = _delay.extract();
                res.score = std::sqrt(corr[i]);
                return res;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] int frame_len() const noexcept {
        return _corr_flt.block_size();
    }

    void reset() {
        _pow_flt.process(zeros(frame_len()));
        _corr_flt.process(zeros(frame_len()));
        _delay.reset();
    }

private:
    static arr_cmplx _convert_impulse(const arr_cmplx& h) {
        return flip(h) / (rms(h) * h.size());
    }

    FftFilter _corr_flt;
    MAFilterR _pow_flt;
    real_t _threshold{1.0};
    CDelay<cmplx_t> _delay;
};

//---------------------------------------------------------------------------------------------------------------
PreambleDetector::PreambleDetector(const arr_cmplx& h, real_t threshold) {
    _d = std::make_shared<PreambleDetectorImpl>(h, threshold);
}

std::optional<PreambleDetector::Result> PreambleDetector::process(const arr_cmplx& sig) {
    return _d->process(sig);
}

[[nodiscard]] int PreambleDetector::frame_len() const noexcept {
    return _d->frame_len();
}

void PreambleDetector::reset() {
    return _d->reset();
}

}   // namespace dsplib