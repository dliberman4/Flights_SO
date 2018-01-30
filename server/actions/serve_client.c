#include <stdlib.h>
#include <unistd.h>

#include "../../protocol/messages.h"
#include "actions.h"
#include "../log/log.h"
#include "../constants.h"

void serve_client(int accepted_socket)
{
  int bytes;
  msg_t msg;
  
  while(1) {
    bytes = receive_msg(accepted_socket, &msg);
    if(bytes < 0) {
      print_error_msg("Al leer del socket");
      exit(1);
    }
    if(bytes == 0) {
      print_ok_msg("Cliente desconectado. Fin del proceso.");
      close(accepted_socket);
      exit(0);
    }

    switch(msg.type) {
      case GET_FLIGHT_STATE:
          get_flight_state(accepted_socket, msg);
          break;
      case BOOK_SEAT:
          book_seat(accepted_socket, msg);
          break;
      case CANCEL_SEAT:
          cancel_seat(accepted_socket, msg);
          break;
      case NEW_FLIGHT:
          new_flight(accepted_socket, msg);
          break;
      case REMOVE_FLIGHT:
          remove_flight(accepted_socket, msg);
          break;
      case GET_CANCELLATIONS:
          get_cancellations(accepted_socket, msg);
          break;
      case GET_RESERVATIONS:
          get_reservations(accepted_socket, msg);
          break;
    }
  }
}
