#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void sigchld_handler(int sig);

int main()
{
  int listener_socket;
  int accepted_socket;
  int address_size;
  char * reuse;
  struct sockaddr_in address;
  struct sockaddr_storage client;

  signal(SIGCHLD, sigchld_handler);

  if((listener_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    printf("error al crear el socket\n");
  }

  printf("listener_socket creado\n");

  address.sin_family = PF_INET;
  address.sin_port = htons(2000);

  if(INADDR_ANY)
    address.sin_addr.s_addr = htonl(INADDR_ANY);

  /*evitar problemas de reutilizacion del puerto*/
  reuse = (char *) 1;
  setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, (char *) reuse, sizeof(int));

  if(bind(listener_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
    printf("error en bind\n");
  }
  printf("bind OK!\n");

  if(listen(listener_socket, 10) == -1) //10 es la maxima cantidad que puede atenderse
    printf("Se alcanzó el máximo de clientes. Pruebe más tarde\n");

  while(1) {
    address_size = sizeof(client);
    printf("esperando al cliente...\n");
    accepted_socket = accept(listener_socket, (struct sockaddr *) &client, &address_size);
    if(accepted_socket == -1) {
      printf("error al aceptar\n");
    }
    switch(fork()) {
      case 0:
              close(listener_socket);
              printf("soy el hijo\natendiendo al cliente\n");
              send(accepted_socket, "Hola como estas? Me duermo...\n", 17, 0);
              sleep(15);
              send(accepted_socket, "Ya me dormi. Adios!\n", 17, 0);
              close(accepted_socket);
              exit(0);
              break;
      case -1:
              printf("error en el fork\n");
              close(accepted_socket);
              break;
      default:
              printf("soy el padre, sigo\n");
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
