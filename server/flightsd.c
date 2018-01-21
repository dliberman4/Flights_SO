#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "types.h"
#include "database/database.h"

#define ERROR_SERVER -1
int open_socket();
void bind_to_port(int listener_socket, int port);
void sigchld_handler(int sig);
void new_flight(int accepted_socket);
void get_flight_state_server(char * flight_number, int accepted_socket);

int open_socket()
{
  int s;

  if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return ERROR_SERVER;
  }
  return s;
}

void bind_to_port(int listener_socket, int port)
{
  char * reuse;
  struct sockaddr_in address;

  bzero(&address, sizeof(address));

  address.sin_family = PF_INET;
  address.sin_port = htons(port);

  if(INADDR_ANY)
    address.sin_addr.s_addr = htonl(INADDR_ANY);

  /*evitar problemas de reutilizacion del puerto*/
  reuse = (char *) 1;
  setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, (char *) reuse, sizeof(int));

  if(bind(listener_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
    printf("error en bind\n");
    return;
  }
  printf("bind OK!\n");
}

int main()
{
  int listener_socket;
  int accepted_socket;
  int address_size;
  int bytes;
  struct sockaddr_storage client;
  char buffer[MAX_BUF_SIZE];

  signal(SIGCHLD, sigchld_handler);

  listener_socket = open_socket();
  if(listener_socket == ERROR_SERVER) {
    printf("error al crear el socket\n");
    return 1;
  }
  printf("listener_socket creado\n");

  bind_to_port(listener_socket, SERVER_PORT);

  /*MAKE THE SERVER DAEMON*/
  // switch (fork()) {
  //   case -1:
  //           printf("error en el primer fork\n");
  //           return 1;
  //           break;
  //   default:
  //           printf("soy el padre, me voy para que el hijo sea daemon.\n");
  //           close(listener_socket);
  //           return 0;
  //           break;
  //   case 0:
  //           printf("soy el hijo, comienzo a escuchar\n");
  //           break;
  // }

  if(listen(listener_socket, 10) == -1) //10 es la maxima cantidad que puede atenderse
    printf("Se alcanzó el máximo de clientes. Pruebe más tarde\n");

  while(1) {
    address_size = sizeof(client);
    printf("P: esperando al cliente...\n");
    accepted_socket = accept(listener_socket, (struct sockaddr *) &client, &address_size);
    printf("acepte!\n");
    if(accepted_socket == -1) {
      printf("P: error al aceptar\n");
      return 1;
    }
    switch(fork()) {
      case 0:
              close(listener_socket);
              printf("H: soy el hijo\nH: atendiendo al cliente\n");

              if((bytes = read(accepted_socket, buffer, MAX_BUF_SIZE)) > 0) {
                switch((int) strtol(buffer, (char**) NULL, 10)) {
                  case GET_FLIGHT_STATE:
                      if((bytes = read(accepted_socket, buffer, MAX_BUF_SIZE)) < 0) {
                        printf("error al leer del socket\n");
                      }
                      get_flight_state_server(buffer, accepted_socket);
                      break;
                  case BOOK_SEAT:

                      break;
                  case CANCEL_SEAT:

                      break;
                  case NEW_FLIGHT:

                      break;
                  default:
                      break;
                }
              }

              if(write(accepted_socket, "Ya me dormi. Adios!\n", 20) < 0) {
                printf("H: error al escribir en el socket\n");
              }
              close(accepted_socket);
              printf("H: mi trabajo termino. Adios!\n");
              exit(0);
              break;
      case -1:
              printf("error en el fork\n");
              close(accepted_socket);
              break;
      default:
              printf("P: soy el padre, sigo\n");
              close(accepted_socket);
              break;
    }
  }
  return 0;
}

int seat_in_reservations(char * flight_number, reservation_t * reservations, int fil, int col)
{
  int i = 0;

  while(i < fil*col) {
    if((reservations[i]).seat_row == fil && (reservations[i]).seat_col == col && (reservations[i]).flight_number == flight_number) {
      return 1;
    }
    i++;
  }
  return 0;
}

void get_flight_state_server(char * flight_number, int accepted_socket)
{
  int dim[2];
  int reservations_quantity;
  int i, j;
  reservation_t * reservations;
  char * state;
  int code;

  code = db_open();
  printf("%d\n", code);

  code = db_get_flight_dim(flight_number, dim);
  printf("%d\n", code);
  printf("el vuelo %s tiene filas: %d y cols: %d\n", flight_number, dim[0], dim[1]);
  reservations_quantity = db_get_reservations_quantity(flight_number);
  printf("la cantidad de reservas es: %d\n", reservations_quantity);
  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  if(db_get_reservations(flight_number, reservations));

  state = malloc(sizeof(char) * dim[0] * (dim[1] + 1));

  for(i = 0; i < dim[0]; i++) {
    for(j = 0; j < dim[1]; j++) {
      state[(dim[1]+1)*i + j] = 'D';
    }
    state[(dim[1]+1)*i + j] = '\n';
  }

  for(i = 0; i < reservations_quantity; i++) {
    state[(dim[1]+1)* (reservations[i].seat_row) + (reservations[i].seat_col)] = 'R';
  }

  printf("%s\n", state);

  if(write(accepted_socket, state, sizeof(char) * dim[0] * (dim[1] + 1)) < 0) {
    printf("error al escribir en el socket aquiiiii\n");
  }

  free(reservations);
  free(state);
  db_close(0);
}

void sigchld_handler(int sig)
{
   while (waitpid(-1, 0, WNOHANG) > 0)
  ;
   return;
}
