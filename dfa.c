/*> Description ***********************************************************************************/
/**
* @brief Deals DFAs (deterministic finite automaton).
* @file dfa.c
*/

/*> Includes **************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bitset.h"
#include "dfa.h"
#include "nfa.h"

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/
/**
 * @brief Creates DFA state based on a power set of NFA states.
 * @param[in/out]  dfa_p     The DFA to get a new state.
 * @param[in]      nfa_p     The NFA to that contains the NFA states.
 * @param[in]      powerSet  The power set of NFA states that will form a DFA state.
 */
void create_dfa_state(DfaS* const dfa_p, const NfaS* const nfa_p, const BitSetT powerSet);

/*> Local Function Definitions ********************************************************************/
void create_dfa_state(DfaS* const dfa_p, const NfaS* const nfa_p, const BitSetT powerSet)
{
  int newStateIdx = dfa_p->numStates;
  dfa_p->numStates++;
  assert(dfa_p->numStates < MAX_NUM_DFA_STATES);
  
  DfaStateS* newState_p = &(dfa_p->states[newStateIdx]);
  newState_p->isEndState = false;
  newState_p->outputValue = 0;
  
  bool firstState = true;
  for (int i = 0; i < MAX_NUM_NFA_STATES; i++)
  {
    if (is_in_bitset(&powerSet, i))
    {
      const NfaStateS* nfaState_p = &(nfa_p->states[i]);
      if (nfaState_p->isEndState)
      {
        // assert that new state will not have several return values.
        assert(newState_p->isEndState && newState_p->outputValue != nfaState_p->outputValue);
        newState_p->isEndState = true;
        newState_p->outputValue = nfaState_p->outputValue;
      }

      if (firstState)
      {
        memcpy(newState_p->transitions, nfaState_p->transitions, sizeof(nfaState_p->transitions));
        firstState = false;
      }
      else 
      {
        for (int j = 0; j < NUM_CHARS; j++)
        {
          assert(newState_p->transitions[j] != NO_STATE && 
                   newState_p->transitions[j] != nfaState_p->transitions[j]);
        }
      }

    }
  }

}

/*> Global Function Definitions *******************************************************************/
DfaS* convert_to_dfa(NfaS* nfa_p)
{
  DfaS* dfa_p = malloc(sizeof(*dfa_p));
  dfa_p->numStates = 0;

  BitSetT powerSets[MAX_NUM_DFA_STATES];
  memset(powerSets, 0, sizeof(powerSets));

    // 2. Add start of the NFA to Q'.
    //    Add transitions of the start state to the transition table T'.
    //    If start state makes transition to multiple states for some input alphabet, then treat
    //        those multiple states as a signle state in the DFA.

  powerSets[0] = epsilon_closure(nfa_p, 0);
  create_dfa_state(dfa_p, nfa_p, powerSets[0]);

  bool has_changed = false;
  do
  {


    // 3. If any new state is present in the transition table T'
    //        Add a new state in Q'
    //        Add transitions of that state in the transition table T'

    // 4. Keep repeating step 3 until so new state is present in the transition table T'
  } while(has_changed);

  return dfa_p;
}
