#ifndef FRONTEND_H
#define FRONTEND_H

#include "../../protocol/types.h"
int show_menu();

void print_error_msg(char * msg);
void print_ok_msg(char * msg);
void print_flight(flight_t * flight);
void print_reservation(reservation_t * reservation);
void print_reservations(reservation_t * reservations, int quantity);
void print_DAV(int dim[2], reservation_t * reservations, int reservations_quantity);

void get_flight_number(char * flight_number);
int get_int(char * msg, char * error_msg);
void get_seat(int * row, int * col);


#endif
