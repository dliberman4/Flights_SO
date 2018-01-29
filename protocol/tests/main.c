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

}

void test_string()
{

}

void test_reservation()
{

}

void test_reservation_array()
{

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

void test_messages()
{
  msg_t msg;

  msg.type = GET_FLIGHT_STATE;
  msg.bytes = sizeof(int);
}

int main()
{
  test_int();
  test_flight();
}
