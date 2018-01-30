#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/signal.h>

#include "constants.h"
#include "../protocol/messages.h"
#include "log/log.h"
#include "actions/actions.h"
#include "setup/setup.h"

int main(int argc, char * argv[])
{
  int listener_socket;
  int accepted_socket;
  int bytes;
  msg_t msg;

  /* wait for finished child processes */
  signal(SIGCHLD, sigchld_handler);

  listener_socket = init_socket();
  if(listener_socket == ERROR) {
    return 1;
  }

  /* make the server daemon if required */
  if(argc == 2 && (strcmp("-d", argv[1]) == 0)){
    become_daemon();
  }

  while(1) {
    accepted_socket = accept_client(listener_socket);
    if(accepted_socket < 0) {
      return 1;
    }
    switch(fork()) {
      case 0:
              close(listener_socket);
              while(1) {
                bytes = receive_msg(accepted_socket, &msg);
                if(bytes < 0) {
                  print_error_msg("Al leer del socket");
                  return ERROR;
                }
                if(bytes <= 0) {
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
              exit(0);
              break;
      case -1:
              print_error_msg("En el fork del servidor");
              close(accepted_socket);
              break;
      default:
              close(accepted_socket);
              break;
    }
  }
  return 0;
}
