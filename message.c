#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "message.h"

size_t parse_rfc1459(struct rfc1459_message *m, uint8_t *s, size_t n) {
	size_t i = 0;
	m->nargs = 0;

start:
	for (; i<n; i++) {
		switch (s[i]) {
		case '\r':  goto line_ending;
		case '\n':  goto line_ending;
		case ' ':   break;
		case ':':   goto skip_source;
		default:    goto end_arg;
		}
	}
	goto unexpected_end_of_buffer;

skip_source:
	for (; i<n; i++) {
		switch (s[i]) {
		case '\r':  goto line_ending;
		case '\n':  goto line_ending;
		default:    break;
		case ' ':   goto find_arg;
		}
	}
	goto unexpected_end_of_buffer;

find_arg:
	for (; i<n; i++) {
		switch (s[i]) {
		case '\r':  goto line_ending;
		case '\n':  goto line_ending;
		case ' ':   break;
		case ':':   goto end_trailing;
		default:    goto end_arg;
		}
	}
	goto unexpected_end_of_buffer;

end_arg:
	if (m->nargs < MAX_RFC1459_ARGS) {
		m->args[m->nargs++] = s + i;
	}
	for (; i<n; i++) {
		switch (s[i]) {
		case '\r':  goto line_ending;
		case '\n':  goto line_ending;
		default:    break;
		case ' ':   s[i++] = '\0'; goto find_arg;
		}
	}
	goto unexpected_end_of_buffer;

end_trailing:
	i++; // skip ':'
	if (m->nargs < MAX_RFC1459_ARGS) {
		m->args[m->nargs++] = s + i;
	}
	for (; i<n; i++) {
		switch (s[i]) {
		case '\r':  goto line_ending;
		case '\n':  goto line_ending;
		default:    break;
		}
	}
	goto unexpected_end_of_buffer;

line_ending:
	for (; i<n; i++) {
		switch (s[i]) {
		case '\r':  s[i] = '\0'; break;
		case '\n':  s[i] = '\0'; break;
		default:    return i;
		}
	}
	return i;

unexpected_end_of_buffer:
	return 0;
}

int main(int argc, char *argv[]) {
	uint8_t buffer[512];
	size_t n, i, j, k;
	struct rfc1459_message m;

	if (argc < 2) {
		printf("need arg!\n");
		return 1;
	}

	n = strlen(argv[1]);
	n = n > 512 ? 512 : n;
	memcpy(buffer, argv[1], n);

	printf("buffer =\n");
	for (i=0; i<512; i+=16) {
		printf("  ");
		for (j=0; j<16; j++) {
			printf(" %02x", buffer[i+j]);
		}
		printf("  ");
		for (j=0; j<16; j++) {
			uint8_t c = buffer[i+j];
			printf("%c", isprint(c) ? c : '.');
		}
		printf("\n");
	}

	printf("n = %zu\n", n);

	i = parse_rfc1459(&m, buffer, n);
	printf("i = %zu\n", i);

	printf("buffer =\n");
	for (i=0; i<512; i+=16) {
		printf("  ");
		for (j=0; j<16; j++) {
			printf(" %02x", buffer[i+j]);
		}
		printf("  ");
		for (j=0; j<16; j++) {
			uint8_t c = buffer[i+j];
			printf("%c", isprint(c) ? c : '.');
		}
		printf("\n");
	}

	printf("nargs = %u\n", m.nargs);

	for (j=0; j<MAX_RFC1459_ARGS; j++) {
		printf("args[%2zu] = ", j);
		if (0 < i && j < m.nargs) {
			printf("%s$\n", m.args[j]);
		} else {
			printf("%p\n", m.args[j]);
		}
	}

	return 0;
}
