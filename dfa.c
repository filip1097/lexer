/*> Description ***********************************************************************************/
/**
* @brief Deals DFAs (deterministic finite automaton).
* @file dfa.c
*/

/*> Includes **************************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "dfa.h"

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/

/*> Local Function Definitions ********************************************************************/

/*> Global Function Definitions *******************************************************************/
DfaS* convert_to_dfa(NfaS* nfa_p)
{
  // 1. Let Q' be a new set of states of the DFA. Q' is null in the starting
  //    Let T' be a new transition of the DFA.
  DfaS* dfa_p = malloc(sizeof(*dfa_p));
  memset(dfa_p, 0, sizeof(*dfa_p));

  bool has_changed = false;

  do
  {

    // 2. Add start of the NFA to Q'.
    //    Add transitions of the start state to the transition table T'.
    //    If start state makes transition to multiple states for some input alphabet, then treat
    //        those multiple states as a signle state in the DFA.

    // 3. If any new state is present in the transition table T'
    //        Add a new state in Q'
    //        Add transitions of that state in the transition table T'

    // 4. Keep repeating step 3 until so new state is present in the transition table T'
  } while(has_changed);

  return dfa_p;
}
