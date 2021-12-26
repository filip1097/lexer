/*> Description ***********************************************************************************/
/**
* @brief Deals with regular expressions.
* @file reg_exp.c
*/

/*> Includes **************************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_exp.h"

/*> Defines ***************************************************************************************/
#define MAX_NUM_REGEXP_TOKENS 100

#define PARSING_ERROR(...) {\
  printf("Parsing error for RegExp \"%s\":\n", currRegExpString_p);\
  printf(__VA_ARGS__);\
  printf("\n");\
  exit(1);\
}

/*> Type Declarations *****************************************************************************/
/**
 * @brief The type of a RegExp token.
 */
typedef enum RegExpTokenTypeE
{
  REGEXP_TOKEN_LEFT_PAR = '(',
  REGEXP_TOKEN_RIGHT_PAR = ')',
  REGEXP_TOKEN_STAR = '*',
  REGEXP_TOKEN_PLUS = '+',
  REGEXP_TOKEN_COMMA = ',',
  REGEXP_TOKEN_DASH = '-',
  REGEXP_TOKEN_QUESTION = '?',
  REGEXP_TOKEN_LEFT_BRACKET = '[',
  REGEXP_TOKEN_RIGHT_BRACKET = ']',
  REGEXP_TOKEN_OR = '|',
  REGEXP_TOKEN_STRING,
  REGEXP_TOKEN_END
} RegExpTokenTypeE;

/**
 * @brief Buffer of chars.
 * @param characters Array of chars.
 * @param numChars   The number of characters in the array.
 */
typedef struct RegExpCharBufferS
{
  char characters[MAX_REGEXP_STRING_LENGTH];
  int numChars;
} RegExpCharBufferS;

/**
 * @brief A RegExp token.
 * @param type The type of token.
 * @param characters The characters encompassed by this token (only set for strings).
 * @param numChars The number of characters (only set for strings).
 */
typedef struct RegExpTokenS
{
  RegExpTokenTypeE type;
  char characters[MAX_REGEXP_STRING_LENGTH];
  int numChars;
} RegExpTokenS;

/**
 * @brief A RegExp token array.
 * @param tokens    Array of tokens.
 * @param numTokens Number of tokens.
 */
typedef struct RegExpTokenArrayS
{
  RegExpTokenS tokens[MAX_NUM_REGEXP_TOKENS];
  int numTokens;
} RegExpTokenArrayS;

/**
 * @brief Holds RegExp parser related info.
 * @param tokenArray_p Pointer to the array of tokens.
 * @param tokenIndex   The index of the current token.
 * @param currToken_p  The current token of the parser.
 */
typedef struct RegExpParserS
{
  RegExpTokenArrayS* tokenArray_p;
  int tokenIndex;
  RegExpTokenS* currToken_p;
} RegExpParserS;

/*> Global Constant Definitions *******************************************************************/
static const char* currRegExpString_p;

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/
/**
 * @brief Tokenizes the regular expression and puts them into an array.
 * @param[out] tokenArray_p    The token array.
 * @param[in]  regExpString_p  The regular expression as a string.
 */
static void tokenize_regexp(RegExpTokenArrayS* const tokenArray_p,
                            const char* const regExpString_p);

/**
 * @brief Checks if a character is an operator character.
 * @param[in] ch  The char to be checked.
 * @return true or false.
 */
static bool is_regexp_operator_char(const char ch);

/**
 * @brief Prints the RegExp token.
 * @param[in] token_p The token to print.
 */
static void print_regexp_token(const RegExpTokenS* token_p);

/**
 * @brief Adds RegExp token to token array.
 * @param[in/out] tokenArray_p  Pointer to the array.
 * @param[in]     type          The type of token to be added.
 */
static void add_regexp_token(RegExpTokenArrayS* const tokenArray_p, const RegExpTokenTypeE type);

/**
 * @brief Adds RegExp string token to token array.
 * @param[in/out] tokenArray_p      Pointer to the array.
 * @param[in]     charBuffer_p      Char buffer containing characters to be added to token.
 */
static void add_regexp_string_token(RegExpTokenArrayS* const tokenArray_p,
                                    const RegExpCharBufferS* const charBuffer_p);

/**
 * @brief Start parsing a RegExp.
 *        Start -> Sequence END
 * @param[in/out]  parser_p  Parser related info.
 * @return A regular expression.
 */
static RegExpS* parse_start(RegExpParserS* const parser_p);

/**
 * @brief Parses a sequence of a RegExp components.
 *        Sequence -> Component+
 * @param[in/out]  parser_p  Parser related info.
 * @return A regular expression.
 */
static RegExpS* parse_sequence(RegExpParserS* const parser_p);

/**
 * @brief Parses a component of a RegExp.
 *        Component -> Factor ('|' Component)?
 * @param[in/out] parser_p  Parser related info.
 * @return A regular expression.
 */
static RegExpS* parse_component(RegExpParserS* const parser_p);

/**
 * @brief Parses a factor of a RegExp.
 *        Factor -> Term ('?' | '*' | '+')?
 * @param[in/out] parser_p  Parser related info.
 * @return A regular expression.
 */
static RegExpS* parse_factor(RegExpParserS* const parser_p);

/**
 * @brief Parses a term of a RegExp.
 *        Term -> STRING | '(' Sequence ')' | '[' List ']'
 * @param[in/out] parser_p  Parser related info.
 * @return A regular expression.
 */
static RegExpS* parse_term(RegExpParserS* const parser_p);

/**
 * @brief Parses a list of a RegExp.
 *        List -> ListComponent (',' ListComponent)*
 * @param[in/out] parser_p  Parser related info.
 * @return A regular expression.
 */
static RegExpS* parse_list(RegExpParserS* const parser_p);

/**
 * @brief Parses a list component of a RegExp.
 *        ListComponent -> STRING '-' STRING | STRING
 * @param[in/out] parser_p  Parser related info.
 * @return A regular expression.
 */
static RegExpS* parse_list_component(RegExpParserS* const parser_p);

/**
 * @brief Accepts current token if it is of specified type.
 * @param[in/out] parser_p   Parser related info.
 * @param[in]     tokenType  The type of token to accept.
 * @return true if the token was accepted, false otherwise.
 */
static bool parser_accept(RegExpParserS* const parser_p, const RegExpTokenTypeE tokenType);

/**
 * @brief Expects the current token to be of specified type, crashes otherwise.
 * @param[in/out] parser_p   Parser related info.
 * @param[in]     tokenType  The type of the token that is expected.
 */
static void parser_expect(RegExpParserS* const parser_p, const RegExpTokenTypeE tokenType);

/**
 * @brief Updates parser information for next token.
 * @param[in/out] parser_p  Parser related info.
 */
static void parser_next_token(RegExpParserS* const parser_p);

/**
 * @brief Allocates memory for RegExp and sets type.
 * @param[in] regExpType  The type of the RegExp.
 * @return Pointer to the allocated RegExp.
 */
static RegExpS* create_regexp(const RegExpTypeE regExpType);

/**
 * @brief Allocates memory for a RegExp string and sets type and copies characters.
 * @param[in] stringToken_p  The string token containing string for RegExp.
 * @return Pointer to the allocated RegExp.
 */
static RegExpS* create_regexp_string(RegExpTokenS* const stringToken_p);

/**
 * @brief Adds child RegExp to parent RegExp.
 * @param[out] parent_p  The parent RegExp.
 * @param[in]  child_p   The child RegExp.
 */
static void add_child_to_regexp(RegExpS* const parent_p, RegExpS* const child_p);

/**
 * @brief Checks the RegExp is correctly formatted. Crashes otherwise.
 * @param[in] regexp_p  The RegExp.
 */
static void check_regexp_format(const RegExpS* const regexp_p);

/*> Local Function Definitions ********************************************************************/
static void tokenize_regexp(RegExpTokenArrayS* tokenArray_p, const char* const regExpString_p)
{
  RegExpCharBufferS charBuffer = { .numChars = 0 };
  int charIndex = 0;
  bool escapeNextChar = false;
  char currChar = regExpString_p[charIndex];

  while (currChar != '\0')
  {
    if (is_regexp_operator_char(regExpString_p[charIndex]) && !escapeNextChar)
    {
      if (charBuffer.numChars > 0)
      {
        add_regexp_string_token(tokenArray_p, &charBuffer);
        charBuffer.numChars = 0;
      }
      
      add_regexp_token(tokenArray_p, currChar);
      escapeNextChar = false;
    }
    else if (currChar == '\\' && !escapeNextChar)
    {
      escapeNextChar = true;
    }
    else
    {
      charBuffer.characters[charBuffer.numChars] = currChar;
      (charBuffer.numChars)++;
      assert(charBuffer.numChars <= MAX_REGEXP_STRING_LENGTH);
      escapeNextChar = false;
    }

    charIndex++;
    currChar = regExpString_p[charIndex];
  } 

  if (charBuffer.numChars > 0)
  {
    add_regexp_string_token(tokenArray_p, &charBuffer);
  }

  add_regexp_token(tokenArray_p, REGEXP_TOKEN_END);
}

static bool is_regexp_operator_char(const char ch)
{
  return (ch == '(') || (ch == ')') || (ch == '*') || (ch == '+') || (ch == ',') ||
         (ch == '-') || (ch == '?') || (ch == '[') || (ch == ']') || (ch == '|');
}

static void print_regexp_token(const RegExpTokenS* token_p)
{
  if (token_p->type == REGEXP_TOKEN_STRING)
  {
    printf("'%.*s'", token_p->numChars, token_p->characters);
  }
  else if (token_p->type == REGEXP_TOKEN_END)
  {
    printf("END");
  }
  else
  {
    printf("'%c'", token_p->type);
  }
}

static void add_regexp_token(RegExpTokenArrayS* const tokenArray_p, const RegExpTokenTypeE type)
{
  tokenArray_p->tokens[tokenArray_p->numTokens].type = type;
  (tokenArray_p->numTokens)++;
  assert(tokenArray_p->numTokens <= MAX_NUM_REGEXP_TOKENS);
}

static void add_regexp_string_token(RegExpTokenArrayS* const tokenArray_p,
                                    const RegExpCharBufferS* const charBuffer_p)
{
  tokenArray_p->tokens[tokenArray_p->numTokens].type = REGEXP_TOKEN_STRING;
  memcpy(tokenArray_p->tokens[tokenArray_p->numTokens].characters,
         charBuffer_p->characters,
         charBuffer_p->numChars);
  tokenArray_p->tokens[tokenArray_p->numTokens].numChars = charBuffer_p->numChars;
        
  (tokenArray_p->numTokens)++;
  assert(tokenArray_p->numTokens <= MAX_NUM_REGEXP_TOKENS);
}

static RegExpS* parse_start(RegExpParserS* const parser_p)
{
  RegExpS* sequence_p = parse_sequence(parser_p);
  parser_expect(parser_p, REGEXP_TOKEN_END);
  return sequence_p;
}

static RegExpS* parse_sequence(RegExpParserS* const parser_p)
{
  RegExpS* regExpSequence_p = create_regexp(REGEXP_SEQUENCE);
  RegExpTokenTypeE nextTokenType;
  do
  {
    RegExpS* component_p = parse_component(parser_p);
    add_child_to_regexp(regExpSequence_p, component_p);
    nextTokenType = parser_p->currToken_p->type;
  } while (nextTokenType != REGEXP_TOKEN_END && 
           nextTokenType != REGEXP_TOKEN_RIGHT_PAR && 
           nextTokenType != REGEXP_TOKEN_RIGHT_BRACKET);
  return regExpSequence_p;
}

static RegExpS* parse_component(RegExpParserS* const parser_p)
{
  RegExpS* factor_p = parse_factor(parser_p);
  if (parser_accept(parser_p, '|'))
  {
    RegExpS* component_p = parse_component(parser_p);
    RegExpS* orRegExp_p = create_regexp(REGEXP_OR);
    add_child_to_regexp(orRegExp_p, factor_p);
    add_child_to_regexp(orRegExp_p, component_p);
    return orRegExp_p;
  }
  else 
  {
    return factor_p;
  }
}

static RegExpS* parse_term(RegExpParserS* const parser_p)
{
  RegExpTokenS* currToken_p = parser_p->currToken_p;
  if (parser_accept(parser_p, REGEXP_TOKEN_STRING))
  {
    RegExpS* stringRegExp_p = create_regexp_string(currToken_p);
    return stringRegExp_p;
  }
  else if (parser_accept(parser_p, '('))
  {
    RegExpS* sequence_p = parse_sequence(parser_p);
    parser_expect(parser_p, ')');
    return sequence_p;
  }
  else if (parser_accept(parser_p, '['))
  {
    RegExpS* list_p = parse_list(parser_p);
    parser_expect(parser_p, ']');
    return list_p;
  }
  else
  {
    PARSING_ERROR("Expected string, '(', or '[', but got %c", parser_p->currToken_p->type);
  }
}

static RegExpS* parse_factor(RegExpParserS* const parser_p)
{
  RegExpS* term_p = parse_term(parser_p);
  if (parser_accept(parser_p, '?'))
  {
    RegExpS* optionalRegExp_p = create_regexp(REGEXP_OPTIONAL);
    add_child_to_regexp(optionalRegExp_p, term_p);
    return optionalRegExp_p;
  }
  else if (parser_accept(parser_p, '*'))
  {
    RegExpS* zeroOrMoreRegExp_p = create_regexp(REGEXP_ZERO_OR_MORE);
    add_child_to_regexp(zeroOrMoreRegExp_p, term_p);
    return zeroOrMoreRegExp_p;
  }
  else if (parser_accept(parser_p, '+'))
  {
    RegExpS* oneOrMoreRegExp_p = create_regexp(REGEXP_ONE_OR_MORE);
    add_child_to_regexp(oneOrMoreRegExp_p, term_p);
    return oneOrMoreRegExp_p;
  }
  else
  {
    return term_p;
  }
}

static RegExpS* parse_list(RegExpParserS* const parser_p)
{
  RegExpS* oneOfRegExp_p = create_regexp(REGEXP_ONE_OF);
  RegExpS* firstListComponent_p = parse_list_component(parser_p);
  add_child_to_regexp(oneOfRegExp_p, firstListComponent_p);
  while (parser_accept(parser_p, ','))
  {
    RegExpS* otherListComponent_p = parse_list_component(parser_p);
    add_child_to_regexp(oneOfRegExp_p, otherListComponent_p);
  }
  return oneOfRegExp_p;
}

static RegExpS* parse_list_component(RegExpParserS* const parser_p)
{
  RegExpTokenS* currToken_p = parser_p->currToken_p;
  parser_expect(parser_p, REGEXP_TOKEN_STRING);
  RegExpS* firstRegExpString_p = create_regexp_string(currToken_p);
  if (parser_accept(parser_p, '-'))
  {
    currToken_p = parser_p->currToken_p;
    parser_expect(parser_p, REGEXP_TOKEN_STRING);
    RegExpS* secondRegExpString_p = create_regexp_string(currToken_p);
    RegExpS* rangeRegExp_p = create_regexp(REGEXP_RANGE);
    add_child_to_regexp(rangeRegExp_p, firstRegExpString_p);
    add_child_to_regexp(rangeRegExp_p, secondRegExpString_p);
    return rangeRegExp_p;
  }
  else
  {
    return firstRegExpString_p;
  }
}

static bool parser_accept(RegExpParserS* const parser_p, const RegExpTokenTypeE tokenType)
{
  if (parser_p->currToken_p->type == tokenType)
  {
    parser_next_token(parser_p);
    return true; 
  }
  else
  {
    return false;
  }
}

static void parser_expect(RegExpParserS* const parser_p, const RegExpTokenTypeE tokenType)
{
  if (parser_p->currToken_p->type == tokenType)
  {
    parser_next_token(parser_p);
  }
  else
  {
    PARSING_ERROR("Expected '%c', but got '%c'\n", tokenType, parser_p->currToken_p->type);
  }
}

static void parser_next_token(RegExpParserS* const parser_p)
{
  parser_p->tokenIndex++;
  assert(parser_p->tokenIndex <= parser_p->tokenArray_p->numTokens);
  parser_p->currToken_p = &(parser_p->tokenArray_p->tokens[parser_p->tokenIndex]);
}

static RegExpS* create_regexp(const RegExpTypeE regExpType)
{
  RegExpS* newRegExp_p = malloc(sizeof(*newRegExp_p));
  newRegExp_p->type = regExpType;
  newRegExp_p->numChildren = 0;
  return newRegExp_p;
}

static RegExpS* create_regexp_string(RegExpTokenS* const stringToken_p)
{
  RegExpS* newRegExpString_p = create_regexp(REGEXP_STRING);
  memcpy(newRegExpString_p->characters, stringToken_p->characters, stringToken_p->numChars);
  newRegExpString_p->numChars = stringToken_p->numChars;
  return newRegExpString_p;
}

static void add_child_to_regexp(RegExpS* const parent_p, RegExpS* const child_p)
{
  parent_p->children[parent_p->numChildren] = child_p;
  parent_p->numChildren++;
  assert(parent_p->numChildren <= MAX_NUM_REGEXP_CHILDREN);
}

static void check_regexp_format(const RegExpS* const regexp_p)
{
  if (regexp_p->type == REGEXP_RANGE)
  {
    assert(regexp_p->left_p->numChars == 1);
    assert(regexp_p->right_p->numChars == 1);
    assert(regexp_p->left_p->characters[0] <= regexp_p->right_p->characters[0]);
  }

  for (int i = 0; i < regexp_p->numChildren; i++)
  {
    check_regexp_format(regexp_p->children[i]);
  }
}

/*> Global Function Definitions *******************************************************************/
void free_regexp(RegExpS* const regExpString_p)
{
  for (int i = 0; i < regExpString_p->numChildren; i++)
  {
    free_regexp(regExpString_p->children[i]);
  }
  free(regExpString_p);
}

RegExpS* parse_regexp(const char* const regExpString_p)
{
  currRegExpString_p = regExpString_p;
  RegExpTokenArrayS tokenArray = { .numTokens = 0 };
  tokenize_regexp(&tokenArray, regExpString_p);
  RegExpParserS parser =
  {
    .tokenArray_p = &tokenArray,
    .tokenIndex = 0,
    .currToken_p = &(tokenArray.tokens[0])
  };
  RegExpS* regexp_p = parse_start(&parser);
  check_regexp_format(regexp_p);
  return regexp_p;
}

void print_regexp(const RegExpS* const regExp_p, const int indentation)
{
  for (int i = 0; i < indentation; i++)
  {
    printf(" ");
  }
  switch(regExp_p->type)
  {
  case REGEXP_SEQUENCE:
    printf("Sequence\n");
    break;
  case REGEXP_OPTIONAL:
    printf("Optional\n");
    break;
  case REGEXP_ONE_OR_MORE:
    printf("OneOrMore\n");
    break;
  case REGEXP_ZERO_OR_MORE:
    printf("ZeroOrMore\n");
    break;
  case REGEXP_OR:
    printf("Or\n");
    break;
  case REGEXP_STRING:
    printf("String(\"");
    for (int i = 0; i < regExp_p->numChars; i++)
    {
      printf("%c", regExp_p->characters[i]);
    }
    printf("\")\n");
    break;
  case REGEXP_ONE_OF:
    printf("OneOf\n");
    break;
  case REGEXP_RANGE:
    printf("Range\n");
    break;
  }

  for (int i = 0; i < regExp_p->numChildren; i++)
  {
    print_regexp(regExp_p->children[i], indentation + 1);
  }
}
