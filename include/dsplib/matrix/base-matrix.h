#pragma once

#include <dsplib/types.h>

#include <vector>
#include <array>

namespace dsplib {

//Detect out type for operations:
//real + real -> real
//cmplx + cmplx -> cmplx
//real + cmplx -> cmplx
//cmplx + real -> cmplx

template<bool _Cond, typename _Iftrue, typename _Iffalse>
using conditional_t = typename std::conditional<_Cond, _Iftrue, _Iffalse>::type;

template<typename Bt, typename It>
using ResultType = conditional_t<std::is_same<Bt, cmplx_t>::value || std::is_same<It, cmplx_t>::value, cmplx_t, real_t>;

//global limit for matrix size
//partially protects against errors in matrix multiplication
constexpr int MATRIX_SIZE_LIMIT = 512 * 512;

//TODO: size template param like eigenlib
//typename enable_scalar_t<T>::type
template<typename T>
class base_matrix_t
{
public:
    explicit base_matrix_t(int n, int m)
      : _raw{std::vector<T>(n * m)}
      , _nrow{n}
      , _ncol{m} {
        if (n <= 0 or m <= 0) {
            throw std::runtime_error("wrong matrix size");
        }

        if (n * m > MATRIX_SIZE_LIMIT) {
            throw std::runtime_error("matrix size limit");
        }
    }

    template<class T2, class _C = typename enable_convertible_t<T2, T>::type>
    base_matrix_t(const base_matrix_t<T2>& v) {
        static_assert(std::is_convertible<T2, T>::value, "Type is not convertible");
        _raw.assign(v._raw.begin(), v._raw.end());
        _nrow = v._nrow;
        _ncol = v._ncol;
    }

    template<class T2, class _C = typename enable_convertible_t<T2, T>::type>
    base_matrix_t<T>& operator=(const base_matrix_t<T2>& rhs) {
        *this = base_matrix_t<T>(rhs);
        return *this;
    }

    template<class T2, class _C = typename enable_convertible_t<T2, T>::type>
    base_matrix_t(const std::initializer_list<std::initializer_list<T2>>& v) {
        _nrow = v.size();
        _ncol = v.begin()->size();
        _raw.resize(_nrow * _ncol);
        for (size_t i = 0; i < _nrow; i++) {
            auto row = *(v.begin() + i);
            for (size_t j = 0; j < _ncol; j++) {
                _raw[j * _nrow + i] = *(row.begin() + j);
            }
        }
    }

    T* data() {
        return _raw.data();
    }

    const T* data() const {
        return _raw.data();
    }

    T& operator()(int i, int k) {
        return _raw[k * _nrow + i];
    }

    T operator()(int i, int k) const {
        return _raw[k * _nrow + i];
    }

    //TODO: use MatrixSize class
    std::array<int, 2> size() const {
        return {_nrow, _ncol};
    }

    //--------------------------------------------------------------------
    base_matrix_t& operator+() const {
        return *this;
    }

    base_matrix_t& operator-() {
        for (size_t i = 0; i < _raw.size(); ++i) {
            _raw[i] = (-_raw[i]);
        }
        return *this;
    }

    //--------------------------------------------------------------------
    //matrix += scalar
    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R>& operator+=(const _Scalar& rhs) {
        static_assert(is_scalar_ar<_Scalar>::value, "Type is not scalar");
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _raw.size(); ++i) {
            _raw[i] += rhs;
        }
        return *this;
    }

    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R>& operator-=(const _Scalar& rhs) {
        static_assert(is_scalar_ar<_Scalar>::value, "Type is not scalar");
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _raw.size(); ++i) {
            _raw[i] -= rhs;
        }
        return *this;
    }

    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R>& operator*=(const _Scalar& rhs) {
        static_assert(is_scalar_ar<_Scalar>::value, "Type is not scalar");
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _raw.size(); ++i) {
            _raw[i] *= rhs;
        }
        return *this;
    }

    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R>& operator/=(const _Scalar& rhs) {
        static_assert(is_scalar_ar<_Scalar>::value, "Type is not scalar");
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        for (size_t i = 0; i < _raw.size(); ++i) {
            _raw[i] /= rhs;
        }
        return *this;
    }

    //--------------------------------------------------------------------
    //matrix + scalar
    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R> operator+(const _Scalar& rhs) const {
        base_matrix_t<R> temp(*this);
        temp += rhs;
        return temp;
    }

    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R> operator-(const _Scalar& rhs) const {
        base_matrix_t<R> temp(*this);
        temp -= rhs;
        return temp;
    }

    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R> operator*(const _Scalar& rhs) const {
        base_matrix_t<R> temp(*this);
        temp *= rhs;
        return temp;
    }

    template<class _Scalar, class R = ResultType<T, _Scalar>, class _S = typename enable_scalar_t<_Scalar>::type>
    base_matrix_t<R> operator/(const _Scalar& rhs) const {
        base_matrix_t<R> temp(*this);
        temp /= rhs;
        return temp;
    }

    //--------------------------------------------------------------------
    //matrix * matrix
    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R>& operator+=(const base_matrix_t<T2>& rhs) {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        if (_raw.size() != rhs._raw.size()) {
            throw std::invalid_argument("array sizes are different");
        }
        for (size_t i = 0; i < _raw.size(); ++i) {
            _raw[i] += rhs._raw[i];
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R>& operator-=(const base_matrix_t<T2>& rhs) {
        static_assert(std::is_same<T, R>::value, "The operation changes the type");
        if (_raw.size() != rhs._raw.size()) {
            throw std::invalid_argument("array sizes are different");
        }
        for (size_t i = 0; i < _raw.size(); ++i) {
            _raw[i] -= rhs._raw[i];
        }
        return *this;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R> operator+(const base_matrix_t<T2>& rhs) const {
        base_matrix_t<R> res(*this);
        res += rhs;
        return res;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R> operator-(const base_matrix_t<T2>& rhs) const {
        base_matrix_t<R> res(*this);
        res -= rhs;
        return res;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R>& operator*=(const base_matrix_t<T2>& rhs) = delete;

    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R> operator*(const base_matrix_t<T2>& rhs) const {
        if (_ncol != rhs._nrow) {
            throw std::invalid_argument("array sizes are different");
        }

        const int n = _nrow;
        const int m = _ncol;
        const int p = rhs._ncol;
        base_matrix_t<R> res(n, p);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                res(i, j) = 0;
                for (int k = 0; k < m; k++) {
                    res(i, j) += (*this)(i, k) * rhs(k, j);
                }
            }
        }
        return res;
    }

    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R>& operator/=(const base_matrix_t<T2>& rhs) = delete;

    template<class T2, class R = ResultType<T, T2>>
    base_matrix_t<R> operator/(const base_matrix_t<T2>& rhs) = delete;

    std::vector<T> _raw;
    int _nrow{0};
    int _ncol{0};
};

}   // namespace dsplib