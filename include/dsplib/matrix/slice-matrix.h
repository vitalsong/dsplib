#pragma once

#include <dsplib/types.h>
#include <vector>
#include <array>

#include "base-matrix.h"
#include "index-matrix.h"

namespace dsplib {

template<typename T>
class matrix_slice_const_t;

template<typename T>
class matrix_slice_t;

//-------------------------------------------------------------------------
template<typename T>
class matrix_slice_const_t
{
public:
    matrix_slice_const_t(const base_matrix_t<T>& base, index_matrix_t rows, index_matrix_t cols)
      : _base{base}
      , _row_idx{rows}
      , _col_idx{cols} {
        //TODO: check matrix range
    }

    matrix_slice_const_t(const matrix_slice_const_t& rhs) = default;

    matrix_slice_const_t(const matrix_slice_t<T>& rhs)
      : _base{rhs._base}
      , _row_idx{rhs._row_idx}
      , _col_idx{rhs._col_idx} {
    }

    std::array<int, 2> size() const {
        return {_row_idx.size(), _col_idx.size()};
    }

    const base_matrix_t<T>& _base;
    index_matrix_t _row_idx;
    index_matrix_t _col_idx;
};

//-------------------------------------------------------------------------
template<typename T>
class matrix_slice_t
{
public:
    matrix_slice_t(base_matrix_t<T>& base, index_matrix_t rows, index_matrix_t cols)
      : _base{base}
      , _row_idx{rows}
      , _col_idx{cols} {
        //TODO: check matrix range
    }

    //matrix(idxs) = scalar
    template<typename _Scalar, typename enable_scalar_t<_Scalar>::type>
    matrix_slice_t& operator=(const _Scalar& rhs) {
        static_assert(std::is_convertible<_Scalar, T>::value, "Type is not convertible");
        for (auto i : _row_idx) {
            for (auto k : _col_idx) {
                _base(i, k) = rhs;
            }
        }
    }

    //matrix(idxs) = other_matrix(idxs)
    template<typename T2>
    matrix_slice_t& operator=(const matrix_slice_const_t<T2>& rhs) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        if (this->size() != rhs.size()) {
            throw std::runtime_error("Not equal slices");
        }

        const int n = this->size()[0];
        const int m = this->size()[1];
        for (size_t i = 0; i < n; i++) {
            for (size_t k = 0; k < m; k++) {
                const int i1 = _row_idx[i];
                const int k1 = _col_idx[k];
                const int i2 = rhs._row_idx[i];
                const int k2 = rhs._col_idx[k];
                _base(i1, k1) = rhs._base(i2, k2);
            }
        }

        return *this;
    }

    //matrix(idxs) = other_matrix
    template<typename T2>
    matrix_slice_t& operator=(const base_matrix_t<T2>& rhs) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        if (this->size() != rhs.size()) {
            throw std::runtime_error("Not equal slices");
        }

        const int n = this->size()[0];
        const int m = this->size()[1];
        for (size_t i = 0; i < n; i++) {
            for (size_t k = 0; k < m; k++) {
                const int i1 = _row_idx[i];
                const int k1 = _col_idx[k];
                _base(i1, k1) = rhs(i, k);
            }
        }

        return *this;
    }

    std::array<int, 2> size() const {
        return {_row_idx.size(), _col_idx.size()};
    }

    base_matrix_t<T>& _base;
    index_matrix_t _row_idx;
    index_matrix_t _col_idx;
};

}   // namespace dsplib