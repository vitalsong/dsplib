#pragma once

#include <dsplib/math.h>
#include <dsplib/array.h>

namespace dsplib {

/**
 * @brief Moving Average Filter implementation using binary tree summation
 * @tparam T Data type for filter processing (real_t or cmplx_t recommended)
 * 
 * @details
 * This class implements a computationally efficient moving average (MA) filter 
 * using a binary tree structure for O(log N) per-sample processing complexity.
 * 
 * Key features:
 * - Real-time processing with minimal latency
 * - Efficient window sum updates using binary tree
 * - Support for both scalar and batch processing
 * - Initial state configuration
 * 
 * The filter computes the average over a sliding window of length N:
 * @f[ y[n] = \frac{1}{N} \sum_{k=0}^{N-1} x[n - k] @f]
 * 
 * @see MAFilterR, MAFilterC
 */
template<typename T>
class MAFilter
{
public:
    /**
     * @brief Construct a MAFilter with specified window length
     * @param len Window length (number of samples in moving average)
     * 
     * @pre len >= 1
     * @exception DSPLIB_ASSERT Throws if len < 1
     * 
     * Initializes all filter states to zero.
     */
    explicit MAFilter(int len)
      : d_{nextpow2(len)}
      , w_{len}
      , buf_((1 << (d_ + 1)) - 1, T{0}) {
        DSPLIB_ASSERT(len >= 1, "window len must be positive");
    }

    /**
     * @brief Construct a MAFilter with initial state
     * @param len Window length
     * @param init Initial window values
     * 
     * @pre init.size() == len
     * @exception DSPLIB_ASSERT Throws if init size doesn't match window length
     * 
     * Initializes the filter with predefined window values. Equivalent to:
     * - Constructing with length
     * - Processing init array through filter
     */
    explicit MAFilter(int len, const base_array<T>& init)
      : MAFilter{len} {
        DSPLIB_ASSERT(init.size() == w_, "init size mismatch");
        for (int i = 0; i < w_; i++) {
            this->process(init[i]);
        }
    }

    /**
     * @brief Process single input sample
     * @param in Input sample
     * @return Filtered output (average of current window)
     * 
     * Computational complexity: O(log N)
     */
    T process(const T& in) noexcept {
        if (w_ == 1) {
            return in;
        }

        int k = (1 << d_) - 1 + pos_;
        buf_[k] = in;
        while (k > 0) {
            k = (k - 1) >> 1;
            buf_[k] = buf_[2 * k + 1] + buf_[2 * k + 2];
        }
        pos_ = (pos_ + 1) % w_;
        return buf_[0] / w_;
    }

    /**
     * @brief Process array of input samples
     * @param in Input array
     * @return Output array of filtered samples
     * 
     * Equivalent to sequential processing of each sample.
     * For better performance on large arrays, consider block processing optimization.
     */
    base_array<T> process(const base_array<T>& in) noexcept {
        if (w_ == 1) {
            return in;
        }

        const int n = in.size();
        base_array<T> out(n);
        for (int i = 0; i < n; i++) {
            out[i] = process(in[i]);
        }
        return out;
    }

    /**
     * @brief Process single input sample (operator form)
     * @param x Input sample
     * @return Filtered output
     * 
     * @see process()
     */
    T operator()(const T& x) noexcept {
        return this->process(x);
    }

    /**
     * @brief Process array of input samples (operator form)
     * @param x Input array
     * @return Output array
     * 
     * @see process()
     */
    base_array<T> operator()(const base_array<T>& x) noexcept {
        return this->process(x);
    }

private:
    int d_;                ///< Tree depth: d = ceil(log2(len))
    int w_;                ///< Window length
    std::vector<T> buf_;   ///< Binary tree buffer (size = 2^(d+1)-1)
    int pos_{0};           ///< Current position in circular buffer
};

/**
 * @brief Moving Average Filter for real-valued signals
 * @see MAFilter
 */
using MAFilterR = MAFilter<real_t>;

/**
 * @brief Moving Average Filter for complex-valued signals
 * @see MAFilter
 */
using MAFilterC = MAFilter<cmplx_t>;

}   // namespace dsplib