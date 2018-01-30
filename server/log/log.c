#include <stdio.h>

#include "log.h"

#define GREEN_COLOR "\x1B[32m"
#define DEFAULT_COLOR "\x1B[0m"
#define RED_COLOR "\x1B[31m"

static int no_colors = 0;

void print_error_msg(char * msg)
{
  if(no_colors)
    printf("Error: %s\n", msg);
  else
    printf(RED_COLOR"Error: "DEFAULT_COLOR"%s\n", msg);
}

void print_ok_msg(char * msg)
{
  if(no_colors)
    printf("OK: %s\n", msg);
  else
    printf(GREEN_COLOR"OK: "DEFAULT_COLOR"%s\n", msg);
}

void colors_off()
{
  no_colors = 1;
}
