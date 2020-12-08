#ifndef PLOT_H
#define PLOT_H

#include <dsplib.h>
#include <plot/matplotlibcpp.h>

namespace dsplib {
namespace plot {

inline void plot(const arr_real& x, const std::string& format = "") {
    std::vector<real_t> tx(x.data(), x.data()+x.size());
    matplotlibcpp::plot(tx, format);
}

inline void plot(const arr_real& x, const arr_real& y, const std::string& format = "") {
    std::vector<real_t> tx(x.data(), x.data()+x.size());
    std::vector<real_t> ty(y.data(), y.data()+y.size());
    matplotlibcpp::plot(tx, ty, format);
}

inline void scatterplot(const arr_real& x, const arr_real& y) {
    std::vector<real_t> tx(x.data(), x.data()+x.size());
    std::vector<real_t> ty(y.data(), y.data()+y.size());
    matplotlibcpp::scatter(tx, ty);
}

inline void scatterplot(const arr_cmplx& x) {
    auto tx = real(x);
    auto ty = imag(x);
    scatterplot(tx, ty);
}

inline void title(const std::string& title) {
    matplotlibcpp::title(title);
}

inline void show() {
    matplotlibcpp::show();
}

}   ///< plot
}   ///< dsplib

#endif // PLOT_H
