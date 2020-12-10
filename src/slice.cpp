#include <dsplib/array.h>

namespace dsplib {

//-------------------------------------------------------------------------------------------------
template<class Slice>
void _assign_slice(Slice& lhs, const Slice& rhs)
{
    const int n1 = (lhs.p2 - lhs.p1) / lhs.step;
    const int n2 = (rhs.p2 - rhs.p1) / rhs.step;
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }

    int i1 = (lhs.p1 + lhs.size) % lhs.size;
    int i2 = (rhs.p1 + rhs.size) % rhs.size;
    for (size_t i = 0; i < n2; i++) {
        lhs.data[i1] = rhs.data[i2];
        i1 = (i1 + lhs.step + lhs.size) % lhs.size;
        i2 = (i2 + rhs.step + rhs.size) % rhs.size;
    }
}

//-------------------------------------------------------------------------------------------------
template<class Slice, class Arr>
void _assign_arr(Slice& lhs, const Arr& rhs)
{
    const int n1 = (lhs.p2 - lhs.p1) / lhs.step;
    const int n2 = rhs.size();
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }

    int i1 = (lhs.p1 + lhs.size) % lhs.size;
    for (size_t i = 0; i < n2; i++) {
        lhs.data[i1] = rhs[i];
        i1 = (i1 + lhs.step + lhs.size) % lhs.size;
    }
}

//-------------------------------------------------------------------------------------------------
template<class Slice, class Type>
void _assign_val(Slice& lhs, const Type& value)
{
    const int n = (lhs.p2 - lhs.p1) / lhs.step;
    int i1 = (lhs.p1 + lhs.size) % lhs.size;
    for (size_t i = 0; i < n; i++) {
        lhs.data[i1] = value;
        i1 = (i1 + lhs.step + lhs.size) % lhs.size;
    }
}

//-------------------------------------------------------------------------------------------------
template<class Slice, class Type>
void _assign_list(Slice& lhs, const std::initializer_list<Type>& list)
{
    const int n1 = (lhs.p2 - lhs.p1) / lhs.step;
    const int n2 = list.size();
    if (n1 != n2) {
        throw std::range_error("Not equal size");
    }

    int i1 = (lhs.p1 + lhs.size) % lhs.size;
    const auto* d2 = list.begin();
    for (size_t i = 0; i < n2; i++) {
        lhs.data[i1] = d2[i];
        i1 = (i1 + lhs.step + lhs.size) % lhs.size;
    }
}

//-------------------------------------------------------------------------------------------------
slice_real& slice_real::operator=(const slice_real& rhs)
{
    _assign_slice<slice_real>(*this, rhs);
    return *this;
}

//-------------------------------------------------------------------------------------------------
slice_real& slice_real::operator=(const arr_real& rhs)
{
    _assign_arr<slice_real, arr_real>(*this, rhs);
    return *this;
}

//-------------------------------------------------------------------------------------------------
slice_real& slice_real::operator=(const real_t& value)
{
    _assign_val<slice_real, real_t>(*this, value);
    return *this;
}

//-------------------------------------------------------------------------------------------------
slice_real& slice_real::operator=(const std::initializer_list<real_t>& list)
{
    _assign_list<slice_real, real_t>(*this, list);
    return *this;
}

//-------------------------------------------------------------------------------------------------
slice_cmplx& slice_cmplx::operator=(const slice_cmplx& rhs)
{
    _assign_slice<slice_cmplx>(*this, rhs);
    return *this;
}

//-------------------------------------------------------------------------------------------------
slice_cmplx& slice_cmplx::operator=(const arr_cmplx& rhs)
{
    _assign_arr<slice_cmplx, arr_cmplx>(*this, rhs);
    return *this;
}

//-------------------------------------------------------------------------------------------------
slice_cmplx& slice_cmplx::operator=(const cmplx_t& value)
{
    _assign_val<slice_cmplx, cmplx_t>(*this, value);
    return *this;
}

//-------------------------------------------------------------------------------------------------
slice_cmplx& slice_cmplx::operator=(const std::initializer_list<cmplx_t>& list)
{
    _assign_list<slice_cmplx, cmplx_t>(*this, list);
    return *this;
}

}   // namespace dsplib
