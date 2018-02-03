#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "types.h"

/*--------------------------------SERIALIZERS--------------------------------*/
unsigned char * serialize_int(unsigned char * buffer, int value);
unsigned char * serialize_char(unsigned char * buffer, char value);
unsigned char * serialize_string(unsigned char * buffer, char * str);
unsigned char * serialize_reservation(unsigned char * buffer, reservation_t reservation);
unsigned char * serialize_reservation_array(unsigned char * buffer, reservation_t * reservations, int length);
unsigned char * serialize_flight(unsigned char * buffer, flight_t flight);
unsigned char * serialize_flight_array(unsigned char * buffer, flight_t * flights, int length);
/*-------------------------------DESERIALIZERS-------------------------------*/
unsigned char * deserialize_int(unsigned char * buffer, int * value);
unsigned char * deserialize_char(unsigned char * buffer, char * value);
unsigned char * deserialize_string(unsigned char * buffer, char * str);
unsigned char * deserialize_reservation(unsigned char * buffer, reservation_t * reservation);
unsigned char * deserialize_reservation_array(unsigned char * buffer, reservation_t * reservations, int length);
unsigned char * deserialize_flight(unsigned char * buffer, flight_t * flight);
unsigned char * deserialize_flight_array(unsigned char * buffer, flight_t * flights, int length);

#endif
