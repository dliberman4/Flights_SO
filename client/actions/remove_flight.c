#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int remove_flight(int client_socket)
{
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  msg_t msg;
  char flight_number[MAX_FLIGHT_NUMBER];

  get_flight_number(flight_number);

  end_of_buffer = serialize_string(buffer, flight_number);

  msg.type = REMOVE_FLIGHT;
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
    print_error_msg("Acción no realizada. Datos inválidos.");
  }
  else if(msg.type == RESPONSE_OK) {
    print_ok_msg("Vuelo eliminado con éxito!");
  }
  return 0;
}
