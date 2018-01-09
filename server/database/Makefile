include ../Makefile.inc

SOURCE=$(wildcard *.c)
OBJECTS=$(SOURCE:.c=.o)

TESTS_SRC=$(wildcard tests/*.c)
TESTS=$(TESTS_SRC:.c=.o)

all: $(OBJECTS)

tests: $(OBJECTS) $(TESTS)
	$(CC) $(CC_FLAGS) $(OBJECTS) $(TESTS) -lsqlite3 -o database.test

$(OBJECTS): $(SOURCE)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(TESTS): $(TESTS_SRC)
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	rm $(OBJECTS)
	rm $(TESTS)
