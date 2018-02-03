#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/signal.h>

#include "constants.h"
#include "log/log.h"
#include "actions/actions.h"
#include "utils/utils.h"
#include "database/database.h"

int main(int argc, char * argv[])
{
  int listener_socket;
  int accepted_socket;
  int ret;

  /* make the server daemon if required */
  if(argc == 2 && (strcmp("-d", argv[1]) == 0)){
    become_daemon();
  }

  if(db_check_tables() < 0){
    print_error_msg("Tablas no encontradas.");
    return 1;
  }

  /* wait for finished child processes */
  signal(SIGCHLD, sigchld_handler);

  /* when listener closes, release semaphores */
  signal(SIGINT, close_handler);
  signal(SIGQUIT, close_handler);
  signal(SIGTERM, close_handler);

  listener_socket = init_socket();
  if(listener_socket == ERROR) {
    return 1;
  }

  /* sempaphores initialization */
  if(init_semaphores() < 0) {
    print_error_msg("Al inicializar los semáforos");
    close_semaphores();
    return 1;
  }

  while(1) {
    /* limit quantity of childs */
    wait_semaphore(CHILD_SEM);

    accepted_socket = accept_client(listener_socket);
    if(accepted_socket < 0) {
      return 1;
    }
    fflush(0);
    switch(fork()) {
      case 0:
              close(listener_socket);
              ret = serve_client(accepted_socket);
              post_semaphore(CHILD_SEM);
              return ret;
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
