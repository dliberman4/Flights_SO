include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
SOURCE_FRONTEND=$(wildcard frontend/*.c)
SOURCE_ACTIONS=$(wildcard actions/*.c)
OBJECTS=$(SOURCE:.c=.o)
OBJECTS_FRONTEND=$(SOURCE_FRONTEND:.c=.o)
OBJECTS_ACTIONS=$(SOURCE_ACTIONS:.c=.o)
PROT_OBJECTS=$(wildcard ../protocol/*.o)
EXECUTABLE=client

all: $(OBJECTS) $(OBJECTS_FRONTEND) $(OBJECTS_ACTIONS) protocol
	$(CC) $(CC_FLAGS) $(OBJECTS) $(OBJECTS_FRONTEND) $(OBJECTS_ACTIONS) $(PROT_OBJECTS) -o $(EXECUTABLE)

protocol:
	cd ../protocol && make all;

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(OBJECTS_FRONTEND) 2> /dev/null || true
	-@rm $(OBJECTS_ACTIONS) 2> /dev/null || true
	-@rm $(EXECUTABLE) 2> /dev/null || true

.PHONY: all clean protocol
