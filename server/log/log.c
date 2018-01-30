#include <stdio.h>
#include "log.h"

#define GREEN_COLOR "\x1B[32m"
#define DEFAULT_COLOR "\x1B[0m"
#define RED_COLOR "\x1B[31m"

void print_error_msg(char * msg)
{
  printf(RED_COLOR"Error: "DEFAULT_COLOR"%s\n", msg);
}

void print_ok_msg(char * msg)
{
  printf(GREEN_COLOR"OK: "DEFAULT_COLOR"%s\n", msg);
}
