/*> Description ***********************************************************************************/
/**
* @brief Deals NFAs (nondeterministic finite automaton).
* @file nfa.c
*/

/*> Includes **************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bitset.h"
#include "nfa.h"

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/
/**
 * @brief Indicies of start and end states of an NFA or part of an NFA.
 * @param startIdx  Start state index.
 * @param endIdx    End state index.
 */
typedef struct StartAndEndStatesS
{
  int startIdx;
  int endIdx;
} StartAndEndStateS;

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/
/**
 * @brief Converts the RegExp and adds any generated states to the provided NFA. Based on Thompson's
 *        construction.
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
 * @brief Adds a new state to NFA and returns the index of it.
 * @param[out] nfa_p  The NFA. 
 * @return Index of the new state.
 */
static int add_new_state(NfaS* const nfa_p);

/**
 * @brief Adds an end state to NFA and returns the index of it.
 * @param[out]  nfa_p     The NFA.
 * @param[in]   outValue  The value returned once the NFA reaches its end state.
 * @return Index of the new state.
 */
static int add_end_state(NfaS* const nfa_p, const int outputValue);

/**
 * @brief Adds an epsilon transition between start and end state in the NFA.
 * @param[in/out]  nfa_p     The NFA.
 * @param[in]      startIdx  The index of the start state.
 * @param[in]      endIdx    The index of the end state.
 */
static void add_epsilon_transition(NfaS* const nfa_p, const int startIdx, const int endIdx);

/**
 * @brief Prints an NFA state with all its transitions.
 * @param[in]  nfa_p     The NFA.
 * @param[in]  stateIdx  The index of the state to be printed.
 */
static void print_nfa_state(const NfaS* const nfa_p, const int stateIdx);

/*> Local Function Definitions ********************************************************************/
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
    StartAndEndStateS noStates = {NO_STATE, NO_STATE};
    return noStates;
  }
  }
}

static StartAndEndStateS convert_sequence(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {NO_STATE, NO_STATE};
  for (int i = 0; i < regExp_p->numChildren; i++)
  {
    StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->children[i]);
    if (i == 0)
    {
      startAndEnd.startIdx = startAndEndOfNewConvert.startIdx;
      startAndEnd.endIdx = startAndEndOfNewConvert.endIdx;
    }
    else
    {
      add_epsilon_transition(nfa_p, startAndEnd.endIdx, startAndEndOfNewConvert.startIdx);
      startAndEnd.endIdx = startAndEndOfNewConvert.endIdx;
    }
  }
  return startAndEnd;
}

static StartAndEndStateS convert_optional(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEnd.endIdx);
  StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->child_p);
  add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEndOfNewConvert.startIdx);
  add_epsilon_transition(nfa_p, startAndEndOfNewConvert.endIdx, startAndEnd.endIdx);
  return startAndEnd;
}

static StartAndEndStateS convert_zero_or_more(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEnd.endIdx);
  StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->child_p);
  add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEndOfNewConvert.startIdx);
  add_epsilon_transition(nfa_p, startAndEndOfNewConvert.endIdx, startAndEnd.endIdx);
  add_epsilon_transition(nfa_p, startAndEndOfNewConvert.endIdx, startAndEndOfNewConvert.startIdx);
  return startAndEnd;
}

static StartAndEndStateS convert_one_or_more(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->child_p);
  add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEndOfNewConvert.startIdx);
  add_epsilon_transition(nfa_p, startAndEndOfNewConvert.endIdx, startAndEnd.endIdx);
  add_epsilon_transition(nfa_p, startAndEndOfNewConvert.endIdx, startAndEndOfNewConvert.startIdx);
  return startAndEnd;
}

static StartAndEndStateS convert_or(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  StartAndEndStateS startAndEndOfLeft = convert(nfa_p, regExp_p->left_p);
  StartAndEndStateS startAndEndOfRight = convert(nfa_p, regExp_p->right_p);
  add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEndOfLeft.startIdx);
  add_epsilon_transition(nfa_p, startAndEndOfLeft.endIdx, startAndEnd.endIdx);
  add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEndOfRight.startIdx);
  add_epsilon_transition(nfa_p, startAndEndOfRight.endIdx, startAndEnd.endIdx);
  return startAndEnd;
}

static StartAndEndStateS convert_string(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), NO_STATE};
  for (int i = 0; i < regExp_p->numChars; i++)
  {
    char transition = regExp_p->characters[i];
    int newStateIdx = add_new_state(nfa_p);
    if (i == 0)
    {
      NfaStateS* startState_p = &(nfa_p->states[startAndEnd.startIdx]);
      startState_p->transitions[transition] = newStateIdx;
    }
    else
    {
      NfaStateS* endState_p = &(nfa_p->states[startAndEnd.endIdx]);
      endState_p->transitions[transition] = newStateIdx;
    }
    startAndEnd.endIdx = newStateIdx;
  }
  return startAndEnd;
}

static StartAndEndStateS convert_one_of(NfaS* const nfa_p, const RegExpS* const regExp_p)
{
  StartAndEndStateS startAndEnd = {add_new_state(nfa_p), add_new_state(nfa_p)};
  for (int i = 0; i < regExp_p->numChildren; i++)
  {
    StartAndEndStateS startAndEndOfNewConvert = convert(nfa_p, regExp_p->children[i]);
    add_epsilon_transition(nfa_p, startAndEnd.startIdx, startAndEndOfNewConvert.startIdx);
    add_epsilon_transition(nfa_p, startAndEndOfNewConvert.endIdx, startAndEnd.endIdx);
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
    NfaStateS* startState_p = &(nfa_p->states[startAndEnd.startIdx]);
    startState_p->transitions[i] = startAndEnd.endIdx;
  }
  return startAndEnd;
}

static int add_new_state(NfaS* const nfa_p)
{
  int newStateIdx = nfa_p->numStates;
  nfa_p->numStates++;
  assert(nfa_p->numStates < MAX_NUM_NFA_STATES);

  NfaStateS* newState_p = &(nfa_p->states[newStateIdx]);
  newState_p->epsilonTransitions = 0;
  newState_p->isEndState = false;
  memset(newState_p->transitions, NO_STATE, sizeof(newState_p->transitions[0]) * NUM_CHARS);

  return newStateIdx;
}

static int add_end_state(NfaS* const nfa_p, const int outputValue)
{
  int newStateIdx = add_new_state(nfa_p);
  NfaStateS* newState_p = &(nfa_p->states[newStateIdx]);
  newState_p->isEndState = true;
  newState_p->outputValue = outputValue;
  return newStateIdx;
}

static void add_epsilon_transition(NfaS* const nfa_p, const int startIdx, const int endIdx)
{
  NfaStateS* start_p = &(nfa_p->states[startIdx]);
  add_to_bitset(&(start_p->epsilonTransitions), endIdx);
}

static void print_nfa_state(const NfaS* const nfa_p, const int stateIdx)
{
  const NfaStateS* nfaState_p = &(nfa_p->states[stateIdx]);

  if (nfaState_p->isEndState)
  {
    printf("-State Q%d | End state : %d\n", stateIdx, nfaState_p->outputValue);
  }
  else
  {
    printf("-State Q%d\n", stateIdx);
  }

  int prevTransition = NO_STATE;
  char prevTransitionFirstChar = 0;
  for (int character = 0; character < NUM_CHARS; character++)
  {
    int currTransition = nfaState_p->transitions[character];

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

  for (int i = 0; i < nfa_p->numStates; i++)
  {
    if (is_in_bitset(&(nfaState_p->epsilonTransitions), i))
    {
      printf(" *Transition eps -> Q%d\n", i);
    }
  }
}

/*> Global Function Definitions *******************************************************************/
NfaS* generate_combined_nfa(RegExpS** const regExps_pp, const int numRegExps)
{
  NfaS* nfa_p = malloc(sizeof(*nfa_p));
  memset(nfa_p, 0, sizeof(*nfa_p));

  int startIdx = add_new_state(nfa_p);
  for (int i = 0; i < numRegExps; i++)
  {
    int regExpStartIdx = add_new_state(nfa_p);
    int endIdx = add_end_state(nfa_p, i);
    StartAndEndStateS startAndEndOfConverted = convert(nfa_p, regExps_pp[i]);
    add_epsilon_transition(nfa_p, startIdx, regExpStartIdx);
    add_epsilon_transition(nfa_p, regExpStartIdx, startAndEndOfConverted.startIdx);
    add_epsilon_transition(nfa_p, startAndEndOfConverted.endIdx, endIdx);
  }

  return nfa_p;
}

NfaS* generate_nfa(const RegExpS* const regExp_p, const int outputValue)
{
  NfaS* nfa_p = malloc(sizeof(*nfa_p));
  nfa_p->numStates = 0;

  int startIdx = add_new_state(nfa_p);
  int endIdx = add_end_state(nfa_p, outputValue);
  StartAndEndStateS startAndEndOfConverted = convert(nfa_p, regExp_p);
  add_epsilon_transition(nfa_p, startIdx, startAndEndOfConverted.startIdx);
  add_epsilon_transition(nfa_p, startAndEndOfConverted.endIdx, endIdx);

  return nfa_p;
}

void print_nfa(const NfaS* const nfa_p)
{
  printf("NFA has %d states:\n", nfa_p->numStates);

  for (int i = 0; i < nfa_p->numStates; i++)
  {
    print_nfa_state(nfa_p, i);
  }
}
