#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "types.h"

/*--------------------------------SERIALIZERS--------------------------------*/
char * serialize_int(char * buffer, int value);
char * serialize_char(char * buffer, char value);
char * serialize_string(char * buffer, char * str);
char * serialize_reservation(char * buffer, reservation_t reservation);
char * serialize_reservation_array(char * buffer, reservation_t * reservations, int length);
char * serialize_flight(char * buffer, flight_t flight);
/*-------------------------------DESERIALIZERS-------------------------------*/
char * deserialize_int(char * buffer, int * value);
char * deserialize_char(char * buffer, char * value);
char * deserialize_string(char * buffer, char * str);
char * deserialize_reservation(char * buffer, reservation_t * reservation);
char * deserialize_reservation_array(char * buffer, reservation_t * reservations);
char * deserialize_flight(char * buffer, flight_t * flight);

#endif
