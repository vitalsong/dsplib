TEMPLATE = lib
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug, debug|release) {
    TARGET = dsplibd
  } else {
    TARGET = dsplib
  }

DESTDIR = $${PWD}/lib/

HEADERS += \
    include/dsplib.h \
    include/dsplib/hilbert.h \
    include/dsplib/fir.h \
    include/dsplib/iir.h \
    include/dsplib/medfilt.h \
    include/dsplib/math.h \
    include/dsplib/xcorr.h \
    include/dsplib/window.h \
    include/dsplib/types.h \
    include/dsplib/array.h \
    include/dsplib/math.h \
    include/dsplib/service.h \
    include/dsplib/awgn.h \
    include/dsplib/fft.h \
    include/dsplib/ifft.h \
    src/dft-tables.h

SOURCES += \
    src/math.cpp \
    src/window.cpp \
    src/fir.cpp \
    src/hilbert.cpp \
    src/arr_cmplx.cpp \
    src/arr_real.cpp \
    src/awgn.cpp \
    src/dft-tables.cpp \
    src/fft.cpp \
    src/ifft.cpp \
    src/xcorr.cpp

INCLUDEPATH += $${PWD}/include
