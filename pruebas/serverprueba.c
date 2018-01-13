#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

int main()
{
  int listener_socket;
  int accepted_socket;
  int address_size;
  int reuse;
  struct sockaddr_in address;

  if((listener_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    printf("error al crear el socket\n");
  }

  printf("listener_socket creado\n");

  address.sin_family = PF_INET;
  address.sin_port = htons(2000);

  if(INADDR_ANY)
    address.sin_addr.s_addr = htonl(INADDR_ANY);

  /*evitar problemas de reutilizacion del puerto*/
  reuse = 1;
  setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, (char *) reuse, sizeof(int));

  if(bind(listener_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
    printf("error en bind\n");
  }
  printf("bind OK!\n");

  if(listen(listener_socket, 10) == -1) //10 es la maxima cantidad que puede atenderse
    printf("Se alcanzó el máximo de clientes. Pruebe más tarde\n");

  while(1) {
    struct sockaddr_storage client;
    address_size = sizeof(client);
    printf("esperando al cliente...\n");
    accepted_socket = accept(listener_socket, (struct sockaddr *) &client, &address_size);
    if(accepted_socket == -1) {
      printf("error al aceptar\n");
    }
    printf("atendiendo al cliente\n");
    send(accepted_socket, "hola como estas?\n", 17, 0);
    close(accepted_socket);
  }
  return 0;
}
