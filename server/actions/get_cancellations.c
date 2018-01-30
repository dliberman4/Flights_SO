#include <stdlib.h>

#include "actions.h"
#include "../database/database.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../log/log.h"
#include "../constants.h"

int get_cancellations(int accepted_socket, msg_t msg)
{
  int cancellations_quantity;
  reservation_t * cancellations;
  int code;
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;

  code = db_open();
  if(code < 0) {
    print_error_msg("Al abrir la db");
    return ERROR;
  }

  cancellations_quantity = db_get_cancellations_quantity(NULL);

  cancellations = (reservation_t *) malloc(sizeof(reservation_t) * cancellations_quantity);

  if(db_get_cancellations(NULL, cancellations) < 0) {
    print_error_msg("Al obtener las reservas");
    free(cancellations);
    return ERROR;
  }

  end_of_buffer = serialize_int(buffer, cancellations_quantity);
  end_of_buffer = serialize_reservation_array(end_of_buffer, cancellations, cancellations_quantity);

  msg.type = GET_CANCELLATIONS;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    print_error_msg("Al mandar msg");
  }

  free(cancellations);

  code = db_close(1);
  if(code < 0) {
    print_error_msg("Al cerrar la db");
  }

  return (code < 0 || bytes < 0) * -1;
}
