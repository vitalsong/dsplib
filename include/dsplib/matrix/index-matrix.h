#pragma once

#include <vector>

namespace dsplib {

namespace placeholders {

struct all_t
{
    explicit constexpr all_t(int) {
    }
};

inline constexpr all_t all{0};

}   // namespace placeholders

using index_matrix_t = std::vector<int>;

//-------------------------------------------------------------------------
//TODO: generator implementation
// class index_matrix_t
// {
// public:
//     //TODO: cast to vector
//     //...

//     index_matrix_t(const index_matrix_t& idxs) = default;

//     index_matrix_t(int i) {
//         _idxs = {i};
//     }

//     index_matrix_t(const std::vector<int>& idxs)
//       : _idxs{idxs} {
//     }

//     index_matrix_t(const std::initializer_list<int>& idxs)
//       : _idxs{idxs} {
//     }

//     int size() const {
//         return _idxs.size();
//     }

//     typedef typename std::vector<int>::iterator iterator;
//     typedef typename std::vector<int>::const_iterator const_iterator;

//     iterator begin() {
//         return _idxs.begin();
//     }

//     iterator end() {
//         return _idxs.end();
//     }

//     const_iterator begin() const {
//         return _idxs.begin();
//     }

//     const_iterator end() const {
//         return _idxs.end();
//     }

// private:
//     std::vector<int> _idxs;
// };

//TODO: add version for end placeholder
inline index_matrix_t seq(int start, int stop, int step = 1) {
    const int n = (stop - start) / step;
    std::vector<int> r(n);
    int v = start;
    for (int i = 0; i < n; ++i) {
        r[i] = v;
        v += step;
    }
    return r;
}

}   // namespace dsplib