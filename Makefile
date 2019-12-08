OBJECTS = \
	data.o \
	verbs.o

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
	$(CC) -MM -MG -MF $@ $<

%.o: %.c
	$(CC) -c -o $@ $<

verb-table.h: gen-verbs
	./gen-verbs > $@
