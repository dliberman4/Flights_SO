#include <stdlib.h>
#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int get_reservations(int client_socket)
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
