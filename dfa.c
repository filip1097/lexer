/*> Description ***********************************************************************************/
/**
* @brief Deals DFAs (deterministic finite automaton).
* @file dfa.c
*/

/*> Includes **************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

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
 * @brief Creates DFA states recursively. DFA states are based on a power set of NFA states formed
 *        from by the input NFA state.
 * @param[in/out]  dfa_p          The DFA.
 * @param[in/out]  powerSets_p    The power sets of NFA states that forms DFA states.
 * @param[in/out]  nfaToDfaMap_p  Maps nfa indicies to dfa indicies.
 * @param[in]      nfa_p          The NFA to that contains the NFA states.
 * @param[in]      nfaStateIdx    The first NFA state in the power set that will form a new DFA 
 *                                state.
 */
void create_dfa_states(DfaS* const dfa_p,
                       BitSetT* const powerSets_p,
                       int* const nfaToDfaMap_p,
                       const NfaS* const nfa_p,
                       const int nfaStateIdx);

/*> Local Function Definitions ********************************************************************/
void create_dfa_states(DfaS* const dfa_p,
                       BitSetT* const powerSets_p,
                       int* const nfaToDfaMap_p,
                       const NfaS* const nfa_p,
                       const int nfaStateIdx)
{
  int newStateIdx = dfa_p->numStates;
  dfa_p->numStates++;
  assert(dfa_p->numStates < MAX_NUM_DFA_STATES);
  
  DfaStateS* newDfaState_p = &(dfa_p->states[newStateIdx]);
  newDfaState_p->isEndState = false;
  newDfaState_p->outputValue = 0;
  memset(&(newDfaState_p->transitions), NO_STATE, sizeof(newDfaState_p->transitions));

  powerSets_p[newStateIdx] = epsilon_closure(nfa_p, newStateIdx);
  nfaToDfaMap_p[nfaStateIdx] = newStateIdx;
  
  for (int i = 0; i < MAX_NUM_NFA_STATES; i++)
  {
    if (is_in_bitset(&(powerSets_p[newStateIdx]), i))
    {
      const NfaStateS* nfaStateInPowerSet_p = &(nfa_p->states[i]);
      if (nfaStateInPowerSet_p->isEndState)
      {
        // Assert that new state will not have several unique return values.
        bool differentReturnValues =
          newDfaState_p->isEndState && 
          (newDfaState_p->outputValue != nfaStateInPowerSet_p->outputValue);
        assert(!differentReturnValues);
        newDfaState_p->isEndState = true;
        newDfaState_p->outputValue = nfaStateInPowerSet_p->outputValue;
      }

      for (int j = 0; j < NUM_CHARS; j++)
      {
        int currDfaTransition = newDfaState_p->transitions[j];
        int nfaTransition = nfaStateInPowerSet_p->transitions[j];
        int newDfaTransition;

        if (nfaTransition != NO_STATE)
        {
          if (nfaToDfaMap_p[nfaTransition] == NO_STATE)
          {
            create_dfa_states(dfa_p, powerSets_p, nfaToDfaMap_p, nfa_p, nfaTransition);
          }
          newDfaTransition = nfaToDfaMap_p[nfaTransition];
        }
        else
        {
          newDfaTransition = NO_STATE;
        }
        
        // Assert that new state will not transition to several unique states. 
        // This could be handeled in the future with supersets.
        bool transitionsToDifferentStates = (currDfaTransition != NO_STATE) &&
                                            (newDfaTransition != NO_STATE) &&
                                            (currDfaTransition != newDfaTransition);
        assert(!transitionsToDifferentStates);

        if (newDfaTransition != NO_STATE)
        {
          newDfaState_p->transitions[j] = newDfaTransition;
        }
      }
    }
  }
}

/*> Global Function Definitions *******************************************************************/
DfaS* convert_to_dfa(const NfaS* const nfa_p)
{
  DfaS* dfa_p = malloc(sizeof(*dfa_p));
  dfa_p->numStates = 0;
  BitSetT powerSets[MAX_NUM_DFA_STATES] = {0};
  int nfaToDfaMap[MAX_NUM_NFA_STATES] = {NO_STATE};

  create_dfa_states(dfa_p, powerSets, nfaToDfaMap, nfa_p, 0);

  return dfa_p;
}
