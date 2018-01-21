#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LENGTH 255

static sqlite3 * db_connection;


int db_open()
{
  int code;

  code = sqlite3_open(DB_FILE_NAME, &db_connection);

  if(code != SQLITE_OK)
    return DB_ERROR;

  sqlite3_db_config(db_connection, SQLITE_DBCONFIG_ENABLE_FKEY, 1, NULL);
  return DB_OK;
}

int db_close(int should_wait)
{
  int code;

  code = sqlite3_close(db_connection);
  if(code == SQLITE_BUSY) {
    if(!should_wait)
      return DB_BUSY;
    while(code == SQLITE_BUSY) {
      code = sqlite3_close(db_connection);
    }
  }
  if(code)
    return DB_ERROR;
  return DB_OK;
}

int db_check_tables() {
  int code;
  char * query;
  sqlite3_stmt * statement;

  query = "select * from flight, reservation, cancellation;";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  code = sqlite3_step(statement);
  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;

  return DB_OK;
}

int db_new_flight(const char * flight_number, int plane_rows, int plane_cols)
{
  int code;
  char * query;
  sqlite3_stmt * statement;

  query = "insert into flight values (?, ?, ?);";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  sqlite3_bind_text(statement, 1, flight_number, -1, NULL);
  sqlite3_bind_int(statement, 2, plane_rows);
  sqlite3_bind_int(statement, 3, plane_cols);

  code = sqlite3_step(statement);
  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;

  return DB_OK;
}

int db_get_flight_dim(const char * flight_number, int dim[2])
{
  int code;
  char * query;
  sqlite3_stmt * statement;

  query = "select plane_rows,plane_cols from flight where flight_number = ?;";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK) {
    printf("error 1\n");
    return DB_ERROR;
  }

  sqlite3_bind_text(statement, 1, flight_number, -1, NULL);

  code = sqlite3_step(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW) {
    printf("error 2\n");
    return DB_ERROR;
  }
  dim[0] = sqlite3_column_int(statement, 0);
  dim[1] = sqlite3_column_int(statement, 1);


  sqlite3_finalize(statement);
  return DB_OK;
}

int db_remove_flight(const char * flight_number)
{
  int code;
  char * query;
  sqlite3_stmt * statement;

  query = "delete from flight where flight_number = ?;";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  sqlite3_bind_text(statement, 1, flight_number, -1, NULL);

  code = sqlite3_step(statement);
  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;

  return DB_OK;
}

int db_get_reservations_quantity(const char * flight_number)
{
  int code, quantity;
  char * query;
  sqlite3_stmt * statement;

  if(flight_number != NULL)
    query = "select count(*) from reservation where flight_number = ?;";
  else
    query = "select count(*) from reservation;";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  if(flight_number != NULL)
    sqlite3_bind_text(statement, 1, flight_number, -1, NULL);

  code = sqlite3_step(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;

  quantity = sqlite3_column_int(statement, 0);
  sqlite3_finalize(statement);
  return quantity;
}

int db_exists_reservation(reservation_t * reservation)
{
  int code;
  char * query;
  sqlite3_stmt * statement;

  query = "select * from reservation where flight_number = ? and seat_row = ? and seat_col = ? and dni = ?;";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  sqlite3_bind_text(statement, 1, reservation->flight_number, -1, NULL);
  sqlite3_bind_int(statement, 2, reservation->seat_row);
  sqlite3_bind_int(statement, 3, reservation->seat_col);
  sqlite3_bind_int(statement, 4, reservation->dni);

  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return 0;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;
  return 1;
}

int db_get_reservations(const char * flight_number, reservation_t * reservations)
{
  int code;
  int i = 0;
  char * query;
  sqlite3_stmt * statement;

  if(flight_number != NULL)
    query = "select * from reservation where flight_number = ?;";
  else
    query = "select * from reservation;";
  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  if(flight_number != NULL)
    sqlite3_bind_text(statement, 1, flight_number, -1, NULL);

  while((code = sqlite3_step(statement)) == SQLITE_ROW) {
    strcpy(reservations[i].flight_number, (const char *)sqlite3_column_text(statement, 0));
    reservations[i].seat_row = sqlite3_column_int(statement, 1);
    reservations[i].seat_col = sqlite3_column_int(statement, 2);
    reservations[i].dni = sqlite3_column_int(statement, 3);
    i++;
  }
  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE)
    return DB_ERROR;
  return DB_OK;
}

int db_get_cancellations_quantity(const char * flight_number)
{
  int code, quantity;
  char * query;
  sqlite3_stmt * statement;

  if(flight_number != NULL)
    query = "select count(*) from cancellation where flight_number = ?;";
  else
    query = "select count(*) from cancellation;";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  if(flight_number != NULL)
    sqlite3_bind_text(statement, 1, flight_number, -1, NULL);

  code = sqlite3_step(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;

  quantity = sqlite3_column_int(statement, 0);
  sqlite3_finalize(statement);
  return quantity;
}

int db_get_cancellations(const char * flight_number, reservation_t * cancellations)
{
  int code;
  int i = 0;
  char * query;
  sqlite3_stmt * statement;

  if(flight_number != NULL)
    query = "select * from cancellation where flight_number = ?;";
  else
    query = "select * from cancellation;";
  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  if(flight_number != NULL)
    sqlite3_bind_text(statement, 1, flight_number, -1, NULL);

  while((code = sqlite3_step(statement)) == SQLITE_ROW) {
    strcpy(cancellations[i].flight_number, (const char *)sqlite3_column_text(statement, 0));
    cancellations[i].seat_row = sqlite3_column_int(statement, 1);
    cancellations[i].seat_col = sqlite3_column_int(statement, 2);
    cancellations[i].dni = sqlite3_column_int(statement, 3);
    i++;
  }
  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE)
    return DB_ERROR;
  return DB_OK;
}

int db_book_seat(reservation_t * reservation)
{
  int code;
  int flight_dim[2];
  char * query;
  sqlite3_stmt * statement;

  db_get_flight_dim(reservation->flight_number, flight_dim);

  if(
    flight_dim[0] < reservation->seat_row ||
    flight_dim[1] < reservation->seat_col ||
    reservation->seat_row < 0 ||
    reservation->seat_col < 0
  )
    return DB_WRONG_RESULT;

  query = "insert into reservation values (?, ?, ?, ?);";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  sqlite3_bind_text(statement, 1, reservation->flight_number, -1, NULL);
  sqlite3_bind_int(statement, 2, reservation->seat_row);
  sqlite3_bind_int(statement, 3, reservation->seat_col);
  sqlite3_bind_int(statement, 4, reservation->dni);

  code = sqlite3_step(statement);
  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;

  return DB_OK;
}

int db_cancel_seat(reservation_t * reservation)
{
  int code;
  char * query;
  sqlite3_stmt * statement;

  if(!db_exists_reservation(reservation))
    return DB_WRONG_RESULT;

  query = "delete from reservation where flight_number = ? and seat_row = ? and seat_col = ? and dni = ?;";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK)
    return DB_ERROR;

  sqlite3_bind_text(statement, 1, reservation->flight_number, -1, NULL);
  sqlite3_bind_int(statement, 2, reservation->seat_row);
  sqlite3_bind_int(statement, 3, reservation->seat_col);
  sqlite3_bind_int(statement, 4, reservation->dni);

  code = sqlite3_step(statement);
  sqlite3_finalize(statement);

  if(code == SQLITE_ERROR)
    return DB_WRONG_RESULT;
  if(code != SQLITE_DONE && code != SQLITE_ROW)
    return DB_ERROR;

  query = "insert into cancellation values (?, ?, ?, ?);";

  code = sqlite3_prepare_v2(db_connection, query, -1, &statement, NULL);

  if(code != SQLITE_OK && code != SQLITE_ROW)
    return DB_ERROR;

  sqlite3_bind_text(statement, 1, reservation->flight_number, -1, NULL);
  sqlite3_bind_int(statement, 2, reservation->seat_row);
  sqlite3_bind_int(statement, 3, reservation->seat_col);
  sqlite3_bind_int(statement, 4, reservation->dni);

  code = sqlite3_step(statement);
  sqlite3_finalize(statement);

  return DB_OK;
}
