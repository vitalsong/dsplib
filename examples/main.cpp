#include <plot.h>

namespace plt = dsplib::plot;
namespace dsp = dsplib;

//fs = 2000Hz, start = 200Hz, stop = 300Hz, attenuation = -80dB
static const int IR_LEN = 45;
static const double IR[IR_LEN] =
{
    -0.00038208231336125866, -0.0014251618131260284, -0.0034611155687191593, -0.006377681668456472, -0.009338694093439029,
    -0.010721539707974506, -0.008542541437724895, -0.0013790909582854624, 0.010515511659775947, 0.024478484368800194,
    0.03568818871259766, 0.038677302188105625, 0.029782915282474908, 0.009586710099600326, -0.015891560978081032,
    -0.0361941327639497, -0.03970916681917197, -0.0182415126115788, 0.028935349277648056, 0.09341675013735361,
    0.15945198560909052, 0.20882683425444856, 0.22711048813297713, 0.20882683425444856, 0.15945198560909052,
    0.09341675013735361, 0.028935349277648056, -0.0182415126115788, -0.03970916681917197, -0.0361941327639497,
    -0.015891560978081032, 0.009586710099600326, 0.029782915282474908, 0.038677302188105625, 0.03568818871259766,
    0.024478484368800194, 0.010515511659775947, -0.0013790909582854624, -0.008542541437724895, -0.010721539707974506,
    -0.009338694093439029, -0.006377681668456472, -0.0034611155687191593, -0.0014251618131260284, -0.00038208231336125866
};

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
    auto y = dsp::medfilt(x, 10);

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
    int fs = 2000;
    int n = 200;

    dsp::arr_real h(IR, IR_LEN);
    auto flt = dsp::fir(h);
    auto t = dsp::range(0, n) * 2 * M_PI * 50 / fs;
    auto x_in = dsp::sin(t) * 100 + dsp::randn(n) * 10;
    auto x_out = flt.process(x_in);

    plt::title("LP FIR Example");
    plt::plot(x_in);
    plt::plot(x_out);
    plt::show();
}

//--------------------------------------------------------------------------------
static void hilbert_example()
{
    int fs = 8000;
    int n = 1000;
    auto t = dsp::range(0, n) * 2 * M_PI * 40 / fs;
    auto x = dsp::sin(t) * 100;
    auto y = dsp::hilbert(x);

    plt::title("Hilbert Example");
    plt::plot(dsp::real(y));
    plt::plot(dsp::imag(y));
    plt::show();
}

//--------------------------------------------------------------------------------
static void interp_example()
{
    int fs = 2000;
    int n = 30;
    int m = 4;

    dsp::arr_real h(IR, IR_LEN);
    auto flt = dsp::interp_filter(h, m);
    auto t = dsp::range(0, n) * 2 * M_PI * 200 / fs;
    auto x_in = dsp::sin(t) * 100;
    auto x_out = flt.process(x_in);

    plt::title("Interp 4x Example");
    plt::plot(dsp::range(0, n * m, m), x_in.slice(0, n));
    plt::plot(x_out.slice(0, n * m));
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
    interp_example();
}
