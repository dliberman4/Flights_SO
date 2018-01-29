include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
OBJECTS=$(SOURCE:.c=.o)
PROT_OBJECTS=$(wildcard ../protocol/*.o)
EXECUTABLE=client

all: $(OBJECTS) protocol
	$(CC) $(CC_FLAGS) $(OBJECTS) $(PROT_OBJECTS) -o $(EXECUTABLE)

protocol:
	cd ../protocol && make all;

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(EXECUTABLE) 2> /dev/null || true

.PHONY: all clean
