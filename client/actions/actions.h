#ifndef ACTIONS_H
#define ACTIONS_H

int get_flight_state(int client_socket, char * flight_number);
int book_seat(int client_socket);
int cancel_seat(int client_socket);
int new_flight(int client_socket);
int remove_flight(int client_socket);
int get_reservations(int client_socket);
int get_cancellations(int client_socket);

#endif
