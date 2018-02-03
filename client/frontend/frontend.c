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
#define BLUE_COLOR "\x1B[34m"

void show_title()
{
  printf(
          "  ______ _ _       _     _       \n"
          " |  ____| (_)     | |   | |      \n"
          " | |__  | |_  __ _| |__ | |_ ___ \n"
          " |  __| | | |/ _` | '_ \\| __/ __|\n"
          " | |    | | | (_| | | | | |_\\__ \\\n"
          " |_|    |_|_|\\__, |_| |_|\\__|___/\n"
          "              __/ |              \n"
          "             |___/               \n"
);
}

int show_menu()
{
  int choice = 0;
  int code;

  printf("---------------------------------\n");
  printf(BLUE_COLOR"1:"DEFAULT_COLOR" Obtener el estado de vuelo.\n"
  BLUE_COLOR"2:"DEFAULT_COLOR" Reservar asiento.\n"
  BLUE_COLOR"3:"DEFAULT_COLOR" Cancelar reserva de asiento.\n"
  BLUE_COLOR"4:"DEFAULT_COLOR" Crear nuevo vuelo\n"
  BLUE_COLOR"5:"DEFAULT_COLOR" Eliminar un vuelo\n"
  BLUE_COLOR"6:"DEFAULT_COLOR" Obtener reservas\n"
  BLUE_COLOR"7:"DEFAULT_COLOR" Obtener cancelaciones\n"
  BLUE_COLOR"8:"DEFAULT_COLOR" Salir\n"
  "---------------------------------\n"
  "Introduzca el numero de operacion:"BLUE_COLOR"\n> "DEFAULT_COLOR);

  scanf("%d", &choice);
  while(choice < 1 || choice > MAX_CHOICE) {
    printf("Esa opción no es correcta. Ingrese otra."BLUE_COLOR"\n> "DEFAULT_COLOR);
    while ((code = getchar()) != EOF && code != '\n') { }
    scanf("%d", &choice);
  }
  while ((code = getchar()) != EOF && code != '\n') { }
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
  int code;

  printf("Ingrese el numero de vuelo\n"BLUE_COLOR"> "DEFAULT_COLOR);
  scanf("%"STR(FLIGHT_NUMBER_TOO_LONG)"s", buffer);
  while(strlen(buffer) > MAX_FLIGHT_NUMBER) {
    printf("Ese numero no es válido. Ingrese máximo %d caracteres\n"BLUE_COLOR"> "DEFAULT_COLOR, MAX_FLIGHT_NUMBER);
    while ((code = getchar()) != '\n' && code != EOF) { }
    scanf("%s", buffer);
  }
  strcpy(flight_number, buffer);
  while ((code = getchar()) != EOF && code != '\n') { }
}

int get_int(char * msg, char * error_msg)
{
  int code;
  int num;

  printf("%s\n"BLUE_COLOR"> "DEFAULT_COLOR, msg);
  code = scanf("%d", &num);
  while(code != 1 || num < 0) {
    printf("%s\n"BLUE_COLOR"> "DEFAULT_COLOR, error_msg);
    while ((code = getchar()) != EOF && code != '\n') { }
    code = scanf("%d", &num);
  }
  while ((code = getchar()) != EOF && code != '\n') { }
  return num;
}

void get_seat(int * row, int * col)
{
  int code;
  char aux_col[2];

  printf("Ingrese el asiento\n"BLUE_COLOR"> "DEFAULT_COLOR);
  code = scanf("%d%1[a-zA-Z]", row, aux_col);
  while(code != 2) {
    printf("Ese no es un asiento válido. Ingréselo nuevamente.\n"BLUE_COLOR"> "DEFAULT_COLOR);
    while ((code = getchar()) != '\n' && code != EOF) { }
    code = scanf("%d%1[a-zA-Z]", row, aux_col);
  }
  *col = aux_col[0] - (islower(aux_col[0])  ? 'a' : 'A');
  (*row)--;

  while ((code = getchar()) != EOF && code != '\n') { }
}
