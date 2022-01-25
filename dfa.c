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
static void create_dfa_states(DfaS* const dfa_p,
                              BitSetT* const powerSets_p,
                              int* const nfaToDfaMap_p,
                              const NfaS* const nfa_p,
                              const int nfaStateIdx);

/**
 * @brief Checks if two DFA states are equal.
 * @param[in]  dfaState1_p  The first DFA state.
 * @param[in]  dfaState2_p  The second DFA state.
 * @return true if the states are equal, false otherwise.
 */
static bool dfa_state_is_equal(const DfaStateS* const dfaState1_p,
                               const DfaStateS* const dfaState2_p);

/**
 * @brief Optimizes the DFA by removing unecessary states.
 * @param[in/out]  dfa_p  The DFA.
 */
static void optimize_dfa(DfaS* const dfa_p);

/**
 * @brief Prints a DFA state with all its transitions.
 * @param[in]  dfa_p     The DFA.
 * @param[in]  stateIdx  The index of the state to be printed.
 */
static void print_dfa_state(const DfaS* const dfa_p, const int stateIdx);

/**
 * @brief Joins two equal DFA states together.
 * @param[in/out]  dfa_p      The DFA that contains the states.
 * @param[in]      stateIdx1  The index of the first DFA state.
 * @param[in]      stateIdx2  The index of the second DFA state.
 */
static void join_equal_dfa_states(DfaS* const dfa_p,
                              const int stateIdx1,
                              const int stateIdx2);

/*> Local Function Definitions ********************************************************************/
static void create_dfa_states(DfaS* const dfa_p,
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

  powerSets_p[newStateIdx] = epsilon_closure(nfa_p, nfaStateIdx);
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

static bool dfa_state_is_equal(const DfaStateS* const dfaState1_p, 
                               const DfaStateS* const dfaState2_p)
{
  if (dfaState1_p->isEndState != dfaState2_p->isEndState ||
      dfaState1_p->outputValue != dfaState2_p->outputValue)
  {
    return false;
  }

  for (int i = 0; i < NUM_CHARS; i++)
  {
    if (dfaState1_p->transitions[i] != dfaState2_p->transitions[i])
    {
      return false;
    }
  }

  return true;
}

static void optimize_dfa(DfaS* const dfa_p)
{
  bool hasChanged;

  do
  {
    hasChanged = false;

    for (int i = 0; i < dfa_p->numStates; i++)
    {
      for (int j = i + 1; j < dfa_p->numStates; j++)
      {
        if (dfa_state_is_equal(&dfa_p->states[i], &dfa_p->states[j]))
        {
          join_equal_dfa_states(dfa_p, i, j);

          hasChanged = true;
          // check if the replacement state is also the same
          j--;
        }
      }
    }
  } while (hasChanged);
}

static void print_dfa_state(const DfaS* const dfa_p, const int stateIdx)
{
  const DfaStateS* dfaState_p = &(dfa_p->states[stateIdx]);

  if (dfaState_p->isEndState)
  {
    printf("-State Q%d | End state : %d\n", stateIdx, dfaState_p->outputValue);
  }
  else
  {
    printf("-State Q%d\n", stateIdx);
  }

  int prevTransition = NO_STATE;
  char prevTransitionFirstChar = 0;
  for (int character = 0; character < NUM_CHARS; character++)
  {
    int currTransition = dfaState_p->transitions[character];

    if (prevTransition != currTransition)
    {
      if (prevTransition != NO_STATE)
      {
        if (prevTransitionFirstChar >= character - 1)
        {
          printf(" *Transition '%c' -> Q%d\n", prevTransitionFirstChar, prevTransition);
        }
        else
        {
          printf(" *Transition '%c'-'%c' -> Q%d\n",
                 prevTransitionFirstChar,
                 character-1,
                 prevTransition);
        }
      }
      prevTransitionFirstChar = character;
    }

    prevTransition = currTransition;
  }
}

static void join_equal_dfa_states(DfaS* const dfa_p,
                                  const int stateIdx1,
                                  const int stateIdx2)
{
  int lastStateIdx = dfa_p->numStates - 1;

  for (int i = 0; i < dfa_p->numStates; i++)
  {
    for (int j = 0; j < NUM_CHARS; j++)
    {
      if (dfa_p->states[i].transitions[j] == stateIdx2)
      {
        dfa_p->states[i].transitions[j] = stateIdx1;
      }
      else if (dfa_p->states[i].transitions[j] == lastStateIdx)
      {
        dfa_p->states[i].transitions[j] = stateIdx2;
      }
    }
  }

  memcpy(&dfa_p->states[stateIdx2], &dfa_p->states[lastStateIdx], sizeof(DfaStateS));
  dfa_p->numStates--;
}

/*> Global Function Definitions *******************************************************************/
DfaS* convert_to_dfa(const NfaS* const nfa_p)
{
  DfaS* dfa_p = malloc(sizeof(*dfa_p));
  dfa_p->numStates = 0;
  BitSetT powerSets[MAX_NUM_DFA_STATES] = {0};
  int nfaToDfaMap[MAX_NUM_NFA_STATES];
  memset(nfaToDfaMap, NO_STATE, sizeof(nfaToDfaMap));

  create_dfa_states(dfa_p, powerSets, nfaToDfaMap, nfa_p, 0);
  optimize_dfa(dfa_p);

  return dfa_p;
}

void print_dfa(const DfaS* const dfa_p)
{
  printf("DFA has %d states:\n", dfa_p->numStates);

  for (int i = 0; i < dfa_p->numStates; i++)
  {
    print_dfa_state(dfa_p, i);
  }
}
