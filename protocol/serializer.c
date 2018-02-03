#include "serializer.h"

/*--------------------------------SERIALIZERS--------------------------------*/

unsigned char * serialize_int(unsigned char * buffer, int value)
{
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;
  return buffer + 4;
}

unsigned char * serialize_char(unsigned char * buffer, char value)
{
  buffer[0] = value;
  return buffer + 1;
}

unsigned char * serialize_string(unsigned char * buffer, char * str)
{
  do {
    buffer = serialize_char(buffer, *str);
  } while(*str++ != '\0');
  return buffer;
}

unsigned char * serialize_reservation(unsigned char * buffer, reservation_t reservation)
{
  buffer = serialize_string(buffer, reservation.flight_number);
  buffer = serialize_int(buffer, reservation.seat_row);
  buffer = serialize_int(buffer, reservation.seat_col);
  buffer = serialize_int(buffer, reservation.dni);
  return buffer;
}

unsigned char * serialize_reservation_array(unsigned char * buffer, reservation_t * reservations, int length)
{
  while(length > 0) {
    buffer = serialize_reservation(buffer, *reservations++);
    length--;
  }
  return buffer;
}

unsigned char * serialize_flight(unsigned char * buffer, flight_t flight)
{
  buffer = serialize_string(buffer, flight.flight_number);
  buffer = serialize_int(buffer, flight.dim[0]);
  buffer = serialize_int(buffer, flight.dim[1]);
  return buffer;
}

unsigned char * serialize_flight_array(unsigned char * buffer, flight_t * flights, int length)
{
  while(length > 0) {
    buffer = serialize_flight(buffer, *flights++);
    length--;
  }
  return buffer;
}

/*-------------------------------DESERIALIZERS-------------------------------*/
unsigned char * deserialize_int(unsigned char * buffer, int * value)
{
  *value = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
  return buffer + 4;
}

unsigned char * deserialize_char(unsigned char * buffer, char * value)
{
  *value = buffer[0];
  return buffer + 1;
}

unsigned char * deserialize_string(unsigned char * buffer, char * str)
{
  do {
    buffer = deserialize_char(buffer, str);
  } while(*str++ != '\0');
  return buffer;
}

unsigned char * deserialize_reservation(unsigned char * buffer, reservation_t * reservation)
{
  buffer = deserialize_string(buffer, reservation->flight_number);
  buffer = deserialize_int(buffer, &(reservation->seat_row));
  buffer = deserialize_int(buffer, &(reservation->seat_col));
  buffer = deserialize_int(buffer, &(reservation->dni));
  return buffer;
}

unsigned char * deserialize_reservation_array(unsigned char * buffer, reservation_t * reservations, int length)
{
  while(length > 0) {
    buffer = deserialize_reservation(buffer, reservations++);
    length--;
  }
  return buffer;
}

unsigned char * deserialize_flight(unsigned char * buffer, flight_t * flight)
{
  buffer = deserialize_string(buffer, flight->flight_number);
  buffer = deserialize_int(buffer, &(flight->dim[0]));
  buffer = deserialize_int(buffer, &(flight->dim[1]));
  return buffer;
}

unsigned char * deserialize_flight_array(unsigned char * buffer, flight_t * flights, int length)
{
  while(length > 0) {
    buffer = deserialize_flight(buffer, flights++);
    length--;
  }
  return buffer;
}
