#include <dsplib/sinad.h>
#include <dsplib/fft.h>
#include <dsplib/window.h>
#include <dsplib/math.h>
#include <dsplib/utils.h>

namespace dsplib {

//------------------------------------------------------------------------------------
real_t sinad(const arr_real& x) {
    if (x.size() > 0x7fff) {
        throw std::runtime_error("The vector size is too large");
    }

    const int n = 1L << nextpow2(x.size());
    auto y = zeropad(x * window::gauss(x.size()), n);
    auto rfft = fft(y);

    real_t re, im;
    auto spectrum = zeros(n / 2);
    for (int i = 0; i < spectrum.size(); ++i) {
        re = rfft[i].re;
        im = rfft[i].im;
        spectrum[i] = ((re * re) + (im * im));
    }

    int freq_num = argmax(spectrum);

    int lpos = freq_num;
    while ((lpos != 0) && (spectrum[lpos] > spectrum[lpos - 1])) {
        --lpos;
    }

    int rpos = freq_num;
    while ((rpos != n / 2 - 1) && (spectrum[rpos] > spectrum[rpos + 1])) {
        ++rpos;
    }

    auto pow_signal = sum(spectrum.slice(lpos, rpos));
    auto pow_noise = sum(spectrum) - pow_signal;
    real_t sinad = 10 * ::log10(pow_signal / (pow_noise + 0.01));

    return sinad;
}

}   // namespace dsplib
