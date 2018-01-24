#ifndef TYPES_H
#define TYPES_H

#include "constants.h"

typedef struct {
  char flight_number[MAX_FLIGHT_NUMBER+1];
  int seat_row;
  int seat_col;
  int dni;
} reservation_t;

typedef struct {
  int choice;
  char flight_number[MAX_FLIGHT_NUMBER+1];
  int dim[2];
} flight_t;

typedef struct {
  int choice;
  char flight_number[MAX_FLIGHT_NUMBER+1];
  int row;
  int col;
  int dni;
} flight_seat_t;

#endif
