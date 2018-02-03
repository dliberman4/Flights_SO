#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int cancel_seat(int client_socket)
{
  int bytes;
  int code;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  reservation_t cancellation;
  msg_t msg;

  get_flight_number(cancellation.flight_number);

  code = get_flight_state(client_socket, cancellation.flight_number);

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
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes <= 0) {
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    print_error_msg("Acción no realizada. Datos inválidos.");
  }
  else if(msg.type == RESPONSE_OK) {
    print_ok_msg("Reserva cancelada con éxito!");
  }

  get_flight_state(client_socket, cancellation.flight_number);
  return 0;
}
