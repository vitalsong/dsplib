#pragma once

#include <functional>
#include <dsplib/span.h>
#include <dsplib/math.h>

namespace dsplib {

template<class Fn>
class ScopeGuard
{
public:
    explicit ScopeGuard(Fn f)
      : f_(std::move(f)) {
    }
    ~ScopeGuard() {
        f_();
    }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    Fn f_;
};

/**
 * @class FIFOBuffer
 * @brief A thread-unsafe FIFO buffer for trivially copyable data types
 * @tparam T Element type (must be trivially copyable and not bool)
 * @warning Not thread-safe for concurrent access
 * @warning T must be trivially copyable (POD-like)
 */
template<typename T>
class FIFOBuffer
{
public:
    static_assert(!std::is_same_v<T, bool>, "boolean buffer is not supported");
    static_assert(std::is_trivially_copyable<T>(), "type must be trivially copyable");

    /**
     * @brief Default constructor
     * Creates an empty buffer with default capacity
     */
    FIFOBuffer() = default;

    /**
     * @brief Write data to buffer
     * @param data Contiguous span of elements to append
     * Appends elements to the end of buffer. No-op for empty spans.
     */
    void write(span_t<T> data) noexcept {
        if (data.empty()) {
            return;
        }
        buf_.insert(buf_.end(), data.begin(), data.end());
    }

    /**
     * @brief Write single element (copy)
     * @param value Element to append
     */
    void write(const T& value) noexcept {
        buf_.push_back(value);
    }

    /**
     * @brief Read data from buffer
     * @param out Output span for data
     * @return Number of elements actually read
     */
    int read(mut_span_t<T> out) noexcept {
        const int n = min(out.size(), int(buf_.size()));
        if (n == 0) {
            return 0;
        }

        std::memcpy(out.data(), buf_.data(), n * sizeof(T));
        if (n == buf_.size()) {
            buf_.clear();
        } else {
            std::memmove(buf_.data(), buf_.data() + n, (buf_.size() - n) * sizeof(T));
            buf_.resize(buf_.size() - n);
        }
        return n;
    }

    /**
     * @brief Read data to new vector
     * @param n Number of elements to read
     * @return Vector with read elements
     */
    std::vector<T> read(int n) noexcept {
        n = min(n, buf_.size());
        std::vector<T> out(n);
        this->read(out);
        return out;
    }

    /**
     * @brief Get current element count
     * @return Number of elements in buffer
     */
    [[nodiscard]] int size() const noexcept {
        return buf_.size();
    }

    /**
     * @brief Check if buffer empty
     * @return true if size() == 0
     */
    [[nodiscard]] bool empty() const noexcept {
        return buf_.empty();
    }

    /**
     * @brief Clear buffer contents
     * Removes all elements. Preserves allocated capacity.
     */
    void reset() noexcept {
        buf_.clear();
    }

    /**
     * @brief Get read-only view of buffer
     * @return Span covering all elements (in FIFO order)
     * @warning Invalidated by write operations
     */
    [[nodiscard]] span_t<T> view() const noexcept {
        return make_span(buf_);
    }

private:
    std::vector<T> buf_;
};

/**
 * @class ChunkBuffer
 * @brief Accumulates data samples and processes them in fixed-size chunks
 * @tparam T Data type of stored elements (typically real_t or cmplx_t)
 * 
 * This buffer collects incoming data samples and automatically processes
 * complete chunks of specified size through a callback. Partial chunks are
 * preserved between write operations. Useful for block-based DSP processing.
 */
template<typename T>
class ChunkBuffer
{
public:
    /// Callback type for processing full chunks
    using Callback = std::function<void(span_t<T>)>;

    static_assert(!std::is_same_v<T, bool>, "boolean buffer is not supported");
    static_assert(std::is_trivially_copyable<T>(), "type must be trivially copyable");

    /**
     * @brief Construct a chunk buffer with specified chunk size
     * @param chunk_size Fixed processing chunk size (must be > 1)
     * @warning Triggers assertion if chunk_size <= 1
     * @note Pre-allocates memory for 2*chunk_size elements
     */
    explicit ChunkBuffer(int chunk_size)
      : chunk_size_{chunk_size} {
        DSPLIB_ASSERT(chunk_size > 1, "chunk size must be greater than 1");
        buf_.reserve(chunk_size * 2);
    }

    /**
     * @brief Add data to buffer and process complete chunks
     * @param data Input data span
     * @param fn Processing callback for each full chunk
     * 
     * Behavior:
     * - No-op for empty input
     * - Asserts on recursive calls
     * - Preserves partial chunks between calls
     * - Calls callback for each complete chunk
     * - Maintains temporal order of samples
     * 
     * @warning Callback execution MUST NOT call write()/flush() on same buffer
     */
    void write(span_t<T> data, Callback fn) {
        if (data.empty()) {
            return;
        }

        DSPLIB_ASSERT(!in_write_, "recursive write call detected");
        in_write_ = true;
        const auto guard = ScopeGuard([this] {
            in_write_ = false;
        });

        buf_.insert(buf_.end(), data.begin(), data.end());
        const auto* pdata = buf_.data();
        int psize = buf_.size();
        while (psize >= chunk_size_) {
            fn(make_span(pdata, chunk_size_));
            pdata += chunk_size_;
            psize -= chunk_size_;
        }

        std::memmove(buf_.data(), buf_.data() + buf_.size() - psize, psize * sizeof(T));
        buf_.resize(psize);
    }

    /**
     * @brief Process remaining data
     * @param fn Callback receiving all remaining samples
     * 
     * Clears buffer after processing. Safe to call with empty buffer
     * (callback receives empty span). Use for end-of-stream processing.
     * 
     * @warning Callback execution MUST NOT call write()/flush() on same buffer
     */
    void flush(Callback fn) {
        DSPLIB_ASSERT(!in_write_, "flush during callback is unsafe");
        in_write_ = true;
        const auto guard = ScopeGuard([this] {
            in_write_ = false;
        });

        fn(span_t<T>(buf_.data(), buf_.size()));
        buf_.clear();
    }

    /**
     * @brief Clear buffer contents
     * Resets buffer to empty state. Preserves allocated capacity.
     * @throws Assertion if called during write/flush operation
     */
    void reset() {
        DSPLIB_ASSERT(!in_write_, "reset during callback is unsafe");
        buf_.clear();
    }

    /**
     * @brief Get current number of accumulated samples
     * @return Count of unprocessed samples in buffer
     * 
     * Always 0 ≤ size() < chunk_size after write() completes
     */
    int size() const noexcept {
        return buf_.size();
    }

private:
    int chunk_size_;
    std::vector<T> buf_;
    bool in_write_{false};
};

}   // namespace dsplib