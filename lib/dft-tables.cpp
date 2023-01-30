#include <dft-tables.h>
#include <dsplib/throw.h>
#include <dsplib/math.h>

#include <cmath>
#include <vector>
#include <memory>

namespace dsplib {
namespace tables {

//-------------------------------------------------------------------------------------------------
class ExpTable
{
public:
    explicit ExpTable(int n) noexcept
      : _base(n)
      , _n{n} {
        real_t p;
        for (size_t i = 0; i < _n; ++i) {
            p = i / real_t(_n);
            //TODO: only sin operation required (N/8 optimization)
            _base[i].re = std::cos(2 * pi * p);
            _base[i].im = -std::sin(2 * pi * p);
        }
    }

    explicit ExpTable(const ExpTable& rhs) = default;

    ExpTable(ExpTable&& rhs) noexcept
      : _n{rhs._n} {
        std::swap(rhs._base, _base);
    }

    ExpTable& operator=(const ExpTable& rhs) noexcept {
        if (this == &rhs) {
            return *this;
        }
        this->_base = rhs._base;
        this->_n = rhs._n;
        return *this;
    }

    [[nodiscard]] arr_cmplx gen(int to_n) const noexcept {
        if (!convertable(to_n)) {
            return {};
        }

        if (to_n == _n) {
            return arr_cmplx(_base);
        }

        const int d = _n / to_n;
        arr_cmplx r(to_n);
        assert(d > 0);
        for (int i = 0; i < to_n; ++i) {
            r[i] = _base[i * d];
        }

        return r;
    }

    [[nodiscard]] bool convertable(int to_n) const noexcept {
        return ((to_n > 0) && (_n % to_n == 0));
    }

    [[nodiscard]] int size() const noexcept {
        return _n;
    }

private:
    std::vector<cmplx_t> _base;
    int _n{0};
};

//-------------------------------------------------------------------------------------------------
//TODO: optional disable caching
thread_local static ExpTable g_base_dft(DEFAULT_MIN_NFFT);

//-------------------------------------------------------------------------------------------------
dsplib::arr_cmplx dft_table(size_t size) {
    if (g_base_dft.size() < size) {
        const int nfft = 1L << nextpow2(size);
        if (nfft != size) {
            DSPLIB_THROW("Table len is not power of 2");
        }
        g_base_dft = ExpTable(size);
    }
    return g_base_dft.gen(size);
}

}   // namespace tables
}   // namespace dsplib
