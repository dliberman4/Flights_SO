#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF_SIZE 256
#define SERVER_PORT 1049

/*
primer argumento: puerto
segundo argumento: ip
*/
int main(int argc, char * argv[])
{
    int client_socket;
    int bytes;
    struct sockaddr_in address;
    char buffer[MAX_BUF_SIZE];

    if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      printf("error al crear el socket\n");
      return 1;
    }

    printf("client_socket creado\n");

    address.sin_family = AF_INET;
    address.sin_port = htons(SERVER_PORT);
    address.sin_addr.s_addr = inet_addr((char *) "127.0.0.1");

    if(connect(client_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
      printf("error al conectar\n");
      close(client_socket);
      return 1;
    }

    if((bytes = read(client_socket, buffer, MAX_BUF_SIZE)) > 0) {
      write(1, buffer, bytes);
    }

    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    bytes = write(client_socket, buffer, MAX_BUF_SIZE);

    if(bytes < 0) {
      printf("C: error al escribir en el socket\n");
    }


/*nuevo*/

    while(1){
      if((bytes = read(client_socket, buffer, MAX_BUF_SIZE)) > 0) {
        write(1, buffer, bytes);
      }

      bzero(buffer, 256);
      fgets(buffer, 255, stdin);

      bytes = write(client_socket, buffer, MAX_BUF_SIZE);

      if(bytes < 0) {
        printf("C: error al escribir en el socket\n");
      }
    }

    close(client_socket);
    return 0;
}
