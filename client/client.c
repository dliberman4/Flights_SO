#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "constants.h"

#define MAX_BUF_SIZE 256
#define SERVER_PORT 1049
#define ERROR_SOCKET -1

int client_socket_initialize();
void get_flight_state_client(int client_socket);
void book_seat_client(int client_socket);
void cancel_seat_client(int client_socket);
void new_flight_client(int client_socket);

int client_socket_initialize()
{
  int client_socket;
  struct sockaddr_in address;

  if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return ERROR_SOCKET;
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
  return client_socket;
}

void get_flight_state_client(int client_socket)
{
  int bytes;
  char buffer[MAX_BUF_SIZE];

  printf("ingrese el numero de vuelo que desea consultar\n");
  bzero(buffer, 256);
  scanf("%s", buffer);
  //fgets(buffer, 255, stdin);

  /*mando el numero de vuelo*/
  bytes = write(client_socket, buffer, strlen(buffer)+1);
  if(bytes < 0) {
    printf("C: error al escribir en el socket\n");
  }

  if((bytes = read(client_socket, buffer, MAX_BUF_SIZE)) > 0) {
    write(1, buffer, bytes);
  }
}

void book_seat_client(int client_socket)
{
  int bytes;
  char buffer[MAX_BUF_SIZE];


}
void cancel_seat_client(int client_socket)
{
  int bytes;
  char buffer[MAX_BUF_SIZE];


}
void new_flight_client(int client_socket)
{
  int bytes;
  char buffer[MAX_BUF_SIZE];

}

/*
primer argumento: puerto
segundo argumento: ip
*/
int main(int argc, char * argv[])
{
    int client_socket;
    int bytes;
    char buffer[MAX_BUF_SIZE];

    client_socket = client_socket_initialize();
    if(client_socket == ERROR_SOCKET) {
      printf("error al crear el socket\n");
      return 1;
    }

    printf("Introduzca el numero de operacion:\n1: Obtener el estado de vuelo.\n2: Reservar asiento.\n3: Cancelar reserva de asiento.\n4: Crear nuevo vuelo\n");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    /*mando la opcion elegida*/
    bytes = write(client_socket, buffer, strlen(buffer)+1);
    if(bytes < 0) {
      printf("C: error al escribir en el socket\n");
    }

    switch((int) strtol(buffer, (char**) NULL, 10)) {
      case GET_FLIGHT_STATE:
        get_flight_state_client(client_socket);
        break;
      case BOOK_SEAT:
        book_seat_client(client_socket);
        break;
      case CANCEL_SEAT:
        cancel_seat_client(client_socket);
        break;
      case NEW_FLIGHT:
        new_flight_client(client_socket);
        break;

    }

    close(client_socket);
    return 0;
}
