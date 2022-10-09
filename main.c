/*> Description ***********************************************************************************/
/**
* @brief Contains the main method of the program.
* @file main.c
*/

/*> Includes **************************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "lexer_generator.h"

/*> Defines ***************************************************************************************/

/*> Type Declarations *****************************************************************************/

/*> Global Constant Definitions *******************************************************************/

/*> Global Variable Definitions *******************************************************************/

/*> Local Constant Definitions ********************************************************************/

/*> Local Variable Definitions ********************************************************************/

/*> Local Function Declarations *******************************************************************/

/*> Local Function Definitions ********************************************************************/
int main()
{
  const char* regExps[] = {"int", "char", "[0-9]+", "ba(g|d|[h,2])?(ab(hg)+)*"};
  char testString[] = {"intchar99900099"};

  LexerS* lexer_p = generate_lexer(regExps, 4);
  start_reading(lexer_p, testString);

  free(lexer_p);

  printf("Finished\n");
  return 0;
}

/*> Global Function Definitions *******************************************************************/
