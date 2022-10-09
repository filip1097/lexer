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
/**
 * @brief A lexer; reads strings and returns tokens.
 *
 * @param input_p     Pointer to the input string.
 * @param currChar    The current char in the input string.
 */
typedef struct LexerS
{
  char* input_p;
  int currCharIdx;
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

/**
 * @brief Starts reading the provided input string.
 * @param[in/out]      Pointer to the lexer.
 * @param[in] input_p  Pointer to the string to read.
 */
void start_reading(LexerS* const lexer_p, char* const input_p);

/*> End of Multiple Inclusion Protection *****************************************************************************/
#endif
