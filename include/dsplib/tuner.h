#pragma once

#include <dsplib/array.h>
#include <memory>
#include <vector>

namespace dsplib {

//tune signal to freq (-fs/2 : +fs/2)
class Tuner
{
public:
    explicit Tuner(int fs, int freq);
    arr_cmplx process(const arr_cmplx& x);
    [[nodiscard]] int freq() const;

    arr_cmplx operator()(const arr_cmplx& x) {
        return this->process(x);
    }

private:
    int _fs;
    int _freq;
    int _iphase{0};
    int _qphase{0};
    std::shared_ptr<std::vector<real_t>> _tb;
};

using tuner [[deprecated]] = Tuner;

}   // namespace dsplib
