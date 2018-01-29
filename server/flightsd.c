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
#include "database/database.h"
#include "../protocol/messages.h"
#include "../protocol/serializer.h"

int open_socket();
void bind_to_port(int listener_socket, int port);
void sigchld_handler(int sig);
int get_flight_state_server(int accepted_socket, msg_t msg);
int book_seat_server(int accepted_socket, msg_t msg);
int cancel_seat_server(int accepted_socket, msg_t msg);
int new_flight_server(int accepted_socket, msg_t msg);
int remove_flight_server(int accepted_socket, msg_t msg);
int get_reservations_server(int accepted_socket, msg_t msg);
int get_cancellations_server(int accepted_socket, msg_t msg);

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

void sigchld_handler(int sig)
{
  while (waitpid(-1, 0, WNOHANG) > 0)
  ;
  return;
}

int main()
{
  int listener_socket;
  int accepted_socket;
  int bytes;
  int code;
  int should_close;
  msg_t msg;
  socklen_t address_size;
  struct sockaddr_storage client;

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

  if(listen(listener_socket, LISTEN_QUEUE_SIZE) == -1)
    printf("listen error\n");

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
              should_close = 0;

              while(!should_close) {
                printf("esperando recibir msg\n");
                bytes = receive_msg(accepted_socket, &msg);

                if(bytes < 0) {
                  printf("error al leer del socket\n");
                  return ERROR_SERVER;
                }

                printf("msg.type = %d\n", msg.type);
                switch(msg.type) {
                  case GET_FLIGHT_STATE:
                      code = get_flight_state_server(accepted_socket, msg);
                      break;
                  case BOOK_SEAT:
                      code = book_seat_server(accepted_socket, msg);
                      break;
                  case CANCEL_SEAT:
                      code = cancel_seat_server(accepted_socket, msg);
                      break;
                  case NEW_FLIGHT:
                      code = new_flight_server(accepted_socket, msg);
                      break;
                  case REMOVE_FLIGHT:
                      code = remove_flight_server(accepted_socket, msg);
                      break;
                  case GET_RESERVATIONS:
                      code = get_reservations_server(accepted_socket, msg);
                      break;
                  case GET_CANCELLATIONS:
                      code = get_cancellations_server(accepted_socket, msg);
                      break;
                  case CLOSE:
                      should_close = 1;
                      break;
                  default:
                      break;
                }

                if(code < 0){
                  printf("error: %d\n", code);
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


int get_flight_state_server(int accepted_socket, msg_t msg)
{
  int reservations_quantity;
  reservation_t * reservations;
  int code;
  int bytes;
  flight_t flight;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;

  deserialize_string(msg.buffer, flight.flight_number);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    printf("error al abrir la db\n");
  }
  code = db_get_flight_dim(flight.flight_number, flight.dim);
  if(code < 0) {
    printf("error al pedir las dimensiones\n");
    msg.type = RESPONSE_ERROR;
    msg.bytes = 0;
    bytes = send_msg(accepted_socket, msg);
    return ERROR_SERVER;
  }
  printf("el vuelo %s tiene filas: %d y cols: %d\n", flight.flight_number, flight.dim[0], flight.dim[1]);

  reservations_quantity = db_get_reservations_quantity(flight.flight_number);
  printf("la cantidad de reservas es: %d\n", reservations_quantity);

  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  if(db_get_reservations(flight.flight_number, reservations) < 0) {
    printf("error al obtener las reservas\n");
  }

  end_of_buffer = serialize_flight(buffer, flight);
  end_of_buffer = serialize_int(end_of_buffer, reservations_quantity);
  end_of_buffer = serialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  msg.type = GET_FLIGHT_STATE;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    printf("error al mandar msg\n");
  }

  free(reservations);
  printf("db a punto de cerrarse\n");
  code = db_close(1);
  printf("db cerrada\n");
  if(code < 0) {
    printf("error al cerrar la db\n");
  }

  return (code < 0 || bytes < 0) * -1;
}

int book_seat_server(int accepted_socket, msg_t msg)
{
  int code;
  int bytes;
  reservation_t reservation;

  deserialize_reservation(msg.buffer, &reservation);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    printf("error al abrir la db\n");
  }
  printf("flight_number: %s, seat_row: %d, seat_col: %d, dni: %d\n",
        reservation.flight_number, reservation.seat_row, reservation.seat_col,
        reservation.dni);
  code = db_book_seat(&reservation);

  printf("code: %d\n", code);
  if(code < 0)
    msg.type = RESPONSE_ERROR;
  else
    msg.type = RESPONSE_OK;

  msg.bytes = 0;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    printf("error al escribir en el socket (response)\n");
  }
  printf("antes cerrar la db\n");
  code = db_close(1);
  if(code < 0) {
    printf("error al cerrar la db\n");
  }
  printf("despues cerrar la db\n");
  return 0;
}
int cancel_seat_server(int accepted_socket, msg_t msg)
{
  int code;
  int bytes;
  reservation_t reservation;

  deserialize_reservation(msg.buffer, &reservation);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    printf("error al abrir la db\n");
  }
  printf("flight_number: %s, seat_row: %d, seat_col: %d, dni: %d\n",
        reservation.flight_number, reservation.seat_row, reservation.seat_col,
        reservation.dni);
  code = db_cancel_seat(&reservation);

  printf("code: %d\n", code);
  if(code < 0)
    msg.type = RESPONSE_ERROR;
  else
    msg.type = RESPONSE_OK;

  msg.bytes = 0;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    printf("error al escribir en el socket (response)\n");
  }

  code = db_close(1);
  if(code < 0) {
    printf("error al cerrar la db\n");
  }
  return 0;
}
int new_flight_server(int accepted_socket, msg_t msg)
{
  int code;
  int bytes;
  flight_t flight;

  deserialize_flight(msg.buffer, &flight);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    printf("error al abrir la db\n");
  }

  code = db_new_flight(flight.flight_number, flight.dim[0], flight.dim[1]);

  if(code < 0)
    msg.type = RESPONSE_ERROR;
  else
    msg.type = RESPONSE_OK;

  msg.bytes = 0;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    printf("error al escribir en el socket (response)\n");
  }

  code = db_close(1);
  if(code < 0) {
    printf("error al cerrar la db\n");
  }
  return 0;
}

int remove_flight_server(int accepted_socket, msg_t msg)
{
  int code;
  int bytes;
  char flight_number[MAX_FLIGHT_NUMBER];

  deserialize_string(msg.buffer, flight_number);
  free(msg.buffer);

  code = db_open();
  if(code < 0) {
    printf("error al abrir la db\n");
  }

  code = db_remove_flight(flight_number);

  if(code < 0)
    msg.type = RESPONSE_ERROR;
  else
    msg.type = RESPONSE_OK;

  msg.bytes = 0;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    printf("error al escribir en el socket (response)\n");
  }

  code = db_close(1);
  if(code < 0) {
    printf("error al cerrar la db\n");
  }
  return 0;
}

int get_reservations_server(int accepted_socket, msg_t msg)
{
  int reservations_quantity;
  reservation_t * reservations;
  int code;
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;

  code = db_open();
  if(code < 0) {
    printf("error al abrir la db\n");
  }

  reservations_quantity = db_get_reservations_quantity(NULL);
  printf("la cantidad de reservas totales es: %d\n", reservations_quantity);

  reservations = (reservation_t *) malloc(sizeof(reservation_t) * reservations_quantity);

  if(db_get_reservations(NULL, reservations) < 0) {
    printf("error al obtener las reservas\n");
  }

  end_of_buffer = serialize_int(buffer, reservations_quantity);
  end_of_buffer = serialize_reservation_array(end_of_buffer, reservations, reservations_quantity);

  msg.type = GET_RESERVATIONS;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    printf("error al mandar msg\n");
  }

  free(reservations);
  printf("db a punto de cerrarse\n");
  code = db_close(1);
  printf("db cerrada\n");
  if(code < 0) {
    printf("error al cerrar la db\n");
  }

  return 0;
}

int get_cancellations_server(int accepted_socket, msg_t msg)
{
  int cancellations_quantity;
  reservation_t * cancellations;
  int code;
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * end_of_buffer;

  code = db_open();
  if(code < 0) {
    printf("error al abrir la db\n");
  }

  cancellations_quantity = db_get_cancellations_quantity(NULL);
  printf("la cantidad de cancelaciones totales es: %d\n", cancellations_quantity);

  cancellations = (reservation_t *) malloc(sizeof(reservation_t) * cancellations_quantity);

  if(db_get_cancellations(NULL, cancellations) < 0) {
    printf("error al obtener las reservas\n");
  }

  end_of_buffer = serialize_int(buffer, cancellations_quantity);
  end_of_buffer = serialize_reservation_array(end_of_buffer, cancellations, cancellations_quantity);

  msg.type = GET_CANCELLATIONS;
  msg.bytes = end_of_buffer - buffer;
  msg.buffer = buffer;

  bytes = send_msg(accepted_socket, msg);

  if(bytes < 0) {
    printf("error al mandar msg\n");
  }

  free(cancellations);
  printf("db a punto de cerrarse\n");
  code = db_close(1);
  printf("db cerrada\n");
  if(code < 0) {
    printf("error al cerrar la db\n");
  }

  return 0;
}
