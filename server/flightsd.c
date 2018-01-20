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

#define ERROR_SERVER -1
int open_socket();
void bind_to_port(int listener_socket, int port);
void sigchld_handler(int sig);

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

              bytes = write(accepted_socket, "Introduzca el numero de operacion:\n1: Obtener el estado de vuelo.\n2: Reservar asiento.\n3: Cancelar reserva de asiento.\n", 119);

              if(bytes < 0) {
                printf("H: error al escribir en el socket\n");
              }

              if((bytes = read(accepted_socket, buffer, MAX_BUF_SIZE)) > 0) {
                switch((int) strtol(buffer, (char**) NULL, 10)) {
                  case GET_FLIGHT_STATE:
                      printf("eligió opcion 1\n");
                      break;
                  case BOOK_SEAT:
                      printf("eligió opcion 2\n");
                      break;
                  case CANCEL_SEAT:
                      printf("eligió opcion 3\n");
                      break;
                }
              }

              sleep(10);
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


void sigchld_handler(int sig)
{
   while (waitpid(-1, 0, WNOHANG) > 0)
  ;
   return;
}
