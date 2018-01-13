#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "constants.h"

int main()
{
  int listening_socket;
  int accepted_socket;
  int address_size;
  struct sockaddr_in address;

  if ((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
   perror("socket");
   return 1;
  }

  bzero(&address, sizeof(address));

  address.sin_family = AF_INET;
  address.sin_port = htons(SERVER_PORT);

  if (INADDR_ANY)
   address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(listening_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
   perror("bind");
   return 2;
  }

  switch (fork()) {
    case -1:
      perror("fork");
      return 3;
      break;
    default:
      close(listening_socket);
      return 0;
      break;
    case 0:
      break;
  }

  listen(listening_socket, CLIENT_QUEUE_SIZE);

  while(1) {
    address_size = sizeof(address);

    if((accepted_socket = accept(listening_socket, (struct sockaddr *)&address, &accepted_socket)) < 0) {
      perror("accept");
      return 1;
    }
    new_server(accepted_socket);
  }
}

void new_server(int accepted_socket)
{
  switch (fork()) {
    case -1:
      perror("fork");
      return;
      break;
    default:
      //sigue listening
      close(accepted_socket);
      return;
      break;
    case 0:
      //sirvo al cliente
      close(listening_socket);

  }
}

int recieve_from_client(int recieving_socket)
{

}
