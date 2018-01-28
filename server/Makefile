include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
OBJECTS=$(SOURCE:.c=.o)
DB_OBJECTS=$(wildcard database/*.o)
EXECUTABLE=server

all: $(OBJECTS) database
	$(CC) $(CC_FLAGS) $(OBJECTS) $(DB_OBJECTS) -lsqlite3 -o $(EXECUTABLE)

database:
	cd database && make all;

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(EXECUTABLE) 2> /dev/null || true

.PHONY: database all clean
