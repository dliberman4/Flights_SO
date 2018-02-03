#include <stdlib.h>

#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int get_flight_state(int client_socket, char * flight_number)
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
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes <= 0) {
    print_error_msg("Conexión perdida");
    return FATAL_ERROR;
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
