#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/signal.h>

#include "constants.h"
#include "log/log.h"
#include "actions/actions.h"
#include "setup/setup.h"
#include "database/database.h"

int main(int argc, char * argv[])
{
  int listener_socket;
  int accepted_socket;

  if(db_check_tables() < 0){
    print_error_msg("Tablas no encontradas.");
    return 1;
  }

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
              serve_client(accepted_socket);
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
