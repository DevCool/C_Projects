SRCDIR=../../../src/prs_socket/tests
OBJDIR=../../../src/prs_socket/tests
BINDIR=../../../bin

CC=gcc
CFLAGS?=-std=gnu99 -Wall -pedantic
LDFLAGS?=-L$(BINDIR) -lprs_socket

SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.c.o)
LIST=$(BINDIR)/test1 $(BINDIR)/test2

.PHONY: all
all: $(LIST)

$(BINDIR)/test1: $(OBJDIR)/test1.c.o
	$(CC) $(LDFLAGS) $< -o $@

$(BINDIR)/test2: $(OBJDIR)/test2.c.o
	$(CC) $(LDFLAGS) $< -o $@

$(OBJECTS): %.c.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean remove clean-windows
clean:
	rm -f *~ $(OBJDIR)/*.o

remove: clean
	rm -f $(LIST)

clean-windows:
	@echo Cleaning up!
	@del *.o $(LIST)
	@echo Cleaning done.
