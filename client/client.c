#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../protocol/protocol_constants.h"
#include "constants.h"
#include "frontend/frontend.h"
#include "actions/actions.h"

int client_socket_initialize(const char * ip);

/* primer argumento: ip */
int main(int argc, char * argv[])
{
    int client_socket;
    int choice;
    int code;
    int should_close;

    if(argc != 2) {
      print_error_msg("Argumentos inválidos. Debe ingresar sólo la IP.");
      return 1;
    }
    client_socket = client_socket_initialize(argv[1]);
    if(client_socket == ERROR) {
      print_error_msg("Conexión perdida");
      return 1;
    }
    should_close = 0;
    show_title();

    print_ok_msg("Conectando...");
    if(check_connection(client_socket) < 0) {
      return 1;
    }

    while(!should_close) {
      choice = show_menu();

      switch(choice) {
        case GET_FLIGHT_STATE:
          code = get_flight_state(client_socket, NULL);
          break;
        case BOOK_SEAT:
          code = book_seat(client_socket);
          break;
        case CANCEL_SEAT:
          code = cancel_seat(client_socket);
          break;
        case NEW_FLIGHT:
          code = new_flight(client_socket);
          break;
        case REMOVE_FLIGHT:
          code = remove_flight(client_socket);
          break;
        case GET_RESERVATIONS:
          code = get_reservations(client_socket);
          break;
        case GET_CANCELLATIONS:
          code = get_cancellations(client_socket);
          break;
        case GET_FLIGHTS:
          code = get_flights(client_socket);
          break;
        case CLOSE:
          should_close = 1;
          break;
      }
      if(code == FATAL_ERROR) {
        should_close = 1;
      }
    }

    close(client_socket);
    return code;
}

int client_socket_initialize(const char * ip)
{
  int client_socket;
  struct sockaddr_in address;

  if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return ERROR;
  }

  address.sin_family = AF_INET;
  address.sin_port = htons(SERVER_PORT);
  address.sin_addr.s_addr = inet_addr(ip);

  if(connect(client_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
    print_error_msg("error al conectar");
    close(client_socket);
    return ERROR;
  }
  return client_socket;
}
