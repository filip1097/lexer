/*> Description ***********************************************************************************/
/**
* @brief Generates a lexer based on input regular expressions.
* @file lexer_generator.c
*/

/*> Includes **************************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dfa.h"
#include "lexer_generator.h"
#include "nfa.h"
#include "reg_exp.h"

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/

/*> Local Function Definitions ********************************************************************/

/*> Global Function Definitions *******************************************************************/
LexerS* generate_lexer(const char** const regExpStrs_pp, const int numRegExps)
{
  RegExpS* regExps[numRegExps];

  for (int i = 0; i < numRegExps; i++)
  {
    printf("Parsing the regExp: %s\n", regExpStrs_pp[i]);
    regExps[i] = parse_regexp(regExpStrs_pp[i]);
    printf("The output is:\n");
    print_regexp(regExps[i], 0);
  }

  NfaS* nfa_p = generate_combined_nfa(regExps, numRegExps);
  DfaS* dfa_p = convert_to_dfa(nfa_p);

  // free_regexps(regExps, numRegExps);
  free(nfa_p);

  return NULL;
}
