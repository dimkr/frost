# compiler stuff
CC ?= cc
CFLAGS ?= -Wall

# installation stuff
PREFIX ?= /usr
SBIN_DIR ?= $(PREFIX)/sbin
MAN_DIR ?= $(PREFIX)/share/man

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

frost: frost.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -v -f frost *.o

install: frost
	install -v -m755 -D frost $(SBIN_DIR)/frost
	install -v -m644 -D frost.8 $(MAN_DIR)/man8/frost.8

uninstall:
	rm -vf $(SBIN_DIR)/frost
	rm -vf $(MAN_DIR)/man8/frost.8

