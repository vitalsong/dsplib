#include <dsplib/math.h>
#include <array>

namespace dsplib {

namespace {

constexpr std::array<uint8_t, 54> PRIMES = {2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,
                                            47,  53,  59,  61,  67,  71,  73,  79,  83,  89,  97,  101, 103, 107,
                                            109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
                                            191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251};

//simple primes generator
//warning: may be slow for big numbers (>2^20)
class PrimesGenerator
{
public:
    PrimesGenerator() {
        _primes.assign(PRIMES.begin(), PRIMES.end());
    }

    uint32_t next() noexcept {
        if (_pos + 1 == _primes.size()) {
            _add_primes();
        }
        ++_pos;
        return current();
    }

    uint32_t next_prime(uint32_t n) noexcept {
        while (current() < n) {
            next();
        }
        return current();
    }

    uint32_t current() const noexcept {
        return _primes[_pos];
    }

private:
    bool _is_prime(uint32_t n) const noexcept {
        for (auto d : _primes) {
            if (d * d > n) {
                break;
            }
            if (n % d == 0) {
                return false;
            }
        }
        return true;
    }

    void _add_primes() noexcept {
        uint32_t val = current() + 2;
        while (!_is_prime(val)) {
            val += 2;
        }
        _primes.push_back(val);
    }

    uint32_t _pos{0};
    std::vector<uint32_t> _primes;
};

}   // namespace

uint32_t nextprime(uint32_t n) {
    PrimesGenerator gen;
    return gen.next_prime(n);
}

arr_int primes(uint32_t n) {
    std::vector<int32_t> res;
    PrimesGenerator gen;
    while (gen.current() <= n) {
        res.push_back(gen.current());
        gen.next();
    }
    return res;
}

bool isprime(uint32_t n) noexcept {
    if (n < 2) {
        return false;
    }

    if ((n > 5) && ((n % 2 == 0) || (n % 3 == 0) || (n % 5 == 0))) {
        return false;
    }

    if (n <= PRIMES[PRIMES.size() - 1]) {
        return std::find(PRIMES.begin(), PRIMES.end(), n) != PRIMES.end();
    }

    PrimesGenerator gen;
    auto d = gen.current();
    while (d * d <= n) {
        if (n % d == 0) {
            return false;
        }
        d = gen.next();
    }

    return true;
}

arr_int factor(uint32_t n) {
    if (n <= 3) {
        return arr_int({static_cast<int>(n)});
    }

    std::vector<int> res;
    PrimesGenerator gen;
    uint32_t d = gen.current();
    while (d * d <= n) {
        while (n % d == 0) {
            n /= d;
            res.push_back(d);
        }
        d = gen.next();
    }
    if (n > 1) {
        res.push_back(n);
    }
    return res;
}

}   // namespace dsplib