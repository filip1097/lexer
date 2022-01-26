/*> Description ***********************************************************************************/
/**
 * @brief Deals with regular expressions.
 * @file reg_exp.h
 */

/*> Multiple Inclusion Protection *****************************************************************/
#ifndef REG_EXP_H
#define REG_EXP_H

/*> Includes **************************************************************************************/

/*> Defines ***************************************************************************************/
#define MAX_NUM_REGEXP_CHILDREN 20
#define MAX_REGEXP_STRING_LENGTH 100

/*> Type Declarations *****************************************************************************/
/**
 * @brief The type of a RegExp.
 */
typedef enum RegExpTypeE
{
  REGEXP_SEQUENCE,
  REGEXP_OPTIONAL,
  REGEXP_ONE_OR_MORE,
  REGEXP_ZERO_OR_MORE,
  REGEXP_OR,
  REGEXP_STRING,
  REGEXP_ONE_OF,
  REGEXP_RANGE
} RegExpTypeE;

/**
 * @brief A regular expression.
 * @param type        The type of the RegExp.
 * @param numChildren The number of children of this RegExp.
 * @param child_p     The first child of this RegExp.
 * @param children    Array containing the children of this RegExp.
 * @param left_p      The RegExp on the left side of an operator.
 * @param right_p     The RegExp on the right side of an operator.
 * @param characters  The characters contained in this RegExp. Only used for RegExp strings.
 * @param numChars    The number of characters of the string contained in the RegExp.
 */
typedef struct RegExpS
{
  RegExpTypeE type;
  
  int numChildren;
  union 
  {
    struct RegExpS* child_p;
    struct RegExpS* children[MAX_NUM_REGEXP_CHILDREN];
    struct 
    {
      struct RegExpS* left_p;
      struct RegExpS* right_p;
    };
    struct
    {
      char characters[MAX_REGEXP_STRING_LENGTH];
      int numChars;
    };
  };
} RegExpS;

/*> Constant Declarations *************************************************************************/

/*> Variable Declarations *************************************************************************/

/*> Function Declarations *************************************************************************/
/**
 * @brief Frees a regular expression.
 * @param[in]  regExp_p  The RegExp.
 */
void free_regexp(RegExpS* const regExp_p);

/**
 * @brief Frees an array of regular expressions.
 * @param[in]  regExps_pp  The RegExps.
 * @param[in]  numRegExps  The number of RegExps.
 */
void free_regexps(RegExpS** const regExps_pp, const int numRegExps);

/**
 * @brief Parses a regular expression.
 * @param[in] regExpString_p The regular expression as a string.
 * @return The regular expression.
 */
RegExpS* parse_regexp(const char* const regExpString_p);

/**
 * @brief Prints the structure of a regexp.
 * @param[in]  regExp_p     The RegExp to print.
 * @param[in]  indentation  The number of spaces to indent the RegExp info.
 */
void print_regexp(const RegExpS* const regExp_p, const int indentation);


/*> End of Multiple Inclusion Protection **********************************************************/
#endif
