#ifndef TYPES_H
#define TYPES_H

#include "constants.h"

typedef struct {
  char flight_number[MAX_FLIGHT_NUMBER];
  int seat_row;
  int seat_col;
  int dni;
} reservation_t;

#endif
