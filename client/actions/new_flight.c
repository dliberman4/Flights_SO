#include "actions.h"
#include "../../protocol/messages.h"
#include "../../protocol/serializer.h"
#include "../frontend/frontend.h"
#include "../constants.h"

int new_flight(int client_socket)
{
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  flight_t flight;
  msg_t msg;

  get_flight_number(flight.flight_number);

  flight.dim[0] = get_int("Ingrese la cantidad de filas del avión",
  "Cantidad de filas inválidas. Ingrésela nuevamente");

  flight.dim[1] = get_int("Ingrese la cantidad de columnas del avión",
  "Cantidad de columnas inválidas. Ingrésela nuevamente");

  end_of_buffer = serialize_flight(buffer, flight);

  msg.type = NEW_FLIGHT;
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
    print_ok_msg("Vuelo creado con éxito!");
    print_flight(&flight);
  }

  return 0;
}
