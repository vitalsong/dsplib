#pragma once

#include <dsplib/array.h>
#include <memory>
#include <vector>

namespace dsplib {

//tune signal to freq (-fs/2 : +fs/2)
class tuner
{
public:
    tuner(int fs, int freq);
    arr_cmplx process(const arr_cmplx& x);
    void set_freq(int freq);
    int freq() const;

private:
    using sin_table_t = std::shared_ptr<std::vector<real_t>>;
    static sin_table_t get_table(int fs);

    int _fs;
    int _freq;
    int _phase;
    sin_table_t _tb;
};

}   // namespace dsplib
