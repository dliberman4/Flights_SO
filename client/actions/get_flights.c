#include <stdlib.h>

#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int get_flights(int client_socket)
{
  msg_t msg;
  int flights_quantity;
  flight_t * flights;
  unsigned char * end_of_buffer;
  int bytes;

  msg.type = GET_FLIGHTS;
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

  end_of_buffer = deserialize_int(msg.buffer, &flights_quantity);
  flights = (flight_t *) malloc(sizeof(flight_t) * flights_quantity);

  end_of_buffer = deserialize_flight_array(end_of_buffer, flights, flights_quantity);

  free(msg.buffer);

  print_flights(flights, flights_quantity);

  free(flights);
  return 0;
}
