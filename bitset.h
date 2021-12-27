/*> Description ***********************************************************************************/
/**
 * @brief Functionality for bitsets.
 * @file bitset.h
 */

/*> Multiple Inclusion Protection *****************************************************************/
#ifndef BITSET_H
#define BITSET_H

/*> Includes **************************************************************************************/
#include <stdbool.h>
#include <stdint.h>

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/
/**
 * @brief Bit set of size 64 bits.
 */
typedef uint64_t BitSetT;

/*> Constant Declarations *************************************************************************/

/*> Variable Declarations *************************************************************************/

/*> Function Declarations *************************************************************************/
/**
 * @brief Adds number to bit set.
 * @param[in/out]  bitset_p  The bitset.
 * @param[in]      num       The number to add.
 */
static inline void add_to_bitset(BitSetT* const bitset_p, const int num)
{
  *bitset_p |= (1 << num);
}

/**
 * @brief Checks if the provided number is in the bit set.
 * @param[in]  bitset_p  The bitset.
 * @param[in]  num       The number.
 * @return true if num is in the bit set, false otherwise.
 */
static inline bool is_in_bitset(const BitSetT* const bitset_p, const int num)
{
  return (*bitset_p & (1 << num)) > 0;
}

/*> End of Multiple Inclusion Protection **********************************************************/
#endif
