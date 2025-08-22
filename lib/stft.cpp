#include <dsplib/stft.h>
#include <dsplib/window.h>
#include <dsplib/utils.h>
#include <dsplib/math.h>
#include <dsplib/fft.h>
#include <dsplib/ifft.h>

namespace dsplib {

namespace {

arr_cmplx _convert_range_stft(const arr_cmplx& x, int nfft, StftRange range) {
    DSPLIB_ASSERT(x.size() == nfft, "Input size must be equal `nfft`");
    if (range == StftRange::Onesided) {
        return x.slice(0, nfft / 2 + 1);
    }
    if (range == StftRange::Centered) {
        return concatenate(x.slice(nfft / 2 + 1, nfft), x.slice(0, nfft / 2 + 1));   //TODO: concat for slices
    }
    return x;
}

arr_cmplx _convert_range_istft(const arr_cmplx& x, int nfft, StftRange range) {
    if (range == StftRange::Onesided) {
        DSPLIB_ASSERT(x.size() == nfft / 2 + 1, "Input size must be equal `nfft/2+1` for `onesided` range");
        return concatenate(x, flip(conj(x.slice(1, nfft / 2))));
    }
    if (range == StftRange::Centered) {
        DSPLIB_ASSERT(x.size() == nfft, "Input size must be equal `nfft` for `centered` range");
        return concatenate(x.slice(nfft / 2 - 1, nfft), x.slice(0, nfft / 2 - 1));   //TODO: concat for slices
    }
    DSPLIB_ASSERT(x.size() == nfft, "Input size must be equal `nfft` for `twosided` range");
    return x;
}

}   // namespace

//------------------------------------------------------------------------------------------------------------------
bool iscola(const arr_real& win, int noverlap, OverlapMethod method) {
    const auto nwin = win.size();
    const auto hop = nwin - noverlap;

    const int pw = method == OverlapMethod::Ola ? 1 : 2;
    const int nsum = (int)std::floor(nwin / hop);
    auto cola_chk = zeros(hop);
    for (int i = 0; i < nsum; ++i) {
        const arr_real w = win.slice(i * hop, (i + 1) * hop);
        cola_chk += power(w, pw);
    }

    const int rm = nwin % hop;
    if (rm != 0) {
        cola_chk.slice(0, rm) += power(win.slice(nwin - rm, nwin), pw);
    }

    const auto nsumtotal = std::floor(nwin / hop) + double(rm != 0);
    const auto m = median(cola_chk);
    const auto max_dev = max(abs(cola_chk - m));
    const auto tf = max_dev < nsumtotal * eps();
    return tf;
}

//------------------------------------------------------------------------------------------------------------------
std::vector<arr_cmplx> stft(const arr_real& x, const arr_real& win, int overlap, int nfft, StftRange range) {
    std::vector<arr_cmplx> y;
    const int nx = x.size();
    const int nwin = win.size();
    const int hop = nwin - overlap;
    const int nseg = (nx - overlap) / (nwin - overlap);
    const auto fftp = fft_plan_r(nfft);
    arr_real px(nfft);
    for (int i = 0; i < nseg; ++i) {
        const int t1 = (i * hop);
        const int t2 = t1 + nwin;
        px.slice(0, nwin) = x.slice(t1, t2) * win;
        y.emplace_back(_convert_range_stft(fftp->solve(px), nfft, range));   //TODO: inplace
    }
    return y;
}

std::vector<arr_cmplx> stft(const arr_real& x, int nfft, StftRange range) {
    return stft(x, window::hann(nfft, false), nfft / 2, nfft, range);
}

//------------------------------------------------------------------------------------------------------------------
arr_real istft(const std::vector<arr_cmplx>& xx, const arr_real& win, int overlap, int nfft, StftRange range,
               OverlapMethod method) {
    const int nwin = win.size();
    const int hop = nwin - overlap;
    const int nseg = xx.size();
    const int xlen = nwin + (nseg - 1) * hop;

    const int a = method == OverlapMethod::Ola ? 0 : 1;
    auto norm_val = zeros(xlen);   ///TODO: too big array
    const auto win_nom = power(win, a);
    const auto win_den = power(win, a + 1);

    const auto irfftp = ifft_plan_r(nfft);
    arr_real x(xlen);
    for (int i = 0; i < nseg; ++i) {
        //TODO: copy optimmization for `_convert_range`
        const arr_real y = irfftp->solve(_convert_range_istft(xx[i], nfft, range)).slice(0, nwin);
        const int t1 = i * hop;
        const int t2 = t1 + nwin;
        //TODO: remove slice unpacking
        x.slice(t1, t2) += y * win_nom;
        norm_val.slice(t1, t2) += win_den;
    }

    //normalize
    //TODO: slice arithmetic, array view
    for (int i = 0; i < nseg; ++i) {
        norm_val[i] = norm_val[i] < nseg * eps() ? 1 : norm_val[i];
    }
    x /= norm_val;

    return x;
}

arr_real istft(const std::vector<arr_cmplx>& xx, int nfft, StftRange range, OverlapMethod method) {
    return istft(xx, window::hann(nfft, false), nfft / 2, nfft, range, method);
}

}   // namespace dsplib