#include "dsplib/utils.h"
#include "dsplib/math.h"

namespace dsplib {

namespace {

int _left_descent(const dsplib::arr_real& x, int idx) {
    int lpos = max(idx, 0);
    while ((lpos > 0) && (x[lpos - 1] < x[lpos])) {
        --lpos;
    }
    return lpos;
}

int _right_descent(const dsplib::arr_real& x, int idx) {
    const int n = x.size();
    int rpos = min(idx, n - 1);
    while ((rpos < n - 1) && (x[rpos] > x[rpos + 1])) {
        ++rpos;
    }
    return rpos;
}

}   // namespace

PeakList findpeaks(arr_real data, int npeaks) {
    PeakList peaks;
    for (int i = 0; i < npeaks; ++i) {
        const int imax = argmax(data);
        const int lp = _left_descent(data, imax);
        const int rp = _right_descent(data, imax);
        peaks.locs.push_back(imax);
        peaks.pks.push_back(data[imax]);
        peaks.wds.push_back(rp - lp + 1);
        data.slice(lp, rp + 1) = 0;
    }
    return peaks;
}

}   // namespace dsplib