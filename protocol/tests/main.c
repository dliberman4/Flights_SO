#include <stdio.h>
#include <string.h>
#include "../messages.h"
#include "../serializer.h"

void test_int()
{
  unsigned char buffer[MAX_BUF_SIZE];
  int value;
  int des_value;

  value = 4;
  serialize_int(buffer, value);
  deserialize_int(buffer, &des_value);

  if(value != des_value) {
    printf("int not working :(\n");
    return;
  }
  printf("int OK!\n");
}

void test_char()
{
  unsigned char buffer[MAX_BUF_SIZE];
  char value;
  char des_value;

  value = 'D';
  serialize_char(buffer, value);
  deserialize_char(buffer, &des_value);

  if(value != des_value) {
    printf("char not working :(\n");
    return;
  }
  printf("char OK!\n");
}

void test_string()
{
  unsigned char buffer[MAX_BUF_SIZE];
  char value[] = "hola!";
  char des_value[10];

  serialize_string(buffer, value);
  deserialize_string(buffer, des_value);

  if(strcmp(value, des_value) != 0) {
    printf("string not working :(\n");
    return;
  }
  printf("string OK!\n");
}

void test_reservation()
{
  unsigned char buffer[MAX_BUF_SIZE];
  reservation_t value;
  reservation_t des_value;
  int ok = 1;

  strcpy(value.flight_number, "AR1234");
  value.seat_row = 2;
  value.seat_col = 4;
  value.dni = 39123456;
  serialize_reservation(buffer, value);
  deserialize_reservation(buffer, &des_value);

  if(strcmp(value.flight_number, des_value.flight_number) != 0) {
    printf("reservation not working :( (flight_number)\n");
    ok = 0;
  }
  if(value.seat_row != des_value.seat_row) {
    printf("reservation not working :( (seat_row)\n");
    ok = 0;
  }
  if(value.seat_col != des_value.seat_col) {
    printf("reservation not working :( (seat_col)\n");
    ok = 0;
  }
  if(value.dni != des_value.dni) {
    printf("reservation not working :( (dni)\n");
    ok = 0;
  }
  if(ok)
    printf("reservation OK!\n");
}

void test_reservation_array()
{
  unsigned char buffer[MAX_BUF_SIZE];
  reservation_t value[2];
  reservation_t des_value[2];
  int ok = 1;

  strcpy(value[0].flight_number, "AR1234");
  value[0].seat_row = 2;
  value[0].seat_col = 4;
  value[0].dni = 39123456;
  strcpy(value[1].flight_number, "AR4321");
  value[1].seat_row = 4;
  value[1].seat_col = 8;
  value[1].dni = 38123456;
  serialize_reservation_array(buffer, value, 2);
  deserialize_reservation_array(buffer, des_value, 2);

  if(strcmp(value[0].flight_number, des_value[0].flight_number) != 0 ||
      strcmp(value[1].flight_number, des_value[1].flight_number) != 0) {
    printf("reservation not working :( (flight_number)\n");
    ok = 0;
  }
  if(value[0].seat_row != des_value[0].seat_row ||
      value[1].seat_row != des_value[1].seat_row) {
    printf("reservation not working :( (seat_row)\n");
    ok = 0;
  }
  if(value[0].seat_col != des_value[0].seat_col ||
      value[1].seat_col != des_value[1].seat_col) {
    printf("reservation not working :( (seat_col)\n");
    ok = 0;
  }
  if(value[0].dni != des_value[0].dni ||
      value[1].dni != des_value[1].dni) {
    printf("reservation not working :( (dni)\n");
    ok = 0;
  }
  if(ok)
    printf("reservation OK!\n");
}

void test_flight()
{
  unsigned char buffer[MAX_BUF_SIZE];
  flight_t value;
  flight_t des_value;
  char error;

  strcpy(value.flight_number, "aaa");
  value.dim[0] = 20;
  value.dim[1] = 5;
  serialize_flight(buffer, value);
  deserialize_flight(buffer, &des_value);

  error = 0;

  if(strcmp(value.flight_number, des_value.flight_number) != 0) {
    printf("flight not working :( (different flight_number)\n");
    error = 1;
  }
  if(value.dim[0] != des_value.dim[0]) {
    printf("flight not working :( (different dim[0])\n");
    error = 1;
  }
  if(value.dim[1] != des_value.dim[1]) {
    printf("flight not working :( (different dim[1])\n");
    error = 1;
  }
  if(!error)
    printf("flight OK!\n");
}

int main()
{
  test_int();
  test_char();
  test_string();
  test_reservation();
  test_reservation_array();
  test_flight();

}
