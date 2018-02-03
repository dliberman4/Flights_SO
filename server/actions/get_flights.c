#include <stdlib.h>

#include "actions.h"
#include "../database/database.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../log/log.h"
#include "../constants.h"

int get_flights(int accepted_socket, msg_t msg)
{
  int flights_quantity;
  flight_t * flights;
  int code;
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;

  code = db_open();
  if(code < 0) {
    print_error_msg("Al abrir la db");
    return ERROR;
  }

  flights_quantity = db_get_flights_quantity();

  flights = (flight_t *) malloc(sizeof(flight_t) * flights_quantity);

  if(db_get_flights(flights) < 0) {
    print_error_msg("Al obtener los vuelos");
    free(flights);
    return ERROR;
  }

  end_of_buffer = serialize_int(buffer, flights_quantity);
  end_of_buffer = serialize_flight_array(end_of_buffer, flights, flights_quantity);

  msg.type = GET_FLIGHTS;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    print_error_msg("Al mandar msg");
  }

  free(flights);

  code = db_close(1);
  if(code < 0) {
    print_error_msg("Al cerrar la db");
  }

  return (code < 0 || bytes < 0) * -1;
}
