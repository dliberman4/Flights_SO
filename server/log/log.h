#ifndef FRONTEND_H
#define FRONTEND_H

#include "../../protocol/types.h"
int show_menu();
void show_title();

void print_error_msg(char * msg);
void print_ok_msg(char * msg);

void get_flight_number(char * flight_number);
int get_int(char * msg, char * error_msg);
void get_seat(int * row, int * col);
void colors_off();

#endif
