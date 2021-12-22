/*> Description ******************************************************************************************************/
/**
* @brief Deals NFAs (nondeterministic finite automaton).
* @file nfa.c
*/

/*> Includes *********************************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "nfa.h"

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/
/**
 * @brief Start and end states of an NFA or part of an NFA.
 * @param start_p Start state.
 * @param end_p   End state.
 */
typedef struct StartAndEndStatesS
{
  NfaStateS* start_p;
  NfaStateS* end_p;
} StartAndEndStateS;

/*> Global Constant Definitions **************************************************************************************/

/*> Global Variable Definitions **************************************************************************************/

/*> Local Constant Definitions ***************************************************************************************/

/*> Local Variable Definitions ***************************************************************************************/

/*> Local Function Declarations **************************************************************************************/
/**
 * @brief Converts the RegExp and adds any generated states to the provided NFA. Based on Thompson's construction.
 * @param[in/out]  nfa_p     The NFA. 
 * @param[in]      regExp_p  The RegExp to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp Sequence and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp Sequence to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_sequence(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp Optional and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp Optional to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_optional(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp OneOrMore and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp OneOrMore to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_one_or_more(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp ZeroOrMore and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp ZeroOrMore to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_zero_or_more(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp Or and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp Or to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_or(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp String and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp String to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_string(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp OneOf and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp String to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_one_of(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Converts the RegExp Range and adds any generated states to the provided NFA.
 * @param[out] nfa_p     The NFA. 
 * @param[in]  regExp_p  The RegExp Range to convert.
 * @return Struct with the start and end states generated part of the nfa.
 */
static StartAndEndStateS convert_range(NfaS* const nfa_p, const RegExpS* const regExp_p);

/**
 * @brief Adds new state to NFA and returns pointer to it.
 * @param[out] nfa_p  The NFA. 
 * @return Pointer to the state.
 */
static NfaStateS* add_new_state(NfaS* const nfa_p);

/**
 * @brief Adds end state to NFA and returns pointer to it.
 * @param[out] nfa_p    The NFA.
 * @param[in]  outValue The value returned once the NFA reaches its end state.
 * @return Pointer to the end state.
 */
static NfaStateS* add_end_state(NfaS* const nfa_p, const int outputValue);

/**
 * @brief Adds an epsilon transition between start and end state.
 * @param[in/out] start_p  The start state. 
 * @param[in]     end_p    The end state. 
 */
static void add_epsilon_transition(NfaStateS* const start_p, NfaStateS* const end_p);

/*> Local Function Definitions ***************************************************************************************/
static StartAndEndStateS convert(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  switch (regExp_p->type)
  {
  case REGEXP_SEQUENCE:
    return convert_sequence(nfa_p, regExp_p);
  case REGEXP_OPTIONAL:
    return convert_optional(nfa_p, regExp_p);
  case REGEXP_ONE_OR_MORE:
    return convert_one_or_more(nfa_p, regExp_p);
  case REGEXP_ZERO_OR_MORE:
    return convert_zero_or_more(nfa_p, regExp_p);
  case REGEXP_OR:
    return convert_or(nfa_p, regExp_p);
  case REGEXP_STRING:
    return convert_string(nfa_p, regExp_p);
  case REGEXP_ONE_OF:
    return convert_one_of(nfa_p, regExp_p);
  case REGEXP_RANGE:
    return convert_range(nfa_p, regExp_p);
  default:
  {
    StartAndEndStateS nullStates = {NULL, NULL};
    return nullStates;
  }
  }
}

static StartAndEndStateS convert_sequence(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {NULL, NULL};
  for (int i = 0; i < regExp_p->numChildren; i++)
  {
    StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->children[i]);
    if (i == 0)
    {
      startAndEnd.start_p = startAndEndOfNewConvert.start_p;
      startAndEnd.end_p = startAndEndOfNewConvert.end_p;
    }
    else
    {
      add_epsilon_transition(startAndEnd.end_p, startAndEndOfNewConvert.start_p);
      startAndEnd.end_p = startAndEndOfNewConvert.end_p;
    }
  }
  return startAndEnd;
}

static StartAndEndStateS convert_optional(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  add_epsilon_transition(startAndEnd.start_p, startAndEnd.end_p);
  StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->child_p);
  add_epsilon_transition(startAndEnd.start_p, startAndEndOfNewConvert.start_p);
  add_epsilon_transition(startAndEndOfNewConvert.end_p, startAndEnd.end_p);
  return startAndEnd;
}

static StartAndEndStateS convert_zero_or_more(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  add_epsilon_transition(startAndEnd.start_p, startAndEnd.end_p);
  StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->child_p);
  add_epsilon_transition(startAndEnd.start_p, startAndEndOfNewConvert.start_p);
  add_epsilon_transition(startAndEndOfNewConvert.end_p, startAndEnd.end_p);
  add_epsilon_transition(startAndEndOfNewConvert.end_p, startAndEndOfNewConvert.start_p);
  return startAndEnd;
}

static StartAndEndStateS convert_one_or_more(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->child_p);
  add_epsilon_transition(startAndEnd.start_p, startAndEndOfNewConvert.start_p);
  add_epsilon_transition(startAndEndOfNewConvert.end_p, startAndEnd.end_p);
  add_epsilon_transition(startAndEndOfNewConvert.end_p, startAndEndOfNewConvert.start_p);
  return startAndEnd;
}

static StartAndEndStateS convert_or(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  StartAndEndStateS startAndEndOfLeft = convert(nfa_p, regExp_p->left_p);
  StartAndEndStateS startAndEndOfRight = convert(nfa_p, regExp_p->right_p);
  add_epsilon_transition(startAndEnd.start_p, startAndEndOfLeft.start_p);
  add_epsilon_transition(startAndEndOfLeft.end_p, startAndEnd.end_p);
  add_epsilon_transition(startAndEnd.start_p, startAndEndOfRight.start_p);
  add_epsilon_transition(startAndEndOfRight.end_p, startAndEnd.end_p);
  return startAndEnd;
}

static StartAndEndStateS convert_string(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), NULL};
  for (int i = 0; i < regExp_p->numChars; i++)
  {
    char transition = regExp_p->characters[i];
    NfaStateS* newState = add_new_state(nfa_p);
    if (i == 0) startAndEnd.start_p->transitions[transition] = newState;
    else        startAndEnd.end_p->transitions[transition] = newState;
    startAndEnd.end_p = newState;
  }
  return startAndEnd;
}

static StartAndEndStateS convert_one_of(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  for (int i = 0; i < regExp_p->numChildren; i++)
  {
    StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->children[i]);
    add_epsilon_transition(startAndEnd.start_p, startAndEndOfNewConvert.start_p);
    add_epsilon_transition(startAndEndOfNewConvert.end_p, startAndEnd.end_p);
  }
  return startAndEnd;
}

static StartAndEndStateS convert_range(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  char leftChar = regExp_p->left_p->characters[0];
  char rightChar = regExp_p->right_p->characters[0];
  for (char i = leftChar; i <= rightChar; i++)
  {
    startAndEnd.start_p->transitions[i] = startAndEnd.end_p;
  }
  return startAndEnd;
}

static NfaStateS* add_new_state(NfaS* const nfa_p)
{
  NfaStateS* newState = &(nfa_p->states[nfa_p->numStates]);
  nfa_p->numStates++;
  assert(nfa_p->numStates < MAX_NUM_NFA_STATES);
  return newState;
}

static NfaStateS* add_end_state(NfaS* const nfa_p, const int outputValue)
{
  NfaStateS* newState = add_new_state(nfa_p);
  newState->isEndState = true;
  newState->outputValue = outputValue;
  return newState;
}

static void add_epsilon_transition(NfaStateS* const start_p, NfaStateS* const end_p)
{
  start_p->epsilonTransitions[start_p->numEpsilonTransitions] = end_p;
  assert(start_p->numEpsilonTransitions < MAX_NUM_EXPSILON_TRANSITIONS);
  start_p->numEpsilonTransitions++;
}

/*> Global Function Definitions **************************************************************************************/
NfaS* generate_nfa(const RegExpS* const regExp_p, const int outputValue)
{
  NfaS* nfa_p = malloc(sizeof(*nfa_p));
  memset(nfa_p, 0, sizeof(*nfa_p));

  NfaStateS* start_p = add_new_state(nfa_p);
  NfaStateS* end_p = add_end_state(nfa_p, outputValue);
  StartAndEndStateS startAndEndOfConverted = convert(nfa_p, regExp_p);
  add_epsilon_transition(start_p, startAndEndOfConverted.start_p);
  add_epsilon_transition(startAndEndOfConverted.end_p, end_p);

  return nfa_p;
}

NfaS* generate_combined_nfa(RegExpS** const regExps_pp, const int numRegExps)
{
  NfaS* nfa_p = malloc(sizeof(*nfa_p));
  memset(nfa_p, 0, sizeof(*nfa_p));

  NfaStateS* start_p = add_new_state(nfa_p);
  for (int i = 0; i < numRegExps; i++)
  {
    NfaStateS* regExpStart_p = add_new_state(nfa_p);
    NfaStateS* end_p = add_end_state(nfa_p, i);
    StartAndEndStateS startAndEndOfConverted = convert(nfa_p, regExps_pp[i]);
    add_epsilon_transition(start_p, regExpStart_p);
    add_epsilon_transition(regExpStart_p, startAndEndOfConverted.start_p);
    add_epsilon_transition(startAndEndOfConverted.end_p, end_p);
  }

  return nfa_p;
}