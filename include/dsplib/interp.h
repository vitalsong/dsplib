#pragma once

#include <dsplib/array.h>
#include <dsplib/fir.h>

namespace dsplib {

/*!
 * \brief The interp_filter class
 * \todo Implement based on polyphase filter
 */
class interp_filter
{
public:
    /*!
     * \brief Filter constructor
     * \param h Impulse response for interpolation
     * \param m Interpolation factor
     */
    explicit interp_filter(const arr_real& h, int m);

    /*!
     * \brief Filter processing
     * \param x Input frame [n]
     * \return Interpolated frame [n*m]
     */
    arr_real process(const arr_real& x);

private:
    int _m;
    fir _fir;
};

}   ///< dsplib
