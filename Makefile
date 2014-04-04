CC=clang
OPT=-Wall -Wextra -fPIC -c

all: malloc

debug: OPT += -g
debug: malloc

malloc: clean
	$(CC) $(OPT) -o $@.o $@.c && $(CC) -shared -o lib$@.so $@.o

clean:
	rm -f *.o *.so
