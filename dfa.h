/*> Description ***********************************************************************************/
/**
 * @brief Deals with DFAs (deterministic finite automaton).
 * @file dfa.h
 */

/*> Multiple Inclusion Protection *****************************************************************/
#ifndef DFA_H 
#define DFA_H

/*> Includes **************************************************************************************/
#include "nfa.h"

#include <stdbool.h>

/*> Defines ***************************************************************************************/
#define NUM_CHARS 256
#define MAX_NUM_DFA_STATES 64

/*> Type Declarations *****************************************************************************/
/**
 * @brief A state in an DFA.
 * @param  isEndState   True if the state is an end state, false otherwise.
 * @param  outputValue  The value this state returns if it is an end state.
 * @param  transitions  The transitions to other states based on chars.
 */
typedef struct DfaStateS
{
  bool isEndState;
  int outputValue;
  int transitions[NUM_CHARS];
} DfaStateS;

/**
 * @brief An DFA (deterministic finite automaton).
 * @param numStates The number of states of the DFA.
 * @param states The states of the DFA.
 */
typedef struct DfaS
{
  int numStates;
  DfaStateS states[MAX_NUM_DFA_STATES];
} DfaS;

/*> Constant Declarations *************************************************************************/

/*> Variable Declarations *************************************************************************/

/*> Function Declarations *************************************************************************/
/**
 * @brief Converts the input NFA to an DFA.
 * @param[in]  nfa_p  The input NFA.
 * @return Pointer to allocated DFA.
 */
DfaS* convert_to_dfa(const NfaS* const nfa_p);

/**
 * @brief Prints the DFA with all its states and transitions.
 * @param[in]  dfa_p  The DFA to print.
 */
void print_dfa(const DfaS* const dfa_p);

/*> End of Multiple Inclusion Protection **********************************************************/
#endif 
