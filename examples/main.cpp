#include <matplot/matplot.h>
#include <dsplib.h>
#include <set>

namespace dsp = dsplib;

namespace matplot {

//---------------------------------------------------------------------------------
auto plot(dsp::arr_real& x, std::initializer_list<dsp::arr_real> y)
{
    std::vector<std::vector<double>> yy;
    for (auto dy : y) {
        std::vector<double> xx(dy.begin(), dy.end());
        yy.push_back(xx);
    }
    return plot(x, yy);
}

//---------------------------------------------------------------------------------
auto plot(std::initializer_list<dsp::arr_real> y)
{
    std::vector<std::vector<double>> yy;
    for (auto dy : y) {
        std::vector<double> xx(dy.begin(), dy.end());
        yy.push_back(xx);
    }
    return plot(yy);
}

}   // namespace matplot

//fs = 2000Hz, start = 200Hz, stop = 300Hz, attenuation = -80dB
static dsp::arr_real IR = {
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
    const int fs = 8000;
    const int nfft = 512;
    auto v = dsp::range(0, nfft) * 2 * dsp::pi * 440 / fs;
    auto x = dsp::expj(v) * 1000;
    x = dsp::awgn(x, 10);
    x *= dsp::window::hamming(nfft);
    auto y = dsp::fft(x) / nfft;
    y = y.slice(0, nfft / 2);
    auto z = 20 * dsp::log10(dsp::abs(y) / 0x7FFF);
    auto freqs = dsp::range(nfft / 2) * fs / nfft;
    matplot::title("Spectrum Example");
    matplot::plot(freqs, z);
    matplot::show();
}

//--------------------------------------------------------------------------------
static void medfilt_example()
{
    int fs = 100;
    auto t = dsp::range(0, 1, 1.0 / fs);
    auto x = dsp::sin(t * 2 * dsp::pi * 3) + dsp::sin(t * 2 * dsp::pi * 40) * 0.25;
    auto y = dsp::medfilt(x, 10);

    matplot::title("Medfilt Example");
    matplot::plot(t, x, t, y);
    matplot::legend({"Input", "Output"});
    matplot::show();
}

//--------------------------------------------------------------------------------
static void xcorr_example()
{
    int n = 1000;
    auto x1 = dsp::randn(n) * 100;
    auto x2 = dsp::awgn(x1, 10);
    auto y = dsp::xcorr(x1, x2);

    matplot::title("XCorr Example");
    matplot::plot(y);
    matplot::legend("xcorr");
    matplot::show();
}

//--------------------------------------------------------------------------------
static void fir_example()
{
    int fs = 2000;
    int n = 200;

    auto flt = dsp::fir(IR);
    auto t = dsp::range(0, n) * 2 * dsp::pi * 50 / fs;
    auto x_in = dsp::sin(t) * 100 + dsp::randn(n) * 10;
    auto x_out = flt.process(x_in);

    matplot::title("LP FIR Example");
    matplot::plot({x_in, x_out});
    matplot::legend({"Input", "Output"});
    matplot::show();
}

//--------------------------------------------------------------------------------
static void hilbert_example()
{
    int fs = 8000;
    int n = 1000;
    auto t = dsp::range(0, n) * 2 * dsp::pi * 40 / fs;
    auto x = dsp::sin(t) * 100;
    auto y = dsp::hilbert(x);

    matplot::title("Hilbert Example");
    dsp::arr_real y1 = dsp::real(y);
    dsp::arr_real y2 = dsp::imag(y);
    matplot::plot({y1, y2});
    matplot::legend({"Re", "Im"});
    matplot::show();
}

//--------------------------------------------------------------------------------
static void interp_example()
{
    int fs = 2000;
    int n = 30;
    int m = 4;

    auto flt = dsp::interp_filter(IR, m);
    auto t = dsp::range(0, n) * 2 * dsp::pi * 200 / fs;
    auto x_in = dsp::sin(t) * 100;
    auto x_out = flt.process(x_in);

    matplot::title("Interp 4x Example");
    dsp::arr_real x1 = dsp::range(0, n * m, m);
    dsp::arr_real x2 = dsp::range(0, n * m);
    dsp::arr_real y1 = x_in.slice(0, n);
    dsp::arr_real y2 = x_out.slice(0, n * m);
    matplot::plot(x1, y1, x2, y2);
    matplot::legend({"Original", "Interp"});
    matplot::show();
}

//--------------------------------------------------------------------------------
static void lms_example()
{
    int M = 100;
    int L = 10000;
    auto flt = dsp::fir(IR);
    auto t = dsp::range(0, L) / 1000;
    auto s = dsp::sin(2 * dsp::pi * 3 * t) + dsp::sin(2 * dsp::pi * 4 * t);
    auto x = 10 * dsp::randn(L);
    auto d = flt.process(x) + s;

    auto mu_max = 2 / ((M + 1) * dsp::mean(x * x));
    auto mu = 0.05 * mu_max;
    auto lms = dsp::lms(M, mu);
    auto [y, e, w] = lms.process(x, d);

    matplot::title("LMS");
    matplot::plot({s, e});
    matplot::legend({"Desired", "Output"});
    matplot::show();
}

//--------------------------------------------------------------------------------
static void tuner_example()
{
    int fs = 8000;
    int n = 2048;

    dsplib::tuner tuner1(fs, 1000);
    dsplib::tuner tuner2(fs, -1000);

    auto t = dsp::range(n) / fs;

    auto y1 = 100 * dsp::expj(2 * dsp::pi * 440 * t);   ///< 440 Hz
    auto y2 = tuner1.process(y1);                    ///< 1440 Hz
    auto y3 = tuner2.process(y1);                    ///< -560 Hz

    auto z1 = 20 * dsp::log10(dsp::abs(dsp::fft(y1)));
    auto z2 = 20 * dsp::log10(dsp::abs(dsp::fft(y2)));
    auto z3 = 20 * dsp::log10(dsp::abs(dsp::fft(y3)));

    auto freqs = dsp::range(n) * fs / n;
    matplot::title("Tuner Example");
    matplot::plot(freqs, {z1, z2, z3});
    matplot::legend({"Original", "+1000Hz", "-1000Hz"});
    matplot::show();
}

//--------------------------------------------------------------------------------
void agc_example()
{
    auto agc = dsp::agc(1, 30, 100, 0.01, 0.01);
    auto t = dsp::range(10000) / 8000;
    auto x = 10 * dsp::sin(2 * dsp::pi * 440 * t);
    auto [y, g] = agc.process(x);
    matplot::title("Agc example");
    matplot::plot({x, y});
    matplot::legend({"Input", "Output"});
    matplot::show();
}

//--------------------------------------------------------------------------------
int main()
{
    agc_example();
    lms_example();
    spectrum_example();
    medfilt_example();
    xcorr_example();
    fir_example();
    hilbert_example();
    interp_example();
    tuner_example();
}
