#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include "constants.h"
#include "types.h"
#include "../protocol/protocol_constants.h"


int client_socket_initialize(const char * ip);
int get_flight_state_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int book_seat_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int cancel_seat_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int new_flight_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int close_connection(int client_socket, int choice);

int client_socket_initialize(const char * ip)
{
  int client_socket;
  struct sockaddr_in address;

  if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return ERROR_SOCKET;
  }

  printf("client_socket creado\n");

  address.sin_family = AF_INET;
  address.sin_port = htons(SERVER_PORT);
  address.sin_addr.s_addr = inet_addr(ip);

  if(connect(client_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
    printf("error al conectar\n");
    close(client_socket);
    return 1;
  }
  return client_socket;
}

int close_connection(int client_socket, int choice) {
  if(write(client_socket, &choice, sizeof(int)) < 0) {
    return ERROR_SOCKET;
  }
  return 0;
}

/*
primer argumento: puerto
segundo argumento: ip
*/
int main(int argc, char * argv[])
{
    int client_socket;
    int choice;
    int code;
    int should_close;
    char buffer[MAX_BUF_SIZE];
    char flight_number[MAX_FLIGHT_NUMBER];

    client_socket = client_socket_initialize(argv[1]);
    if(client_socket == ERROR_SOCKET) {
      printf("error al crear el socket\n");
      return 1;
    }
    should_close = 0;

    while(!should_close) {
      printf("Introduzca el numero de operacion:\n1: Obtener el estado de vuelo.\n"
      "2: Reservar asiento.\n3: Cancelar reserva de asiento.\n4: "
      "Crear nuevo vuelo\n5: Eliminar un vuelo\n6: Salir\n");
      bzero(buffer, MAX_BUF_SIZE);
      scanf("%d", &choice);
      while(choice < 1 || choice > MAX_CHOICE) {
        printf("Esa opción no es correcta. Ingrese otra.\n");
        bzero(buffer, MAX_BUF_SIZE);
        scanf("%d", &choice);
      }

      if(choice != CLOSE) {
        printf("ingrese el numero de vuelo\n");
        bzero(buffer, MAX_BUF_SIZE);
        scanf("%s", buffer);
        while(strlen(buffer) > MAX_FLIGHT_NUMBER) {
          printf("Ese numero no es válido. Ingrese máximo %d caracteres\n", MAX_FLIGHT_NUMBER);
          bzero(buffer, MAX_BUF_SIZE);
          scanf("%s", buffer);
        }
        strcpy(flight_number, buffer);
      }

      switch(choice) {
        case GET_FLIGHT_STATE:
          code = get_flight_state_client(client_socket, choice, flight_number);
          break;
        case BOOK_SEAT:
          code = book_seat_client(client_socket, choice, flight_number);
          break;
        case CANCEL_SEAT:
          code = cancel_seat_client(client_socket, choice, flight_number);
          break;
        case NEW_FLIGHT:
          code = new_flight_client(client_socket, choice, flight_number);
          break;
        case REMOVE_FLIGHT:
          //code = new_flight_client(client_socket, choice, flight_number);
          break;
        case CLOSE:
          code = close_connection(client_socket, choice);
          if(code == 0) {
            should_close = 1;
          }
          break;
      }

      if(code < 0) {
        printf("ended with error %d\n", code);
      }
    }

    close(client_socket);
    return code;
}

int get_flight_state_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  int bytes;
  int i,j;
  flight_t flight;
  reservation_t * reservations;
  int reservations_quantity;
  char * state;

  flight.choice = choice;
  strcpy(flight.flight_number, flight_number);

  bytes = write(client_socket, &flight, sizeof(flight_t));
  if(bytes < 0) {
    printf("Se ha producido un error. Acción no realizada. Intentelo mas tarde.\n");
    return ERROR_SOCKET;
  }

  if((bytes = read(client_socket, flight.dim, sizeof(int[2]))) < 0) {
    printf("error al leer del socket (dim)\n");
    return ERROR_SOCKET;
  }

  printf("tengo %d filas y %d cols\n", flight.dim[0], flight.dim[1]);

  if((bytes = read(client_socket, &reservations_quantity, sizeof(int))) < 0) {
    printf("error al leer del socket (reservations_quantity)\n");
    return ERROR_SOCKET;
  }
  printf("hay %d reservas\n", reservations_quantity);

  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  if((bytes = read(client_socket, reservations, sizeof(reservation_t) * reservations_quantity)) < 0) {
    printf("error al leer del socket (reservations)\n");
    return ERROR_SOCKET;
  }

  state = malloc(sizeof(char) * flight.dim[0] * (flight.dim[1]));

  for(i = 0; i < flight.dim[0]; i++) {
    for(j = 0; j < flight.dim[1]; j++) {
      state[(flight.dim[1])*i + j] = 'D';
    }
  }

  for(i = 0; i < reservations_quantity; i++) {
    printf("dni: %d, fila: %d, col: %d\n", reservations[i].dni, reservations[i].seat_row, reservations[i].seat_col);
    state[(flight.dim[1])* (reservations[i].seat_row) + (reservations[i].seat_col)] = 'R';
  }
  printf("   |");
  for(j = 0; j < flight.dim[1]; j++) {
    putchar(j+'A');
  }
  putchar('\n');
  for(i = 0; i < flight.dim[0]; i++) {
    printf("%3d|", i+1);
    for(j = 0; j < flight.dim[1]; j++) {
      putchar(state[(flight.dim[1])*i + j]);
    }
    putchar('\n');
  }

  free(reservations);
  free(state);
  return 0;
}

int book_seat_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  int bytes;
  int code;
  char aux_col;
  flight_seat_t seat;
  seat.choice = choice;
  strcpy(seat.flight_number, flight_number);

  get_flight_state_client(client_socket, GET_FLIGHT_STATE, flight_number);

  printf("Ingrese el asiento\n");
  code = scanf("%d%1[a-zA-Z]", &(seat.row), &aux_col);
  printf("code %d\n", code);
  while(code != 2) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("Ese no es un asiento válido. Ingréselo nuevamente.\n");
    code = scanf("%d%1[a-zA-Z]", &(seat.row), &aux_col);
  }

  printf("ingrese su DNI\n");
  code = scanf("%d", &seat.dni);
  while(code != 1) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("DNI inválido. Ingréselo nuevamente\n");
    code = scanf("%d", &seat.dni);
  }

  seat.col = aux_col - (islower(aux_col)  ? 'a' : 'A');
  seat.row--;

  //printf("mando el asiento: fil: %d col: %d dni: %d\n", seat.row, seat.col, seat.dni);
  bytes = write(client_socket, &seat, sizeof(flight_seat_t));
  if(bytes < 0) {
    printf("Se ha producido un error. Acción no realizada. Intentelo mas tarde.\n");
    return ERROR_SOCKET;
  }

  if((bytes = read(client_socket, &code, sizeof(int))) < 0) {
    printf("error al leer del socket (ultimo)\n");
    return ERROR_SOCKET;
  }
  if(code < 0) {
    printf("El asiento ingresado no pudo ser reservado.\n");
    return 1;
  }
  printf("Reserva realizada con éxito!\n");
  get_flight_state_client(client_socket, GET_FLIGHT_STATE, flight_number);
  return 0;
}

int cancel_seat_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  //int bytes;
  return 0;
}
int new_flight_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  //int bytes;
  return 0;
}
