include Makefile.inc

SOURCE=$(wildcard *.c)
SOURCE_H=$(wildcard *.h)
OBJECTS=$(SOURCE:.c=.o)
EXECUTABLE=client

all: $(OBJECTS)
	$(CC) $(CC_FLAGS) $(OBJECTS) -o $(EXECUTABLE)

$(OBJECTS): $(SOURCE) $(SOURCE_H)
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	-@rm $(OBJECTS) 2> /dev/null || true
	-@rm $(EXECUTABLE) 2> /dev/null || true

.PHONY: all clean
