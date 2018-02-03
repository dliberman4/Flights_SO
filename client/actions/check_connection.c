#include "actions.h"
#include "../../protocol/messages.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int check_connection(int client_socket)
{
  msg_t msg;
  int bytes;

  msg.type = CHECK_CONNECTION;
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
  if(msg.type == RESPONSE_OK) {
    return 0;
  }
  return ERROR;
}
