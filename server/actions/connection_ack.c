#include "../../protocol/messages.h"
#include "actions.h"
#include "../log/log.h"

int connection_ack(int accepted_socket, msg_t msg)
{
  int bytes;

  msg.type = RESPONSE_OK;
  msg.bytes = 0;
  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    print_error_msg("Al escribir en el socket");
  }

  return 0;
}
