#include "dsplib/array.h"
#include "dsplib/keywords.h"
#include "dsplib/math.h"
#include "dsplib/utils.h"

namespace dsplib {

namespace {

real_t _pearson_corr(const arr_real& x, const arr_real& y) noexcept {
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

std::vector<int> _get_ranks(const arr_real& x) noexcept {
    const int n = x.size();
    const auto [_, x_idx] = sort(x);
    std::vector<int> rank(n);
    for (int i = 0; i < n; ++i) {
        rank[x_idx[i]] = i;
    }
    return rank;
}

real_t _spearman_corr(const arr_real& x, const arr_real& y) noexcept {
    const auto x_rank = _get_ranks(x);
    const auto y_rank = _get_ranks(y);
    //FIXIT: valid only for unique values
    const auto r = _pearson_corr(x_rank, y_rank);
    return r;
}

//TODO: N * log(N) optimization (combine nc/nd calculation with sort)
real_t _kendall_corr(const arr_real& x, const arr_real& y) noexcept {
    const int n = x.size();
    const auto [_, x_idx] = sort(x);
    const auto ybyx = y[x_idx];

    int n_c = 0;
    int n_d = 0;
    for (int i = 0; i < (n - 1); ++i) {
        for (int k = (i + 1); k < n; ++k) {
            if (y[i] < y[k]) {
                ++n_c;
            } else {
                ++n_d;
            }
        }
    }

    const auto tau = real_t(n_c - n_d) / (n_c + n_d);
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
    }
    return 0;
}

}   // namespace dsplib