#include <dsplib/snr.h>
#include <dsplib/fft.h>
#include <dsplib/window.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

#include <utility>
#include <vector>

namespace dsplib {

namespace {

//TODO: disable limits
constexpr int PERIODOGRAM_SIZE_LIMIT = (int(1) << 18);

struct ToneInfo
{
    int lpos{0};
    int rpos{0};
    int size{0};
    real_t freq{0};
    real_t power{0};
};

int _locate_peak(const dsplib::arr_real& spec, int idx) {
    assert(idx < spec.size());
    int peak = idx;
    const int n = spec.size();
    while ((peak > 0) && (spec[peak - 1] > spec[peak])) {
        --peak;
    }
    while ((peak < n - 1) && (spec[peak] < spec[peak + 1])) {
        ++peak;
    }
    return peak;
}

int _left_descent(const dsplib::arr_real& spec, int idx) {
    int lpos = max(idx, 0);
    while ((lpos > 0) && (spec[lpos - 1] < spec[lpos])) {
        --lpos;
    }
    return lpos;
}

int _right_descent(const dsplib::arr_real& spec, int idx) {
    const int n = spec.size();
    int rpos = min(idx, n - 1);
    while ((rpos < n - 1) && (spec[rpos] > spec[rpos + 1])) {
        ++rpos;
    }
    return rpos;
}

ToneInfo _get_psd_tone(const dsplib::arr_real& spec, real_t tone_freq) {
    const int n = spec.size();
    int freq_num = std::round(tone_freq * n);
    freq_num = min(freq_num, spec.size() - 1);
    freq_num = max(freq_num, 0);

    const int ipeak = _locate_peak(spec, freq_num);
    const int lpos = _left_descent(spec, ipeak);
    const int rpos = _right_descent(spec, ipeak);

    const arr_real f_fund = arange(lpos, rpos + 1) / n;
    const arr_real s_fund = spec.slice(lpos, rpos + 1);
    const auto freq = dot(f_fund, s_fund) / sum(s_fund);

    ToneInfo info;
    info.size = n;
    info.lpos = lpos;
    info.rpos = rpos;
    info.freq = freq;
    info.power = sum(s_fund);

    return info;
}

ToneInfo _get_psd_tone(const dsplib::arr_real& spec) {
    const int n = spec.size();
    const real_t tone_freq = argmax(spec) / real_t(n);
    return _get_psd_tone(spec, tone_freq);
}

real_t _alias_to_nyquist(real_t f, real_t fs) {
    auto tone_f = std::fmod(f, fs);
    if (tone_f > (fs / 2)) {
        return fs - tone_f;
    }
    return tone_f;
}

struct HarmInfo
{
    arr_real harmpow;
    arr_real harmfreq;
    real_t noisepow;
};

HarmInfo _harm_analyze(arr_real spectrum, int nharm, bool aliased = false) {
    auto harm_pow = zeros(nharm);
    auto harm_freq = zeros(nharm);

    std::vector<ToneInfo> tones;

    //detect fundamental harmonic
    const auto tn = _get_psd_tone(spectrum);
    harm_pow[0] = tn.power;
    harm_freq[0] = tn.freq;
    spectrum.slice(tn.lpos, tn.rpos + 1) = 0;
    tones.push_back(tn);

    //detect other harmonics
    for (int i = 1; i < nharm; ++i) {
        auto freq = (i + 1) * harm_freq[0];
        if (aliased) {
            freq = _alias_to_nyquist(freq, 2.0);
        }
        if (freq > 1.0) {
            break;
        }
        const auto tn = _get_psd_tone(spectrum, freq);
        harm_pow[i] = tn.power;
        harm_freq[i] = tn.freq;
        spectrum.slice(tn.lpos, tn.rpos + 1) = 0;
        tones.push_back(tn);
    }

    //filling voids after removing harmonics (for noise)
    //TODO: fix small values for N/2+1 array size with zeropad to N
    const auto idxs = (spectrum > 0);
    const auto noise_floor = (sum(idxs) > 0) ? median(spectrum[idxs]) : 0;
    for (const auto& tn : tones) {
        spectrum.slice(tn.lpos, tn.rpos + 1) = noise_floor;
    }

    HarmInfo res;
    res.harmpow = harm_pow;
    res.harmfreq = (harm_freq / 2);
    res.noisepow = sum(spectrum);
    return res;
}

//TODO: add standard periodogram function
arr_real _periodogram(const arr_real& sig) {
    DSPLIB_ASSERT(sig.size() <= PERIODOGRAM_SIZE_LIMIT,
                  "The input vector is too large. Use welch() to calculate spectrum.")

    //remove DC
    auto x = sig - mean(sig);

    //use kaiser window
    auto w = window::kaiser(x.size(), 38);

    //compensates for the power of the window
    w /= rms(w);

    //calculate spectrum
    const int n = 1 << nextpow2(sig.size());
    const real_t u = real_t(sig.size()) * n / 2;
    const arr_cmplx rfft = fft(x * w, n).slice(0, n / 2);
    const arr_real spec = abs2(rfft) / u;
    return spec;
}

}   // namespace

//------------------------------------------------------------------------------------
real_t sinad(const arr_real& sig, SinadType type) {
    const auto pxx = (type == SinadType::Time) ? _periodogram(sig) : sig;
    auto info = _harm_analyze(pxx, 1);
    return pow2db(info.harmpow[0] / info.noisepow);
}

ThdRes thd(const arr_real& sig, int nharm, bool aliased, SinadType type) {
    DSPLIB_ASSERT(nharm > 1, "number of harmonics must be 2 or greater");
    const auto pxx = (type == SinadType::Time) ? _periodogram(sig) : sig;
    const auto info = _harm_analyze(pxx, nharm, aliased);
    ThdRes res;
    const auto harm_sum = sum(info.harmpow.slice(1, nharm));
    res.value = pow2db(harm_sum / info.harmpow[0]);
    res.harmpow = pow2db(info.harmpow);
    res.harmfreq = info.harmfreq;
    return res;
}

real_t snr(const arr_real& sig, int nharm, bool aliased, SinadType type) {
    const auto pxx = (type == SinadType::Time) ? _periodogram(sig) : sig;
    auto info = _harm_analyze(pxx, nharm, aliased);
    return pow2db(info.harmpow[0] / info.noisepow);
}

}   // namespace dsplib
