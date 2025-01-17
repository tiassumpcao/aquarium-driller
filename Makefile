CC = gcc
CFLAGS = -I./include/ -Wall -g3 -ggdb -O0
LDFLAGS =

AD_INCLUDE_DIR = ./include
INCLUDE_DIR = /usr/include
LIB_DIR = /usr/lib

SOURCES = interesting-numbers.c memory-maps.c random.c random-page.c variable-generation.c\
		  random-address.c random-length.c seed.c aquarium-driller.c
LIBAD = libad.a
OBJECTS = $(SOURCES:.c=.o)
TESTALL = test-all
TESTCOMPLEXTYPES = test-complex-types
UNITTESTS = $(TESTALL) $(TESTCOMPLEXTYPES)

all: $(SOURCES) $(TESTALL) $(TESTCOMPLEXTYPES) $(LIBAD)

install:
	cp -r $(AD_INCLUDE_DIR)/* $(INCLUDE_DIR)/
	cp $(LIBAD) $(LIB_DIR)
	ldconfig

clean:
	rm -f *.o $(UNITTESTS) $(LIBAD)
	rm -f $(LIB_DIR)/$(LIBAD)
	rm -rf $(INCLUDE_DIR)/aquarium-driller

$(TESTALL): $(OBJECTS)
	$(CC) $(CFLAGS) test-all.c $^ $(LDFLAGS) -o $@

$(TESTCOMPLEXTYPES): $(OBJECTS)
	$(CC) $(CFLAGS) test-complex-types.c $^ $(LDFLAGS) -o $@

$(LIBAD): $(OBJECTS)
	ar rcs $@ $^

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

