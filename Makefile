OBJECTS = \
	data.o \
	verbs.o

.DEFAULT: ircd-brick

ircd-brick: ${OBJECTS}
	${CC} -o $@ $^

clean:
	rm -f ${OBJECTS}
	rm -f verb-table.h
	rm -f gen-verbs

.c.o:
	${CC} -c -o $@ $^

verbs.c: verb-table.h

verb-table.h: gen-verbs
	./gen-verbs > $@
