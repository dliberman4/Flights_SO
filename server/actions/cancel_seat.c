#include <stdlib.h>

#include "actions.h"
#include "../database/database.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../log/log.h"
#include "../constants.h"
#include "../utils/utils.h"

int cancel_seat(int accepted_socket, msg_t msg)
{
  int code;
  int bytes;
  reservation_t reservation;

  deserialize_reservation(msg.buffer, &reservation);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    print_error_msg("Al abrir la db");
    return ERROR;
  }

  wait_semaphore(DATABASE_SEM);
  code = db_cancel_seat(&reservation);
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
    print_error_msg("Al cerrar la db\n");
  }
  return 0;
}
