#pragma once

namespace dsplib {
namespace indexing {

struct end_t
{
    explicit constexpr end_t(int) {
    }
};

constexpr end_t end{0};

}   // namespace indexing

}   // namespace dsplib