#pragma once

#include <dsplib/types.h>
#include <vector>
#include <memory>
#include <stdint.h>

namespace dsplib {
namespace tables {

using dft_ptr = std::shared_ptr<std::vector<cmplx_t>>;

/*!
 * \brief Get (or generate) a table for calculating DFT
 * \param n DFT base
 * \return Table pointer
 */
const dft_ptr dft_table(size_t n);

/*!
 * \brief Clear table from cache
 * \param n DFT base
 */
void dft_clear(size_t n);

/*!
 * \brief Check if table cached
 * \param n DFT base
 * \return Cached
 */
bool dft_cached(size_t n);

//bit-reverse table
using bitrev_ptr = std::shared_ptr<std::vector<int32_t>>;
const bitrev_ptr bitrev_table(size_t n);
bool bitrev_cached(size_t n);
void bitrev_clear(size_t n);

}   // namespace tables
}   // namespace dsplib
