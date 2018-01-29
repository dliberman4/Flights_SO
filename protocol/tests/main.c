#include <stdio.h>
#include "../messages.h"
#include "../serializer.h"

void test_int()
{
  char buffer[MAX_BUF_SIZE];
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
}
