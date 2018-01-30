#include <stdlib.h>

#include "actions.h"
#include "../database/database.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../log/log.h"
#include "../constants.h"

int get_flight_state(int accepted_socket, msg_t msg)
{
  int reservations_quantity;
  reservation_t * reservations;
  int code;
  int bytes;
  flight_t flight;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;

  deserialize_string(msg.buffer, flight.flight_number);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    print_error_msg("Al abrir la db");
    return ERROR;
  }
  code = db_get_flight_dim(flight.flight_number, flight.dim);
  if(code < 0) {
    print_error_msg("Al pedir las dimensiones");
    msg.type = RESPONSE_ERROR;
    msg.bytes = 0;
    bytes = send_msg(accepted_socket, msg);
    return ERROR;
  }

  reservations_quantity = db_get_reservations_quantity(flight.flight_number);

  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  if(db_get_reservations(flight.flight_number, reservations) < 0) {
    print_error_msg("Al obtener las reservas");
    free(reservations);
    return ERROR;
  }

  end_of_buffer = serialize_flight(buffer, flight);
  end_of_buffer = serialize_int(end_of_buffer, reservations_quantity);
  end_of_buffer = serialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  msg.type = GET_FLIGHT_STATE;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    print_error_msg("Al mandar msg");
  }

  free(reservations);

  code = db_close(1);
  if(code < 0) {
    print_error_msg("Al cerrar la db\n");
  }

  return (code < 0 || bytes < 0) * -1;
}
