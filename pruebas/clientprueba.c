#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF_SIZE 256

int main()
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
    address.sin_port = htons(2000);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
      printf("error al conectar\n");
      close(client_socket);
      return 1;
    }

    if((bytes = read(client_socket, buffer, MAX_BUF_SIZE)) > 0) {
      write(1, buffer, bytes);
    }

    if((bytes = read(client_socket, buffer, MAX_BUF_SIZE)) > 0) {
      write(1, buffer, bytes);
    }

    close(client_socket);
    return 0;
}
