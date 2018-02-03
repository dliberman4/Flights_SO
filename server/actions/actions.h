#ifndef ACTIONS_H
#define ACTIONS_H

#include "../../protocol/types.h"

int get_flight_state(int accepted_socket, msg_t msg);
int book_seat(int accepted_socket, msg_t msg);
int cancel_seat(int accepted_socket, msg_t msg);
int new_flight(int accepted_socket, msg_t msg);
int remove_flight(int accepted_socket, msg_t msg);
int get_reservations(int accepted_socket, msg_t msg);
int get_cancellations(int accepted_socket, msg_t msg);
int connection_ack(int accepted_socket, msg_t msg);
int get_flights(int accepted_socket, msg_t msg);

int serve_client(int accepted_socket);

#endif
