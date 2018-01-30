include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
SOURCE_FRONTEND=$(wildcard frontend/*.c)
OBJECTS=$(SOURCE:.c=.o)
OBJECTS_FRONTEND=$(SOURCE_FRONTEND:.c=.o)
PROT_OBJECTS=$(wildcard ../protocol/*.o)
EXECUTABLE=client

all: $(OBJECTS) $(OBJECTS_FRONTEND) protocol
	$(CC) $(CC_FLAGS) $(OBJECTS) $(OBJECTS_FRONTEND) $(PROT_OBJECTS) -o $(EXECUTABLE)

protocol:
	cd ../protocol && make all;

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(OBJECTS_FRONTEND) 2> /dev/null || true
	-@rm $(EXECUTABLE) 2> /dev/null || true

.PHONY: all clean protocol
