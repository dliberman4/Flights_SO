#include "serializer.h"

/*--------------------------------SERIALIZERS--------------------------------*/

char * serialize_int(char * buffer, int value)
{
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;
  return buffer + 4;
}

char * serialize_char(char * buffer, char value)
{
  buffer[0] = value;
  return buffer + 1;
}

char * serialize_string(char * buffer, char * str)
{
  do {
    buffer = serialize_char(buffer, *str);
  } while(*str++ != '\0');
  return buffer;
}

char * serialize_reservation(char * buffer, reservation_t reservation)
{
  buffer = serialize_string(buffer, reservation.flight_number);
  buffer = serialize_int(buffer, reservation.seat_row);
  buffer = serialize_int(buffer, reservation.seat_col);
  buffer = serialize_int(buffer, reservation.dni);
  return buffer;
}

char * serialize_reservation_array(char * buffer, reservation_t * reservations, int length)
{
  buffer = serialize_int(buffer, length);
  while(length > 0) {
    buffer = serialize_reservation(buffer, *reservations++);
    length--;
  }
  return buffer;
}

char * serialize_flight(char * buffer, flight_t flight)
{
  buffer = serialize_string(buffer, flight.flight_number);
  buffer = serialize_int(buffer, flight.dim[0]);
  buffer = serialize_int(buffer, flight.dim[1]);
  return buffer;
}

/*-------------------------------DESERIALIZERS-------------------------------*/
char * deserialize_int(char * buffer, int * value)
{
  *value = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
  return buffer + 4;
}

char * deserialize_char(char * buffer, char * value)
{
  *value = buffer[0];
  return buffer + 1;
}

char * deserialize_string(char * buffer, char * str)
{
  do {
    buffer = deserialize_char(buffer, str);
  } while(*str++ != '\0');
  return buffer;
}

char * deserialize_reservation(char * buffer, reservation_t * reservation)
{
  buffer = deserialize_string(buffer, reservation->flight_number);
  buffer = deserialize_int(buffer, &(reservation->seat_row));
  buffer = deserialize_int(buffer, &(reservation->seat_col));
  buffer = deserialize_int(buffer, &(reservation->dni));
  return buffer;
}

char * deserialize_reservation_array(char * buffer, reservation_t * reservations)
{
  int length;

  buffer = deserialize_int(buffer, &length);
  while(length > 0) {
    buffer = deserialize_reservation(buffer, reservations++);
    length--;
  }
  return buffer;
}

char * deserialize_flight(char * buffer, flight_t * flight)
{
  buffer = deserialize_string(buffer, flight->flight_number);
  buffer = deserialize_int(buffer, &(flight->dim[0]));
  buffer = deserialize_int(buffer, &(flight->dim[1]));
  return buffer;
}
