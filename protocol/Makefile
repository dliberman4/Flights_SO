include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
OBJECTS=$(SOURCE:.c=.o)

TESTS_SRC=$(wildcard tests/*.c)
TESTS=$(TESTS_SRC:.c=.o)

all: $(OBJECTS)

tests: $(OBJECTS) $(TESTS)
	$(CC) $(CC_FLAGS) $(OBJECTS) $(TESTS) -o serializer.test

%.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true

.PHONY: all clean
