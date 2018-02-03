#include <stdlib.h>

#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int get_cancellations(int client_socket)
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
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes <= 0) {
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
  }

  end_of_buffer = deserialize_int(msg.buffer, &cancellations_quantity);
  cancellations = (reservation_t *) malloc(sizeof(reservation_t) * cancellations_quantity);

  end_of_buffer = deserialize_reservation_array(end_of_buffer, cancellations, cancellations_quantity);

  free(msg.buffer);

  print_reservations(cancellations, cancellations_quantity);

  free(cancellations);

  return 0;
}
