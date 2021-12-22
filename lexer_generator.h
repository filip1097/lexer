/*> Description ******************************************************************************************************/
/**
 * @brief Generates a lexer based on input regular expressions.
 * @file lexer_generator.h
 */

/*> Multiple Inclusion Protection ************************************************************************************/
#ifndef LEXER_GENERATOR_H
#define LEXER_GENERATOR_H

/*> Includes *********************************************************************************************************/

/*> Defines **********************************************************************************************************/

/*> Type Declarations ************************************************************************************************/
typedef struct LexerS
{
  int dummy;
} LexerS;

/*> Constant Declarations ********************************************************************************************/

/*> Variable Declarations ********************************************************************************************/

/*> Function Declarations ********************************************************************************************/
/**
 * @brief Generates a lexer based on the input regular expressions.
 * @param[in] regExpStrs_pp  Array of regular expressions as strings.
 * @param[in] numRegExps     The number of regular expressions.
 * @return The generated lexer.
 */
LexerS* generate_lexer(const char** const regExpStrs_pp, const int numRegExps);

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif
