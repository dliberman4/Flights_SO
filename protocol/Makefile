include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
OBJECTS=$(SOURCE:.c=.o)

all: $(OBJECTS)

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true

.PHONY: all clean
