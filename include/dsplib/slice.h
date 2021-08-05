#pragma once

namespace dsplib {

template<typename T>
class base_array;

template<typename T>
class slice
{
public:
    slice() = default;

    slice(const slice& rhs)
    {
        *this = rhs;
    }

    slice& operator=(const slice& rhs)
    {
        const int n1 = (p2 - p1) / step;
        const int n2 = (rhs.p2 - rhs.p1) / rhs.step;
        if (n1 != n2) {
            throw std::out_of_range("Not equal size");
        }

        if (n1 == 0 && n2 == 0) {
            return *this;
        }

        int i1 = (p1 + size) % size;
        int i2 = (rhs.p1 + rhs.size) % rhs.size;
        for (size_t i = 0; i < n2; i++) {
            data[i1] = rhs.data[i2];
            i1 = (i1 + step + size) % size;
            i2 = (i2 + rhs.step + rhs.size) % rhs.size;
        }

        return *this;
    }

    slice& operator=(const base_array<T>& rhs)
    {
        const int n1 = (p2 - p1) / step;
        const int n2 = rhs.size();
        if (n1 != n2) {
            throw std::out_of_range("Not equal size");
        }

        if (n1 == 0 && n2 == 0) {
            return *this;
        }

        int i1 = (p1 + size) % size;
        for (size_t i = 0; i < n2; i++) {
            data[i1] = rhs[i];
            i1 = (i1 + step + size) % size;
        }
        
        return *this;
    }

    slice& operator=(const T& value)
    {
        const int n = (p2 - p1) / step;
        int i1 = (p1 + size) % size;
        for (size_t i = 0; i < n; i++) {
            data[i1] = value;
            i1 = (i1 + step + size) % size;
        }

        return *this;
    }

    slice& operator=(const std::initializer_list<T>& list)
    {
        const int n1 = (p2 - p1) / step;
        const int n2 = list.size();
        if (n1 != n2) {
            throw std::out_of_range("Not equal size");
        }

        if (n1 == 0 && n2 == 0) {
            return *this;
        }

        int i1 = (p1 + size) % size;
        const auto* d2 = list.begin();
        for (size_t i = 0; i < n2; i++) {
            data[i1] = d2[i];
            i1 = (i1 + step + size) % size;
        }

        return *this;
    }

    int p1 = 0;
    int p2 = 0;
    int step = 0;
    int size = 0;
    T* data = nullptr;
};

}   // namespace dsplib