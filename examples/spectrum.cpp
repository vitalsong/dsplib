#include <plot.h>

namespace plt = dsplib::plot;
namespace dsp = dsplib;

int main()
{
    int nfft = 512;
    auto v = dsp::range(0, nfft) * 2 * M_PI * 440 / 8000;
    auto x = dsp::expj(v) * 1000;
    x = dsp::awgn(x, 10);
    x *= dsp::window::hamming(nfft);
    auto y = dsp::fft(x) / nfft;
    auto z = dsp::log10(dsp::abs(y) / 0x7FFF) * 20;

    plt::title("Spectrum");
    plt::plot(z);
    plt::show();
}
