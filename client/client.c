#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include "constants.h"
#include "../protocol/messages.h"
#include "../protocol/serializer.h"
#include "frontend/frontend.h"


int client_socket_initialize(const char * ip);

int get_flight_state_client(int client_socket, char * flight_number);
int book_seat_client(int client_socket);
int cancel_seat_client(int client_socket);
int new_flight_client(int client_socket);
int remove_flight_client(int client_socket);
int get_reservations_client(int client_socket);
int get_cancellations_client(int client_socket);

int client_socket_initialize(const char * ip)
{
  int client_socket;
  struct sockaddr_in address;

  if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return ERROR;
  }

  address.sin_family = AF_INET;
  address.sin_port = htons(SERVER_PORT);
  address.sin_addr.s_addr = inet_addr(ip);

  if(connect(client_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
    print_error_msg("error al conectar");
    close(client_socket);
    return ERROR;
  }
  return client_socket;
}

/*
primer argumento: ip
*/
int main(int argc, char * argv[])
{
    int client_socket;
    int choice;
    int code;
    int should_close;

    client_socket = client_socket_initialize(argv[1]);
    if(client_socket == ERROR) {
      print_error_msg("error al crear el socket");
      return 1;
    }
    should_close = 0;

    while(!should_close) {
      choice = show_menu();

      switch(choice) {
        case GET_FLIGHT_STATE:
          code = get_flight_state_client(client_socket, NULL);
          break;
        case BOOK_SEAT:
          code = book_seat_client(client_socket);
          break;
        case CANCEL_SEAT:
          code = cancel_seat_client(client_socket);
          break;
        case NEW_FLIGHT:
          code = new_flight_client(client_socket);
          break;
        case REMOVE_FLIGHT:
          code = remove_flight_client(client_socket);
          break;
        case GET_RESERVATIONS:
          code = get_reservations_client(client_socket);
          break;
        case GET_CANCELLATIONS:
          code = get_cancellations_client(client_socket);
          break;
        case CLOSE:
          should_close = 1;
          break;
      }

    }

    close(client_socket);
    return code;
}

int get_flight_state_client(int client_socket, char * flight_number)
{
  int bytes;
  int should_free = 0;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  msg_t msg;
  flight_t flight;
  reservation_t * reservations;
  int reservations_quantity;


  if(flight_number == NULL) {
    flight_number = (char *)malloc(MAX_FLIGHT_NUMBER);
    get_flight_number(flight_number);
    should_free = 1;
  }

  end_of_buffer = serialize_string(buffer, flight_number);
  if(should_free) {
    free(flight_number);
  }

  msg.type = GET_FLIGHT_STATE;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    print_error_msg("Acción no realizada. Inténtelo mas tarde.");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    print_error_msg("error al leer del socket");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    print_error_msg("Vuelo no encontrado");
    return ERROR;
  }

  end_of_buffer = deserialize_flight(msg.buffer, &flight);
  end_of_buffer = deserialize_int(end_of_buffer, &reservations_quantity);
  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  end_of_buffer = deserialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  free(msg.buffer);

  print_DAV(flight.dim, reservations, reservations_quantity);

  free(reservations);

  return 0;
}

int book_seat_client(int client_socket)
{
  int bytes;
  int code;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  reservation_t reservation;
  msg_t msg;

  get_flight_number(reservation.flight_number);

  code = get_flight_state_client(client_socket, reservation.flight_number);

  if(code == ERROR)
    return ERROR;

  get_seat(&reservation.seat_row, &reservation.seat_col);

  reservation.dni = get_int("Ingrese su DNI", "DNI inválido. Ingréselo nuevamente");

  end_of_buffer = serialize_reservation(buffer, reservation);

  msg.type = BOOK_SEAT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    print_error_msg("Acción no realizada. Intentelo mas tarde.");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    print_error_msg("Leyendo del socket");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    print_error_msg("El asiento ingresado no pudo ser reservado.");
  }
  else if(msg.type == RESPONSE_OK) {
    print_ok_msg("Reserva realizada con éxito!");
  }

  code = get_flight_state_client(client_socket, reservation.flight_number);
  if(code < 0)
    return ERROR;

  return 0;
}

int cancel_seat_client(int client_socket)
{
  int bytes;
  int code;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  reservation_t cancellation;
  msg_t msg;

  get_flight_number(cancellation.flight_number);

  code = get_flight_state_client(client_socket, cancellation.flight_number);

  if(code == ERROR)
    return ERROR;

  get_seat(&cancellation.seat_row, &cancellation.seat_col);

  cancellation.dni = get_int("Ingrese su DNI", "DNI inválido. Ingréselo nuevamente");

  end_of_buffer = serialize_reservation(buffer, cancellation);

  msg.type = CANCEL_SEAT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    print_error_msg("Acción no realizada. Intentelo mas tarde.");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    print_error_msg("Leyendo del socket");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    print_error_msg("Acción no realizada. Datos inválidos.");
  }
  else if(msg.type == RESPONSE_OK) {
    print_ok_msg("Reserva cancelada con éxito!");
  }

  get_flight_state_client(client_socket, cancellation.flight_number);
  return 0;
}

int new_flight_client(int client_socket)
{
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  flight_t flight;
  msg_t msg;

  get_flight_number(flight.flight_number);

  flight.dim[0] = get_int("Ingrese la cantidad de filas del avión",
  "Cantidad de filas inválidas. Ingrésela nuevamente");

  flight.dim[1] = get_int("Ingrese la cantidad de columnas del avión",
  "Cantidad de columnas inválidas. Ingrésela nuevamente");

  end_of_buffer = serialize_flight(buffer, flight);

  msg.type = NEW_FLIGHT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    print_error_msg("Acción no realizada. Intentelo mas tarde.");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    print_error_msg("Leyendo del socket");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    print_error_msg("Acción no realizada. Datos inválidos.");
  }
  else if(msg.type == RESPONSE_OK) {
    print_ok_msg("Vuelo creado con éxito!");
    print_flight(&flight);
  }

  return 0;
}

int remove_flight_client(int client_socket)
{
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  msg_t msg;
  char flight_number[MAX_FLIGHT_NUMBER];

  get_flight_number(flight_number);

  end_of_buffer = serialize_string(buffer, flight_number);

  msg.type = REMOVE_FLIGHT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    print_error_msg("Acción no realizada. Intentelo mas tarde.");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    print_error_msg("Leyendo del socket");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    print_error_msg("Acción no realizada. Datos inválidos.");
  }
  else if(msg.type == RESPONSE_OK) {
    print_ok_msg("Vuelo eliminado con éxito!");
  }
  return 0;
}

int get_reservations_client(int client_socket)
{
  msg_t msg;
  int reservations_quantity;
  reservation_t * reservations;
  unsigned char * end_of_buffer;
  int bytes;

  msg.type = GET_RESERVATIONS;
  msg.bytes = 0;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    print_error_msg("Escribiendo en el socket");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    print_error_msg("Leyendo del socket");
    return ERROR;
  }

  end_of_buffer = deserialize_int(msg.buffer, &reservations_quantity);
  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  end_of_buffer = deserialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  free(msg.buffer);

  print_reservations(reservations, reservations_quantity);

  free(reservations);
  return 0;
}

int get_cancellations_client(int client_socket)
{
  msg_t msg;
  int cancellations_quantity;
  reservation_t * cancellations;
  unsigned char * end_of_buffer;
  int bytes;

  msg.type = GET_CANCELLATIONS;
  msg.bytes = 0;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    print_error_msg("Escribiendo en el socket");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    print_error_msg("Leyendo del socket");
    return ERROR;
  }

  end_of_buffer = deserialize_int(msg.buffer, &cancellations_quantity);
  cancellations = (reservation_t *) malloc(sizeof(reservation_t) * cancellations_quantity);

  end_of_buffer = deserialize_reservation_array(end_of_buffer, cancellations, cancellations_quantity);

  free(msg.buffer);

  print_reservations(cancellations, cancellations_quantity);

  free(cancellations);

  return 0;
}
