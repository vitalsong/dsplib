#pragma once

#include <vector>
#include <cstring>

#include <dsplib/types.h>
#include <dsplib/array.h>

#include "base-matrix.h"
#include "slice-matrix.h"

namespace dsplib {

//-------------------------------------------------------------------------
template<typename _Tp>
class matrix_t : public base_matrix_t<_Tp>
{
public:
    explicit matrix_t(int n, int m = 1)
      : base_matrix_t<_Tp>{n, m} {
    }

    //-------------------------------------------------------------------

    //matrix<cmplx> = base_matrix<real> etc
    //because all operators result is base_matrix type
    template<class T2, class _C = typename enable_convertible_t<T2, _Tp>::type>
    matrix_t(const base_matrix_t<T2>& v)
      : base_matrix_t<_Tp>{v} {
    }

    template<class T2, class _C = typename enable_convertible_t<T2, _Tp>::type>
    matrix_t<_Tp>& operator=(const base_matrix_t<T2>& rhs) {
        *this = matrix_t<_Tp>(rhs);
        return *this;
    }

    //-------------------------------------------------------------------

    template<class T2, class _C = typename enable_convertible_t<T2, _Tp>::type>
    matrix_t(const std::initializer_list<std::initializer_list<T2>>& v)
      : base_matrix_t<_Tp>{v} {
    }

    //-------------------------------------------------------------------

    //instance from base_array (legacy support)
    //template<class T2, typename enable_convertible_t<T2, _Tp>::type>
    template<typename T2>
    matrix_t(const base_array<T2>& v)
      : base_matrix_t<_Tp>{v.size(), 1} {
        this->_raw.assign(v.data(), v.data() + v.size());
    }

    //template<class T2, typename enable_convertible_t<T2, _Tp>::type>
    template<typename T2>
    matrix_t<_Tp>& operator=(const base_array<T2>& rhs) {
        *this = matrix_t<_Tp>(rhs);
        return *this;
    }

    //-------------------------------------------------------------------

    //1x1 matrix to scalar
    template<typename T, class _S = typename enable_scalar_t<T>::type,
             class _C = typename enable_convertible_t<_Tp, T>::type>
    operator T() const {
        if ((this->_nrow != 1) || (this->_ncol != 1)) {
            throw std::runtime_error("To scalar cast error. Matrix is not 1x1.");
        }
        return this->_raw[0];
    }

    //-------------------------------------------------------------------

    //TODO: instance from slice
    matrix_t(const matrix_slice_const_t<_Tp>& rhs) {
        //...
    }

    matrix_t<_Tp>& operator=(const matrix_slice_const_t<_Tp>& rhs) {
        *this = matrix_t<_Tp>(rhs);
        return *this;
    }

    //-------------------------------------------------------------------

    //index
    _Tp& operator()(int i, int k) {
        return base_matrix_t<_Tp>::operator()(i, k);
    }

    _Tp operator()(int i, int k) const {
        return base_matrix_t<_Tp>::operator()(i, k);
    }

    //-------------------------------------------------------------------

    //TODO: index 1xN or Nx1
    //_Tp operator()(int i);

    //slicing
    matrix_slice_t<_Tp> operator()(index_matrix_t i, index_matrix_t k) {
        return matrix_slice_t<_Tp>(*this, i, k);
    }
    matrix_slice_const_t<_Tp> operator()(index_matrix_t i, index_matrix_t k) const {
        return matrix_slice_const_t<_Tp>(*this, i, k);
    }

    matrix_slice_t<_Tp> operator()(index_matrix_t i, placeholders::all_t k = placeholders::all) {
        return this->operator()(i, seq(0, this->_ncol));
    }
    matrix_slice_const_t<_Tp> operator()(index_matrix_t i, placeholders::all_t k = placeholders::all) const {
        return this->operator()(i, seq(0, this->_ncol));
    }

    matrix_slice_t<_Tp> operator()(placeholders::all_t i, index_matrix_t k) {
        return this->operator()(seq(0, this->_nrow), k);
    }
    matrix_slice_const_t<_Tp> operator()(placeholders::all_t i, index_matrix_t k) const {
        return this->operator()(seq(0, this->_nrow), k);
    }

    //row/column?
    // matrix_1d_t operator()(int i, index_matrix_t k) {
    //     //...
    // }

    // matrix_1d_t operator()(index_matrix_t i, int k) {
    //     //...
    // }

    //transpose
    //TODO: transpose only after copy
    //TODO: use public T object (transpose proxy?)
    matrix_t& T() {
        //...
    }

    matrix_t T() const {
        //...
    }

    matrix_t inner(const matrix_t& rhs) const {
        //...
    }
};

}   // namespace dsplib