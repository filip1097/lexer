/*> Description ******************************************************************************************************/
/**
 * @brief Deals with NFAs (nondeterministic finite automaton).
 * @file nfa.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef NFA_H 
#define NFA_H

/*> Includes *********************************************************************************************************/
#include "reg_exp.h"

#include <stdbool.h>

/*> Defines **********************************************************************************************************/
#define NUM_CHARS 256
#define MAX_NUM_EXPSILON_TRANSITIONS 25
#define MAX_NUM_NFA_STATES 64

/*> Type Declarations ************************************************************************************************/
/**
 * @brief A state in an NFA.
 * @param isEndState True if the state is an end state, false otherwise.
 * @param outputValue The output value returned once the NFA reaches its end state.
 * @param transitions The transitions to other states based on chars.
 * @param numEpsilonTransitions The number of epsilon transitions this state has.
 * @param epsilonTransitions The epsilon transitions from this state.
 */
typedef struct NfaStateS
{
  bool isEndState;
  int outputValue;
  struct NfaStateS* transitions[NUM_CHARS];
  int numEpsilonTransitions;
  struct NfaStateS* epsilonTransitions[MAX_NUM_EXPSILON_TRANSITIONS];
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

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/

/*> Function Declarations ********************************************************************************************/
/**
 * @brief Converts the input RegExp to an NFA.
 * @param[in]  regExp_p     The input RegExp.
 * @param[in]  outputValue  The value returned once the NFA reaches its end state.
 * @return Pointer to allocated NFA.
 */
NfaS* generate_nfa(const RegExpS* const regExp_p, const int outputValue);

/**
 * @brief Generates a combined NFA based on an array of RegExps.
 * @param[in]  regExps_pp   The input RegExps.
 * @param[in]  numRegExps   The number of RegExps.
 * @return Pointer to allocated NFA.
 */
NfaS* generate_combined_nfa(RegExpS** const regExps_pp, const int numRegExps);

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif 