#include <dft-tables.h>
#include <dsplib/throw.h>
#include <dsplib/math.h>

#include <cmath>
#include <vector>
#include <memory>

namespace dsplib {
namespace tables {

//-------------------------------------------------------------------------------------------------
class BitrevTable
{
public:
    explicit BitrevTable(int n) noexcept
      : _base(n)
      , _n{n} {
        //TODO: table is symmetry, table(n/2, end) == table(0, n/2)+1
        const int s = nextpow2(n);
        for (int i = 0; i < n; ++i) {
            int k = _bitrev(i, s);
            _base[i] = k;
        }
    }

    explicit BitrevTable(const BitrevTable& rhs) = default;

    BitrevTable(BitrevTable&& rhs) noexcept
      : _n{rhs._n} {
        std::swap(rhs._base, _base);
    }

    BitrevTable& operator=(const BitrevTable& rhs) noexcept {
        if (this == &rhs) {
            return *this;
        }
        this->_base = rhs._base;
        this->_n = rhs._n;
        return *this;
    }

    [[nodiscard]] std::vector<int32_t> gen(int to_n) const noexcept {
        if (!convertable(to_n)) {
            return {};
        }

        if (to_n == _n) {
            return _base;
        }

        const int d = _n / to_n;
        std::vector<int32_t> r(to_n);
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
    static inline int _get_bit(int a, int pos) {
        return (a >> pos) & 0x1;
    }

    static inline void _set_bit(int& a, int pos, int bit) {
        a &= ~(1 << pos);
        a |= (bit << pos);
    }

    static inline int _bitrev(int a, int s) {
        int r = 0;
        for (int i = 0; i < ((s + 1) / 2); ++i) {
            _set_bit(r, (s - i - 1), _get_bit(a, i));
            _set_bit(r, i, _get_bit(a, (s - i - 1)));
        }
        return r;
    }

    std::vector<int32_t> _base;
    int _n{0};
};

//-------------------------------------------------------------------------------------------------
thread_local static BitrevTable g_base_bitrev(DEFAULT_MIN_NFFT);

//-------------------------------------------------------------------------------------------------
std::vector<int32_t> bitrev_table(size_t size) {
    if (g_base_bitrev.size() < size) {
        const int nfft = 1L << nextpow2(size);
        if (nfft != size) {
            DSPLIB_THROW("Table len is not power of 2");
        }
        g_base_bitrev = BitrevTable(size);
    }
    return g_base_bitrev.gen(size);
}

}   // namespace tables
}   // namespace dsplib
