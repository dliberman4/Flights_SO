#include <stdio.h>
#include <string.h>
#include "../../database/database.h"
#include "../../../protocol/types.h"

void test_db_open()
{
  if(db_open() == DB_OK)
    printf("db_open OK!\n");
  else
    printf("db_open not working :(\n");
  db_close(1);
}

void test_db_close()
{
  db_open();
  if(db_close(1) == DB_OK)
    printf("db_close OK!\n");
  else
    printf("db_close not working :(\n");
}

void test_db_new_flight()
{
  db_open();
  if(db_new_flight("A", 40, 7) == DB_OK)
    printf("db_new_flight OK!\n");
  else
    printf("db_new_flight not working :(\n");
  db_remove_flight("A");
  db_close(1);
}

void test_db_get_flight_dim()
{
  int aux[2];

  db_open();
  db_new_flight("BBB11", 50, 6);
  db_get_flight_dim("BBB11", aux);
  if(aux[0] != 50 || aux[1] != 6)
    printf("db_get_flight_dim not working :(\n");
  else
    printf("db_get_flight_dim OK!\n");
  db_remove_flight("BBB11");
  db_close(1);
}

void test_db_remove_flight()
{
  db_open();
  db_new_flight("CCC22", 40, 3);
  if(db_remove_flight("CCC22") == DB_OK)
    printf("db_remove_flight OK!\n");
  else
    printf("db_remove_flight not working :(\n");
  db_close(1);
}

void test_db_get_reservations_quantity()
{
  int quantity;
  reservation_t aux;

  strcpy(aux.flight_number, "A");
  aux.seat_row = 1;
  aux.seat_col = 1;
  aux.dni = 1;

  db_open();
  db_new_flight("A", 50, 10);
  db_book_seat(&aux);
  if((quantity=db_get_reservations_quantity("A")) == 1)
    printf("db_get_reservations_quantity OK!\n");
  else
    printf("db_get_reservations_quantity not working :( (got %d)\n", quantity);
  db_remove_flight("A");
  db_close(1);
}

void test_db_get_reservations()
{
  reservation_t aux[20];

  db_open();
  db_new_flight("A", 10, 10);
  if(db_get_reservations("A", aux) == DB_OK) {
    printf("db_get_reservations OK!\n");
  }
  else
    printf("db_get_reservations not working :(\n");
  db_remove_flight("A");
  db_close(1);
}

void test_db_get_cancellations_quantity()
{
  int quantity;

  reservation_t aux;

  strcpy(aux.flight_number, "A");
  aux.seat_row = 1;
  aux.seat_col = 1;
  aux.dni = 1;

  db_open();
  db_new_flight("A", 50, 10);
  db_book_seat(&aux);
  db_cancel_seat(&aux);
  if((quantity=db_get_cancellations_quantity("A")) == 1)
    printf("db_get_cancellations_quantity OK!\n");
  else
    printf("db_get_cancellations_quantity not working :( (got %d)\n", quantity);
  db_remove_flight("A");
  db_close(1);
}

void test_db_get_cancellations()
{
  reservation_t cancellations[1];

  reservation_t aux;

  strcpy(aux.flight_number, "A");
  aux.seat_row = 1;
  aux.seat_col = 1;
  aux.dni = 1;

  db_open();
  db_new_flight("A", 50, 10);
  db_book_seat(&aux);
  db_cancel_seat(&aux);

  if(db_get_cancellations("A", cancellations) == DB_OK)
    printf("db_get_cancellations OK!\n");
  else
    printf("db_get_cancellations not working :(\n");

  db_remove_flight("A");
  db_close(1);
}

void test_db_check_tables()
{
  db_open();
  if(db_check_tables() == DB_OK)
    printf("db_check_tables OK!\n");
  else
    printf("db_check_tables not working :(\n");
  db_close(1);
}

void test_db_book_seat()
{
  reservation_t aux;

  db_open();
  db_new_flight("A", 30, 10);
  strcpy(aux.flight_number, "A");
  aux.seat_row = 4;
  aux.seat_col = 4;
  aux.dni = 4;

  if(db_book_seat(&aux) == DB_OK)
    printf("db_book_seat OK!\n");
  else
    printf("db_book_seat not working :(\n");
  if(db_book_seat(&aux) == DB_OK)
    printf("db_book_seat duplicate not working :(\n");
  else
    printf("db_book_seat duplicate OK!\n");

  db_remove_flight("A");
  db_close(1);
}

void test_db_exists_reservation()
{
  reservation_t aux;

  db_open();
  db_new_flight("A", 30, 10);
  strcpy(aux.flight_number, "A");
  aux.seat_row = 4;
  aux.seat_col = 4;
  aux.dni = 4;
  db_book_seat(&aux);
  if(db_exists_reservation(&aux))
    printf("db_exists_reservation OK!\n");
  else
    printf("db_exists_reservation not working :(\n");

  db_remove_flight("A");
  db_close(1);
}

void test_db_cancel_seat()
{
  reservation_t aux;
  int code;

  db_open();
  db_new_flight("A", 30, 10);
  strcpy(aux.flight_number, "A");
  aux.seat_row = 4;
  aux.seat_col = 4;
  aux.dni = 4;
  db_book_seat(&aux);
  if((code = db_cancel_seat(&aux)) == DB_OK)
    printf("db_cancel_seat OK!\n");
  else
    printf("db_cancel_seat not working :( (error = %d)\n", code);

  db_remove_flight("A");
  db_close(1);
}

int main()
{
  test_db_open();
  test_db_check_tables();
  test_db_new_flight();
  test_db_get_flight_dim();
  test_db_remove_flight();
  test_db_get_reservations_quantity();
  test_db_get_reservations();
  test_db_get_cancellations();
  test_db_get_cancellations_quantity();
  test_db_book_seat();
  test_db_exists_reservation();
  test_db_cancel_seat();
  test_db_close();
}
