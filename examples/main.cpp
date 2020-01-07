#include <plot.h>

namespace plt = dsplib::plot;
namespace dsp = dsplib;

//--------------------------------------------------------------------------------
static void spectrum_example()
{
    int nfft = 512;
    auto v = dsp::range(0, nfft) * 2 * M_PI * 440 / 8000;
    auto x = dsp::expj(v) * 1000;
    x = dsp::awgn(x, 10);
    x *= dsp::window::hamming(nfft);
    auto y = dsp::fft(x) / nfft;
    auto z = dsp::log10(dsp::abs(y) / 0x7FFF) * 20;

    plt::title("Spectrum Example");
    plt::plot(z);
    plt::show();
}

//--------------------------------------------------------------------------------
static void medfilt_example()
{
    int fs = 100;
    auto t = dsp::range(0, 1, 1.0/fs);
    auto x = dsp::sin(t*2*M_PI*3) + dsp::sin(t*2*M_PI*40)*0.25;
    auto y = dsp::medfilt::process(x, 10);

    plt::title("Medfilt Example");
    plt::plot(t, x);
    plt::plot(t, y);
    plt::show();
}

//--------------------------------------------------------------------------------
static void xcorr_example()
{
    int n = 1000;
    auto x1 = dsp::randn(n) * 100;
    auto x2 = dsp::awgn(x1, 10);
    auto y = dsp::xcorr(x1, x2);

    plt::title("XCorr Example");
    plt::plot(y);
    plt::show();
}

//--------------------------------------------------------------------------------
static void fir_example()
{
    const double IR[4] = {0.25, 0.25, 0.25, 0.25};

    //filter
    auto flt = dsp::fir(dsp::arr_real(IR, 4));
    auto x_in = dsp::randn(10000) * 100;
    auto x_out = flt.filter(x_in);

    //view spectrum
    int nfft = 1024;
    auto r = x_out.slice(0, nfft-1) * dsp::window::hamming(nfft);
    auto y = dsp::fft(r) / (nfft/2);
    auto z = dsp::log10(dsp::abs(y) / 0x7FFF) * 20;

    plt::title("LP FIR Example");
    plt::plot(z);
    plt::show();
}

//--------------------------------------------------------------------------------
static void hilbert_example()
{
    int fs = 8000;
    int n = 1000;
    auto t = dsp::range(0, n) * 2 * M_PI * 40 / fs;
    auto x = dsp::sin(t) * 100;
    auto y = dsp::hilbert::process(x);

    plt::title("Hilbert Example");
    plt::plot(dsp::real(y));
    plt::plot(dsp::imag(y));
    plt::show();
}

//--------------------------------------------------------------------------------
int main()
{
    spectrum_example();
    medfilt_example();
    xcorr_example();
    fir_example();
    hilbert_example();
}
