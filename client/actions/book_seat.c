#include <stdlib.h>

#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int book_seat(int client_socket)
{
  int bytes;
  int code;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  reservation_t reservation;
  msg_t msg;

  get_flight_number(reservation.flight_number);

  code = get_flight_state(client_socket, reservation.flight_number);

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
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes <= 0) {
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
  }

  free(msg.buffer);

  if(msg.type == RESPONSE_ERROR) {
    print_error_msg("El asiento ingresado no pudo ser reservado.");
  }
  else if(msg.type == RESPONSE_OK) {
    print_ok_msg("Reserva realizada con éxito!");
  }

  code = get_flight_state(client_socket, reservation.flight_number);
  if(code < 0)
    return ERROR;

  return 0;
}
