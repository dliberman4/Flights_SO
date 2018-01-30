#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "frontend.h"
#include "../../protocol/protocol_constants.h"

#define FLIGHT_NUMBER_TOO_LONG 10
#define STR2(a) #a
#define STR(a) STR2(a)
#define GREEN_COLOR "\x1B[32m"
#define DEFAULT_COLOR "\x1B[0m"
#define RED_COLOR "\x1B[31m"

int show_menu()
{
  int choice;

  printf("Introduzca el numero de operacion:\n"
  "1: Obtener el estado de vuelo.\n"
  "2: Reservar asiento.\n"
  "3: Cancelar reserva de asiento.\n"
  "4: Crear nuevo vuelo\n"
  "5: Eliminar un vuelo\n"
  "6: Obtener reservas\n"
  "7: Obtener cancelaciones\n"
  "8: Salir\n");

  scanf("%d", &choice);
  while(choice < 1 || choice > MAX_CHOICE) {
    printf("Esa opción no es correcta. Ingrese otra.\n");
    scanf("%d", &choice);
  }

  return choice;
}

void print_error_msg(char * msg)
{
  printf(RED_COLOR"Error: "DEFAULT_COLOR"%s\n", msg);
}

void print_ok_msg(char * msg)
{
  printf(GREEN_COLOR"OK: "DEFAULT_COLOR"%s\n", msg);
}

void print_flight(flight_t * flight)
{
  printf("Número de vuelo: %s\n"
  "Cantidad de filas: %d\n"
  "Cantidad de columnas: %d\n\n",
  flight->flight_number, flight->dim[0], flight->dim[1]);
}

void print_reservation(reservation_t * reservation)
{
  printf("%-15s | %-4d | %-7c | %d\n",
  reservation->flight_number, reservation->seat_row+1,
  reservation->seat_col+'A', reservation->dni);
}

void print_reservations(reservation_t * reservations, int quantity)
{
  int i;

  if(quantity == 0) {
    print_ok_msg("No hay datos para mostrar.");
    return;
  }

  printf("Número de vuelo | Fila | Columna | DNI\n");
  for(i = 0; i < quantity; i++) {
    print_reservation(&reservations[i]);
  }

}

void print_DAV(int dim[2], reservation_t * reservations, int reservations_quantity)
{
  char * state;
  int i,j;

  state = malloc(sizeof(char) * dim[0] * (dim[1]));

    for(i = 0; i < dim[0]; i++) {
      for(j = 0; j < dim[1]; j++) {
        state[(dim[1])*i + j] = 'D';
      }
    }

    for(i = 0; i < reservations_quantity; i++) {
      state[(dim[1])* (reservations[i].seat_row) + (reservations[i].seat_col)] = 'R';
    }
    printf("   |");
    for(j = 0; j < dim[1]; j++) {
      putchar(j+'A');
    }
    putchar('\n');
    for(i = 0; i < dim[0]; i++) {
      printf("%3d|", i+1);
      for(j = 0; j < dim[1]; j++) {
        if(state[(dim[1])*i + j] == 'D'){
          printf(GREEN_COLOR"%c"DEFAULT_COLOR, state[(dim[1])*i + j]);
        }
        else
          printf(RED_COLOR"%c"DEFAULT_COLOR, state[(dim[1])*i + j]);
      }
      putchar('\n');
    }
  free(state);
}

void get_flight_number(char * flight_number)
{
  char buffer[FLIGHT_NUMBER_TOO_LONG+1];

  printf("Ingrese el numero de vuelo\n");
  scanf("%"STR(FLIGHT_NUMBER_TOO_LONG)"s", buffer);
  while(strlen(buffer) > MAX_FLIGHT_NUMBER) {
    printf("Ese numero no es válido. Ingrese máximo %d caracteres\n", MAX_FLIGHT_NUMBER);
    scanf("%s", buffer);
  }
  strcpy(flight_number, buffer);
}

int get_int(char * msg, char * error_msg)
{
  int code;
  int num;

  printf("%s\n", msg);
  code = scanf("%d", &num);
  while(code != 1 || num < 0) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    print_error_msg(error_msg);
    code = scanf("%d", &num);
  }

  return num;
}

void get_seat(int * row, int * col)
{
  char aux_col;
  int code;

  printf("Ingrese el asiento\n");
  code = scanf("%d%1[a-zA-Z]", row, &aux_col);
  while(code != 2) {
    while ((code = getchar()) != '\n' && code != EOF) { }
    printf("Ese no es un asiento válido. Ingréselo nuevamente.\n");
    code = scanf("%d%1[a-zA-Z]", row, &aux_col);
  }
  *col = aux_col - (islower(aux_col)  ? 'a' : 'A');
  (*row)--;
}
