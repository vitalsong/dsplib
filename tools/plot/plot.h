#ifndef PLOT_H
#define PLOT_H

#include <dsplib.h>
#include <matplotlibcpp.h>

namespace dsplib {
namespace plot {

void plot(const arr_real& x, const std::string& format = "") {
    std::vector<real_t> tx(x.data(), x.data()+x.size());
    matplotlibcpp::plot(tx, format);
}

void plot(const arr_real& x, const arr_real& y, const std::string& format = "") {
    std::vector<real_t> tx(x.data(), x.data()+x.size());
    std::vector<real_t> ty(y.data(), y.data()+y.size());
    matplotlibcpp::plot(tx, ty, format);
}

void scatterplot(const arr_real& x, const arr_real& y) {
    std::vector<real_t> tx(x.data(), x.data()+x.size());
    std::vector<real_t> ty(y.data(), y.data()+y.size());
    matplotlibcpp::scatter(tx, ty);
}

void scatterplot(const arr_cmplx& x) {
    auto tx = real(x);
    auto ty = imag(x);
    scatterplot(tx, ty);
}

void title(const std::string& title) {
    matplotlibcpp::title(title);
}

void show() {
    matplotlibcpp::show();
}

}   ///< plot
}   ///< dsplib

#endif // PLOT_H
