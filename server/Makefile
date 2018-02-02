include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
SOURCE_LOG=$(wildcard log/*.c)
SOURCE_ACTIONS=$(wildcard actions/*.c)
SOURCE_UTILS=$(wildcard utils/*.c)
OBJECTS=$(SOURCE:.c=.o)
DB_OBJECTS=$(wildcard database/*.o)
OBJECTS_LOG=$(SOURCE_LOG:.c=.o)
OBJECTS_ACTIONS=$(SOURCE_ACTIONS:.c=.o)
OBJECTS_UTILS=$(SOURCE_UTILS:.c=.o)
PROT_OBJECTS=$(wildcard ../protocol/*.o)
EXECUTABLE=server

all: $(OBJECTS) $(OBJECTS_LOG) $(OBJECTS_ACTIONS) $(OBJECTS_UTILS) database protocol
	$(CC) $(CC_FLAGS) $(OBJECTS) $(DB_OBJECTS) $(PROT_OBJECTS) $(OBJECTS_LOG) $(OBJECTS_ACTIONS) $(OBJECTS_UTILS) -lsqlite3 -pthread -o $(EXECUTABLE)

database:
	cd database && make all;

protocol:
	cd ../protocol && make all;

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(OBJECTS_LOG) 2> /dev/null || true
	-@rm $(OBJECTS_ACTIONS) 2> /dev/null || true
	-@rm $(OBJECTS_UTILS) 2> /dev/null || true
	-@rm $(EXECUTABLE) 2> /dev/null || true

.PHONY: database all clean protocol
