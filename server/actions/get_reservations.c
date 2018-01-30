#include <stdlib.h>

#include "actions.h"
#include "../database/database.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../log/log.h"
#include "../constants.h"

int get_reservations(int accepted_socket, msg_t msg)
{
  int reservations_quantity;
  reservation_t * reservations;
  int code;
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;

  code = db_open();
  if(code < 0) {
    print_error_msg("Al abrir la db");
    return ERROR;
  }

  reservations_quantity = db_get_reservations_quantity(NULL);

  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  if(db_get_reservations(NULL, reservations) < 0) {
    print_error_msg("Al obtener las reservas");
    free(reservations);
    return ERROR;
  }

  end_of_buffer = serialize_int(buffer, reservations_quantity);
  end_of_buffer = serialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  msg.type = GET_RESERVATIONS;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    print_error_msg("Al mandar msg");
  }

  free(reservations);

  code = db_close(1);
  if(code < 0) {
    print_error_msg("Al cerrar la db");
  }

  return (code < 0 || bytes < 0) * -1;
}
