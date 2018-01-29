#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "../../protocol/types.h"

#define DB_OK 0
#define DB_ERROR -1
#define DB_BUSY -2
#define DB_WRONG_RESULT -3
#define DB_FILE_NAME "flights.db"

int db_open();
int db_close(int should_wait);
int db_check_tables();
int db_new_flight(const char * flight_number, int plane_rows, int plane_cols);
int db_get_flight_dim(const char * flight_number, int dim[2]);
int db_remove_flight(const char * flight_number);
int db_get_reservations_quantity(const char * flight_number);
int db_get_reservations(const char * flight_number, reservation_t * reservations);
int db_get_cancellations_quantity(const char * flight_number);
int db_get_cancellations(const char * flight_number, reservation_t * cancellations);
int db_book_seat(reservation_t * reservation);
int db_exists_reservation(reservation_t * reservation);
int db_cancel_seat(reservation_t * reservation);

#endif
