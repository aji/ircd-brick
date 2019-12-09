#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "brick.h"

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
