OBJECTS = \
	conn.o \
	data.o \
	event-poll.o \
	log.o \
	main.o \
	message.o \
	verbs.o

CFLAGS = -Og

DEPS = $(OBJECTS:=.deps)

build: ircd-brick

include $(DEPS)

ircd-brick: $(OBJECTS)
	$(CC) -o $@ $^

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPS)
	rm -f verb-table.h
	rm -f gen-verbs
	rm -f ircd-brick

%.o.deps: %.c
	$(CC) $(CFLAGS) -M -MG -MF $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

verb-table.h: gen-verbs
	./gen-verbs > $@
