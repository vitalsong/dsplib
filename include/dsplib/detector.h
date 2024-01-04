#pragma once

#include <optional>
#include <stdexcept>
#include <memory>

#include <dsplib/array.h>

namespace dsplib {

class PreambleDetectorImpl;

// Preamble signal detector (only complex signal)
//
// Equivalent Matlab formula:
// function [res] = detector(x, h)
// nh = length(h);
// corr = filter(conj(h) / (nh * rms(h)), 1, x);
// pagg = filter(ones(1, nh) / nh, 1, abs(x).^2);
// res = sqrt((abs(corr) .^ 2) ./ pagg);
class PreambleDetector
{
public:
    explicit PreambleDetector(const arr_cmplx& h, real_t threshold = 0.5);

    PreambleDetector(const PreambleDetector&) = delete;

    struct Result
    {
        int offset{0};        ///< index of preamble end + 1 (relative to the input signal)
        arr_cmplx preamble;   ///< aligned preamble signal (todo: left/right pad)
        real_t score{0};      ///< detected peak level (>=threshold)
    };

    //TODO: add bypass mode
    //length of the 'sig' must be a multiple of 'frame_len()'
    std::optional<Result> process(const arr_cmplx& sig);

    std::optional<Result> operator()(const arr_cmplx& sig) {
        return this->process(sig);
    }

    [[nodiscard]] int frame_len() const noexcept;

    void reset();

private:
    std::shared_ptr<PreambleDetectorImpl> _d;
};

}   // namespace dsplib
