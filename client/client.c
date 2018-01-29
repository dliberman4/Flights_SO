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
#include "../protocol/messages.h"
#include "../protocol/serializer.h"


int client_socket_initialize(const char * ip);
int get_flight_state_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int book_seat_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int cancel_seat_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int new_flight_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int remove_flight_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1]);
int get_reservations_client(int client_socket);
int get_cancellations_client(int client_socket);
int close_connection(int client_socket, int choice);

int client_socket_initialize(const char * ip)
{
  int client_socket;
  struct sockaddr_in address;

  if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return ERROR;
  }

  printf("client_socket creado\n");

  address.sin_family = AF_INET;
  address.sin_port = htons(SERVER_PORT);
  address.sin_addr.s_addr = inet_addr(ip);

  if(connect(client_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
    printf("error al conectar\n");
    close(client_socket);
    return ERROR;
  }
  return client_socket;
}

int close_connection(int client_socket, int choice) {
  msg_t msg;
  int bytes;

  msg.type = CLOSE;
  msg.bytes = 0;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    return ERROR;
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
    if(client_socket == ERROR) {
      printf("error al crear el socket\n");
      return 1;
    }
    should_close = 0;

    while(!should_close) {
      printf("Introduzca el numero de operacion:\n"
      "1: Obtener el estado de vuelo.\n"
      "2: Reservar asiento.\n"
      "3: Cancelar reserva de asiento.\n"
      "4: Crear nuevo vuelo\n"
      "5: Eliminar un vuelo\n"
      "6: Obtener reservas\n"
      "7: Obtener cancelaciones\n"
      "8: Salir\n");

      bzero(buffer, MAX_BUF_SIZE);
      scanf("%d", &choice);
      while(choice < 1 || choice > MAX_CHOICE) {
        printf("Esa opción no es correcta. Ingrese otra.\n");
        bzero(buffer, MAX_BUF_SIZE);
        scanf("%d", &choice);
      }

      if(choice != CLOSE) {
        printf("Ingrese el numero de vuelo\n");
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
          code = remove_flight_client(client_socket, choice, flight_number);
          break;
        case GET_RESERVATIONS:
          code = get_reservations_client(client_socket);
          break;
        case GET_CANCELLATIONS:
          code = get_cancellations_client(client_socket);
          break;
        case CLOSE:
          code = close_connection(client_socket, choice);
          if(code == 0) {
            should_close = 1;
          }
          break;
      }

    }

    close(client_socket);
    return code;
}

int get_flight_state_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  int bytes;
  int i,j;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  msg_t msg;
  flight_t flight;
  reservation_t * reservations;
  int reservations_quantity;
  char * state;

  msg.type = GET_FLIGHT_STATE;
  end_of_buffer = serialize_string(buffer, flight_number);
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    printf("Se ha producido un error. Acción no realizada. Intentelo mas tarde.\n");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    printf("error al leer del socket\n");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    printf("Vuelo no encontrado\n");
    return ERROR;
  }

  end_of_buffer = deserialize_flight(msg.buffer, &flight);
  printf("tengo %d filas y %d cols en el vuelo %s\n", flight.dim[0], flight.dim[1], flight.flight_number);
  end_of_buffer = deserialize_int(end_of_buffer, &reservations_quantity);
  printf("hay %d reservas\n", reservations_quantity);
  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  end_of_buffer = deserialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  free(msg.buffer);


  state = malloc(sizeof(char) * flight.dim[0] * (flight.dim[1]));

  for(i = 0; i < flight.dim[0]; i++) {
    for(j = 0; j < flight.dim[1]; j++) {
      state[(flight.dim[1])*i + j] = 'D';
    }
  }

  for(i = 0; i < reservations_quantity; i++) {
    //printf("dni: %d, fila: %d, col: %d\n", reservations[i].dni, reservations[i].seat_row, reservations[i].seat_col);
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
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  reservation_t reservation;
  msg_t msg;

  strcpy(reservation.flight_number, flight_number);

  code = get_flight_state_client(client_socket, GET_FLIGHT_STATE, flight_number);

  if(code == ERROR)
    return ERROR;

  printf("Ingrese el asiento\n");
  code = scanf("%d%1[a-zA-Z]", &(reservation.seat_row), &aux_col);

  while(code != 2) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("Ese no es un asiento válido. Ingréselo nuevamente.\n");
    code = scanf("%d%1[a-zA-Z]", &(reservation.seat_row), &aux_col);
  }
  reservation.seat_col = aux_col - (islower(aux_col)  ? 'a' : 'A');
  reservation.seat_row--;

  printf("Ingrese su DNI\n");
  code = scanf("%d", &reservation.dni);
  while(code != 1) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("DNI inválido. Ingréselo nuevamente\n");
    code = scanf("%d", &reservation.dni);
  }

  end_of_buffer = serialize_reservation(buffer, reservation);

  msg.type = BOOK_SEAT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    printf("Se ha producido un error. Acción no realizada. Intentelo mas tarde.\n");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    printf("error al leer del socket (response)\n");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    printf("El asiento ingresado no pudo ser reservado.\n");
  }
  else if(msg.type == RESPONSE_OK) {
    printf("Reserva realizada con éxito!\n");
  }

  code = get_flight_state_client(client_socket, GET_FLIGHT_STATE, flight_number);
  if(code < 0)
    return ERROR;

  return 0;
}

int cancel_seat_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  int bytes;
  int code;
  char aux_col;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  reservation_t reservation;
  msg_t msg;

  strcpy(reservation.flight_number, flight_number);

  code = get_flight_state_client(client_socket, GET_FLIGHT_STATE, flight_number);

  if(code == ERROR)
    return ERROR;

  printf("Ingrese el asiento\n");
  code = scanf("%d%1[a-zA-Z]", &(reservation.seat_row), &aux_col);

  while(code != 2) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("Ese no es un asiento válido. Ingréselo nuevamente.\n");
    code = scanf("%d%1[a-zA-Z]", &(reservation.seat_row), &aux_col);
  }
  reservation.seat_col = aux_col - (islower(aux_col)  ? 'a' : 'A');
  reservation.seat_row--;

  printf("Ingrese su DNI\n");
  code = scanf("%d", &reservation.dni);
  while(code != 1) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("DNI inválido. Ingréselo nuevamente\n");
    code = scanf("%d", &reservation.dni);
  }

  end_of_buffer = serialize_reservation(buffer, reservation);

  msg.type = CANCEL_SEAT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    printf("Se ha producido un error. Acción no realizada. Intentelo mas tarde.\n");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    printf("error al leer del socket (response)\n");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    printf("Acción no realizada. Datos inválidos.\n");
  }
  else if(msg.type == RESPONSE_OK) {
    printf("Reserva cancelada con éxito!\n");
  }

  get_flight_state_client(client_socket, GET_FLIGHT_STATE, flight_number);
  return 0;
}

int new_flight_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  int bytes;
  int code;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  flight_t flight;
  msg_t msg;

  strcpy(flight.flight_number, flight_number);

  printf("Ingrese la cantidad de filas del avión\n");
  code = scanf("%d", &flight.dim[0]);
  while(code != 1) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("Cantidad de filas inválidas. Ingrésela nuevamente\n");
    code = scanf("%d", &flight.dim[0]);
  }

  printf("Ingrese la cantidad de columnas del avión\n");
  code = scanf("%d", &flight.dim[1]);
  while(code != 1) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("Cantidad de columnas inválidas. Ingrésela nuevamente\n");
    code = scanf("%d", &flight.dim[1]);
  }

  end_of_buffer = serialize_flight(buffer, flight);

  msg.type = NEW_FLIGHT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    printf("Se ha producido un error. Acción no realizada. Intentelo mas tarde.\n");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    printf("error al leer del socket (response)\n");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    printf("Acción no realizada. Datos inválidos.\n");
  }
  else if(msg.type == RESPONSE_OK) {
    printf("Vuelo creado con éxito!\n"
    "Número de vuelo: %s\n"
    "Cantidad de filas: %d\n"
    "Cantidad de columnas: %d\n\n", flight.flight_number, flight.dim[0], flight.dim[1]);
  }

  return 0;
}

int remove_flight_client(int client_socket, int choice, char flight_number[MAX_FLIGHT_NUMBER+1])
{
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;
  msg_t msg;

  end_of_buffer = serialize_string(buffer, flight_number);

  msg.type = REMOVE_FLIGHT;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    printf("Se ha producido un error. Acción no realizada. Intentelo mas tarde.\n");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    printf("error al leer del socket (response)\n");
    return ERROR;
  }

  if(msg.type == RESPONSE_ERROR) {
    printf("Acción no realizada. Datos inválidos.\n");
  }
  else if(msg.type == RESPONSE_OK) {
    printf("Vuelo eliminado con éxito!\n");
  }
  return 0;
}
int get_reservations_client(int client_socket)
{
  msg_t msg;
  int reservations_quantity;
  reservation_t * reservations;
  unsigned char * end_of_buffer;
  int i;
  int bytes;

  msg.type = GET_RESERVATIONS;
  msg.bytes = 0;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    printf("error al escribir en el socket\n");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    printf("error al leer del socket\n");
    return ERROR;
  }

  end_of_buffer = deserialize_int(msg.buffer, &reservations_quantity);
  printf("hay %d reservas\n", reservations_quantity);
  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  end_of_buffer = deserialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  free(msg.buffer);

  for(i = 0; i < reservations_quantity; i++) {
    printf("dni: %d, fila: %d, col: %d\n", reservations[i].dni, reservations[i].seat_row, reservations[i].seat_col);
  }

  free(reservations);
  return 0;
}

int get_cancellations_client(int client_socket)
{
  msg_t msg;
  int cancellations_quantity;
  reservation_t * cancellations;
  unsigned char * end_of_buffer;
  int i;
  int bytes;

  msg.type = GET_CANCELLATIONS;
  msg.bytes = 0;

  bytes = send_msg(client_socket, msg);
  if(bytes < 0) {
    printf("error al escribir en el socket\n");
    return ERROR;
  }

  bytes = receive_msg(client_socket, &msg);
  if(bytes < 0) {
    printf("error al leer del socket\n");
    return ERROR;
  }

  end_of_buffer = deserialize_int(msg.buffer, &cancellations_quantity);
  printf("hay %d cancelaciones\n", cancellations_quantity);
  cancellations = (reservation_t *) malloc(sizeof(reservation_t) * cancellations_quantity);

  end_of_buffer = deserialize_reservation_array(end_of_buffer, cancellations, cancellations_quantity);

  free(msg.buffer);

  for(i = 0; i < cancellations_quantity; i++) {
    printf("dni: %d, fila: %d, col: %d\n", cancellations[i].dni, cancellations[i].seat_row, cancellations[i].seat_col);
  }

  free(cancellations);

  return 0;
}
