BINDIR=../../bin
OBJDIR=./
SRCDIR=../../src
MANDIR=../../man

CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic
LDFLAGS=-L$(BINDIR)
LIBS=-lprs_socket

TARGET=SNSH_client
INSTALLDIR?=/usr/local
OBJECTS=$(OBJDIR)/snsh_client.c.o $(OBJDIR)/debug.c.o $(OBJDIR)/socket.c.o

.PHONY: all linux
all: linux

linux: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

$(OBJDIR)/snsh_client.c.o: main.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/debug.c.o: $(SRCDIR)/debug.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/socket.c.o: $(SRCDIR)/prs_socket/socket.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean remove install uninstall
clean:
	rm -f *~ $(OBJECTS) $(BINDIR)/$(TARGET)

remove: clean
	rm -f $(BINDIR)/$(TARGET)

install:
	@echo Installing.
	cp $(BINDIR)/$(TARGET) $(INSTALLDIR)/bin/$(TARGET)
	cp $(MANDIR)/SNSH_client.8 /usr/share/man/man8/SNSH_client.8
	chown root:root /usr/share/man/man8/SNSH_client.8
	chmod 0644 /usr/share/man/man8/SNSH_client.8
	gzip /usr/share/man/man8/SNSH_client.8
	@echo done.

uninstall:
	@echo Uninstalling.
	rm -f $(INSTALLDIR)/bin/$(TARGET)
	rm -f /usr/share/man/man8/SNSH_client.8.gz
	@echo Done.

