create table flight(
  flight_number text primary key,
  plane_rows integer,
  plane_cols integer
);
create table reservation(
  flight_number text,
  seat_row integer,
  seat_col integer,
  dni integer,
  primary key (flight_number, seat_row, seat_col),
  foreign key (flight_number) references flight
);

create table cancellation(
  flight_number text,
  seat_row integer,
  seat_col integer,
  dni integer,
  primary key (flight_number, seat_row, seat_col, dni),
  foreign key (flight_number) references flight
);
