CC=gcc
CFLAGS=-fPIC -Wall -W
LDFLAGS=-shared -Wl,-soname,libchmod.so
LDLIBS=-ldl

all: libchmod.so

%.so: %.o Makefile
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f *.so *.o

install: all
	install libchmod.so /usr/local/lib
	ldconfig
