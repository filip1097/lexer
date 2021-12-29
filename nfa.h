/*> Description ***********************************************************************************/
/**
 * @brief Deals with NFAs (nondeterministic finite automaton).
 * @file nfa.h
 */

/*> Multiple Inclusion Protection *****************************************************************/
#ifndef NFA_H 
#define NFA_H

/*> Includes **************************************************************************************/
#include <limits.h>
#include <stdbool.h>

#include "bitset.h"
#include "reg_exp.h"

/*> Defines ***************************************************************************************/
#define MAX_NUM_NFA_STATES            64
#define NO_STATE                      UINT_MAX
#define NUM_CHARS                     256

/*> Type Declarations *****************************************************************************/
/**
 * @brief A state in an NFA.
 *
 * @param isEndState             True if the state is an end state, false otherwise.
 * @param outputValue            The output value returned once the NFA reaches its end state.
 * @param transitions            The indicies of states to transition to given a char.
 * @param epsilonTransitions     The indicies of states this state has an epsilon transition to as a
 *                               bit set.
 */
typedef struct NfaStateS
{
  bool isEndState;
  int outputValue;
  int transitions[NUM_CHARS];
  BitSetT epsilonTransitions;
} NfaStateS;

/**
 * @brief An NFA (nondeterministic finite automaton).
 * @param numStates The number of states of the NFA.
 * @param states The states of the NFA.
 */
typedef struct NfaS
{
  int numStates;
  NfaStateS states[MAX_NUM_NFA_STATES];
} NfaS;

/*> Constant Declarations *************************************************************************/

/*> Variable Declarations *************************************************************************/

/*> Function Declarations *************************************************************************/
/**
 * @brief Calculates the epsilon closure of the NFA state.
 * @param[in]  nfa_p     The NFA.
 * @param[in]  stateIdx  The index of the state.
 * @return Bit set containing the states in the epsilon closure.
 */
BitSetT epsilon_closure(const NfaS* const nfa_p, const int stateIdx);

/**
 * @brief Generates a combined NFA based on an array of RegExps.
 * @param[in]  regExps_pp   The input RegExps.
 * @param[in]  numRegExps   The number of RegExps.
 * @return Pointer to allocated NFA.
 */
NfaS* generate_combined_nfa(RegExpS** const regExps_pp, const int numRegExps);

/**
 * @brief Converts the input RegExp to an NFA.
 * @param[in]  regExp_p     The input RegExp.
 * @param[in]  outputValue  The value returned once the NFA reaches its end state.
 * @return Pointer to allocated NFA.
 */
NfaS* generate_nfa(const RegExpS* const regExp_p, const int outputValue);

/**
 * @brief Prints the NFA with all its states and transitions.
 * @param[in]  nfa_p  The NFA to print.
 */
void print_nfa(const NfaS* const nfa_p);

/*> End of Multiple Inclusion Protection **********************************************************/
#endif
