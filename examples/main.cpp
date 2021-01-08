#include <matplot/matplot.h>
#include <dsplib.h>

namespace dsp = dsplib;

//fs = 2000Hz, start = 200Hz, stop = 300Hz, attenuation = -80dB
static const int IR_LEN = 45;
static const double IR[IR_LEN] = {
  -0.00038208231336125866, -0.0014251618131260284, -0.0034611155687191593, -0.006377681668456472,
  -0.009338694093439029,   -0.010721539707974506,  -0.008542541437724895,  -0.0013790909582854624,
  0.010515511659775947,    0.024478484368800194,   0.03568818871259766,    0.038677302188105625,
  0.029782915282474908,    0.009586710099600326,   -0.015891560978081032,  -0.0361941327639497,
  -0.03970916681917197,    -0.0182415126115788,    0.028935349277648056,   0.09341675013735361,
  0.15945198560909052,     0.20882683425444856,    0.22711048813297713,    0.20882683425444856,
  0.15945198560909052,     0.09341675013735361,    0.028935349277648056,   -0.0182415126115788,
  -0.03970916681917197,    -0.0361941327639497,    -0.015891560978081032,  0.009586710099600326,
  0.029782915282474908,    0.038677302188105625,   0.03568818871259766,    0.024478484368800194,
  0.010515511659775947,    -0.0013790909582854624, -0.008542541437724895,  -0.010721539707974506,
  -0.009338694093439029,   -0.006377681668456472,  -0.0034611155687191593, -0.0014251618131260284,
  -0.00038208231336125866};

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

    matplot::title("Spectrum Example");
    matplot::plot(z)->line_width(1.5);
    matplot::show();
}

//--------------------------------------------------------------------------------
static void medfilt_example()
{
    int fs = 100;
    auto t = dsp::range(0, 1, 1.0 / fs);
    auto x = dsp::sin(t * 2 * M_PI * 3) + dsp::sin(t * 2 * M_PI * 40) * 0.25;
    auto y = dsp::medfilt(x, 10);

    matplot::title("Medfilt Example");
    matplot::plot(t, x)->line_width(1.5);
    matplot::hold(matplot::on);
    matplot::plot(t, y)->line_width(1.5);
    matplot::show();
    matplot::hold(matplot::off);
}

//--------------------------------------------------------------------------------
static void xcorr_example()
{
    int n = 1000;
    auto x1 = dsp::randn(n) * 100;
    auto x2 = dsp::awgn(x1, 10);
    auto y = dsp::xcorr(x1, x2);

    matplot::title("XCorr Example");
    matplot::plot(y)->line_width(1.5);
    matplot::show();
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

    matplot::title("LP FIR Example");
    matplot::plot(x_in)->line_width(1.5);
    matplot::hold(matplot::on);
    matplot::plot(x_out)->line_width(1.5);
    matplot::show();
    matplot::hold(matplot::off);
}

//--------------------------------------------------------------------------------
static void hilbert_example()
{
    int fs = 8000;
    int n = 1000;
    auto t = dsp::range(0, n) * 2 * M_PI * 40 / fs;
    auto x = dsp::sin(t) * 100;
    auto y = dsp::hilbert(x);

    matplot::title("Hilbert Example");
    matplot::plot(dsp::real(y))->line_width(1.5);
    matplot::hold(matplot::on);
    matplot::plot(dsp::imag(y))->line_width(1.5);
    matplot::show();
    matplot::hold(matplot::off);
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

    matplot::title("Interp 4x Example");
    dsp::arr_real px = dsp::range(0, n * m, m);
    dsp::arr_real py = x_in.slice(0, n);
    matplot::plot(px, py)->line_width(1.5);
    matplot::hold(matplot::on);
    py = x_out.slice(0, n * m);
    matplot::plot(py)->line_width(1.5);
    matplot::show();
    matplot::hold(matplot::off);
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
