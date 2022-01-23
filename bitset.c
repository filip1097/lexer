/*> Description ***********************************************************************************/
/**
 * @brief Functionality for bitsets.
 * @file bitset.c
 */

/*> Includes **************************************************************************************/
#include "bitset.h"

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/

/*> Local Function Definitions ********************************************************************/

/*> Global Function Definitions *******************************************************************/
void bitset_to_string(const BitSetT* const bitset_p, char str[BITSET_STRING_SIZE])
{
  uint64_t one = 1;
  BitSetT bits = *bitset_p;

  for (int i = BITSET_SIZE - 1; i >= 0 ; i--)
  {
    if (bits & one == one)
    {
      str[i] = '1';
    }
    else
    {
      str[i] = '0';
    }
    bits = bits >> 1;
  }

  str[BITSET_STRING_SIZE - 1] = '\0';
}
