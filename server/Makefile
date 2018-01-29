include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
OBJECTS=$(SOURCE:.c=.o)
DB_OBJECTS=$(wildcard database/*.o)
PROT_OBJECTS=$(wildcard ../protocol/*.o)
EXECUTABLE=server

all: $(OBJECTS) database protocol
	$(CC) $(CC_FLAGS) $(OBJECTS) $(DB_OBJECTS) $(PROT_OBJECTS) -lsqlite3 -o $(EXECUTABLE)

database:
	cd database && make all;

protocol:
	cd ../protocol && make all;

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(EXECUTABLE) 2> /dev/null || true

.PHONY: database all clean
