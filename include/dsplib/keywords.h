#pragma once

namespace dsplib {

enum class Direction : bool
{
    Forward = true,
    Reverse = false,
    Ascend = true,
    Descend = false,
};

enum class FilterType
{
    Low,
    High,
    Bandpass,
    Bandstop
};

enum class FirType : int
{
    NonlinearPhase = 0,   //Nonlinear phase
    EvenSymm = 1,         //Even-order symmetric coefficients
    OddSym = 2,           //Odd-order symmetric coefficients
    EvenAntiSym = 3,      //Even-order antisymmetric coefficients
    OddAntiSym = 4        //Odd-order antisymmetric coefficients
};

enum class Correlation
{
    Pearson,
    Spearman,
    Kendall
};

enum class SpectrumType
{
    Psd = 0,
    Power
};

enum class WindowType
{
    Rectangle,
    Sinus,
    Hann,
    Hamming,
    Blackman,
    Gauss,
    Blackmanharris,
    Kaiser
};

}   // namespace dsplib