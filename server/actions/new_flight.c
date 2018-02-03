#include <stdlib.h>

#include "actions.h"
#include "../database/database.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../log/log.h"
#include "../constants.h"
#include "../utils/utils.h"

int new_flight(int accepted_socket, msg_t msg)
{
  int code;
  int bytes;
  flight_t flight;

  deserialize_flight(msg.buffer, &flight);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    print_error_msg("Al abrir la db");
    return ERROR;
  }
  
  wait_semaphore(DATABASE_SEM);
  code = db_new_flight(flight.flight_number, flight.dim[0], flight.dim[1]);
  post_semaphore(DATABASE_SEM);

  if(code < 0)
    msg.type = RESPONSE_ERROR;
  else
    msg.type = RESPONSE_OK;

  msg.bytes = 0;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    print_error_msg("Al escribir en el socket");
  }

  code = db_close(1);
  if(code < 0) {
    print_error_msg("Al cerrar la db");
  }
  return (code < 0 || bytes < 0) * -1;
}
