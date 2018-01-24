include ../Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
OBJECTS=$(SOURCE:.c=.o)

TESTS_SRC=$(wildcard tests/*.c)
TESTS=$(TESTS_SRC:.c=.o)

all: $(OBJECTS)

tests: $(OBJECTS) $(TESTS)
	$(CC) $(CC_FLAGS) $(OBJECTS) $(TESTS) -lsqlite3 -o database.test

$(OBJECTS): $(SOURCE) $(SOURCE_H)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(TESTS): $(TESTS_SRC)
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(TESTS) 2> /dev/null || true

.PHONY: tests all clean
