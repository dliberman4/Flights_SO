#include <stdio.h>
#include <string.h>
#include "../../database/database.h"
#include "../../types.h"

void test_db_open()
{
  if(db_open() == DB_OK)
    printf("db_open OK!\n");
  else
    printf("db_open not working :(\n");
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
  if(db_new_flight("AAA23", 40, 7) == DB_OK)
    printf("db_new_flight OK!\n");
  else
    printf("db_new_flight not working :(\n");
}

void test_db_get_flight_dim()
{
  int aux[2];

  db_new_flight("BBB11", 50, 6);
  db_get_flight_dim("BBB11", aux);
  if(aux[0] != 50 || aux[1] != 6)
    printf("db_get_flight_dim not working :(\n");
  else
    printf("db_get_flight_dim OK!\n");
}

void test_db_remove_flight()
{
  db_new_flight("CCC22", 40, 3);
  if(db_remove_flight("CCC22") == DB_OK)
    printf("db_remove_flight OK!\n");
  else
    printf("db_remove_flight not working :(\n");
}

void test_db_get_reservations_quantity()
{
  int quantity;

  if((quantity=db_get_reservations_quantity("AAA23")) == 3)
    printf("db_get_reservations_quantity OK!\n");
  else
    printf("db_get_reservations_quantity not working :( (got %d)\n", quantity);
}

void test_db_get_reservations()
{
  reservation_t aux[20];
  if(db_get_reservations("AAA23", aux) == DB_OK) {
    printf("db_get_reservations OK!\n");
    printf("%s, %d, %d\n", aux[0].flight_number, aux[0].seat_row, aux[0].seat_col);
  }
  else
    printf("db_get_reservations not working :(\n");
}

void test_db_check_tables()
{
  if(db_check_tables() == DB_OK)
    printf("db_check_tables OK!\n");
  else
    printf("db_check_tables not working :(\n");
}

void test_db_book_seat()
{
  reservation_t aux;

  strcpy(aux.flight_number, "AAA23");
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
}

void test_db_exists_reservation()
{
  reservation_t aux;

  strcpy(aux.flight_number, "AAA23");
  aux.seat_row = 4;
  aux.seat_col = 4;
  aux.dni = 4;
  db_book_seat(&aux);
  if(db_exists_reservation(&aux))
    printf("db_exists_reservation OK!\n");
  else
    printf("db_exists_reservation not working :(\n");
}

void test_db_cancel_seat()
{
  reservation_t aux;
  int code;

  strcpy(aux.flight_number, "AAA23");
  aux.seat_row = 4;
  aux.seat_col = 4;
  aux.dni = 4;
  db_book_seat(&aux);
  if((code = db_cancel_seat(&aux)) == DB_OK)
    printf("db_cancel_seat OK!\n");
  else
    printf("db_cancel_seat not working :( (error = %d)\n", code);
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
  test_db_book_seat();
  test_db_exists_reservation();
  test_db_cancel_seat();
  test_db_close();
}
