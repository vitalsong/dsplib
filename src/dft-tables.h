#ifndef DFT_TABLES_H
#define DFT_TABLES_H

#include <dsplib/types.h>
#include <stdint.h>

namespace dsplib {
namespace tables {

/*!
 * \brief Get (or generate) a table for calculating DFT
 * \param size DFT base
 * \return Table pointer
 */
const cmplx_t* dft_table(size_t size);

/*!
 * \brief Generate and cahed DFT table
 * \param size DFT base
 */
void dft_cash(size_t size);

/*!
 * \brief Clear table from cache
 * \warning Calling this feature may be hazardous to your health!
 * \param size DFT base
 */
void dft_clear(size_t size);

/*!
 * \brief Check for a table in the cache
 * \param size DFT base
 * \return Table state
 */
bool dft_cashed(size_t size);

}   ///< tables
}   ///< dsplib

#endif // DFT_TABLES_H
