#include "dsplib/array.h"
#include "dsplib/keywords.h"
#include "dsplib/math.h"
#include "dsplib/utils.h"

namespace dsplib {

namespace {

real_t _pearson_corr(const arr_real& x, const arr_real& y) {
    const int n = x.size();
    real_t sum_x = 0;
    real_t sum_y = 0;
    real_t sum_xy = 0;
    real_t sqsum_x = 0;
    real_t sqsum_y = 0;
    for (int i = 0; i < n; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sqsum_x += x[i] * x[i];
        sqsum_y += y[i] * y[i];
    }
    sum_xy *= n;
    sqsum_x *= n;
    sqsum_y *= n;
    const real_t den = sum_xy - (sum_x * sum_y);
    const real_t num = std::sqrt((sqsum_x - sum_x * sum_x) * (sqsum_y - sum_y * sum_y));
    const real_t corr = den / num;
    return corr;
}

arr_int _get_ranks(const arr_real& x) {
    const int n = x.size();
    arr_int x_idx{arange(n)};
    std::sort(x_idx.begin(), x_idx.end(), [&](int i, int j) {
        return (x[i] < x[j]);
    });
    arr_int rank(n);
    for (int i = 0; i < n; ++i) {
        rank[x_idx[i]] = i;
    }
    return rank;
}

real_t _spearman_corr(const arr_real& x, const arr_real& y) {
    const int n = x.size();
    const arr_int x_rank = _get_ranks(x);
    const arr_int y_rank = _get_ranks(y);
    const arr_real d2 = pow2(x_rank - y_rank);
    //FIXIT: only for unique values
    const auto r = 1 - (6 * sum(d2)) / (n * (n * n - 1));
    return r;
}

real_t _kendall_corr(const arr_real& x, const arr_real& y) {
    const int n = x.size();
    const arr_int x_rank = _get_ranks(x);
    arr_int y_rank = _get_ranks(y);
    for (int i = 0; i < n; ++i) {
        int k = x_rank[i];
        std::swap(y_rank[i], y_rank[k]);
    }

    int C = 0;
    int D = 0;
    for (int i = 0; i < (n - 1); ++i) {
        for (int k = i + 1; k < n; ++k) {
            if (y_rank[i] < y_rank[k]) {
                ++C;
            } else {
                ++D;
            }
        }
    }

    const auto tau = real_t(C - D) / (C + D);
    return tau;
}

}   // namespace

real_t corr(const arr_real& x, const arr_real& y, Correlation type) {
    DSPLIB_ASSERT(x.size() == y.size(), "Array size must be equal");
    switch (type) {
    case Correlation::Pearson:
        return _pearson_corr(x, y);
    case Correlation::Spearman:
        return _spearman_corr(x, y);
    case Correlation::Kendall:
        return _kendall_corr(x, y);
    default:
        return 0;
    }
}

}   // namespace dsplib