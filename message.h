#ifndef __INC_MESSAGE_H__
#define __INC_MESSAGE_H__

#include <stdint.h>

#define MAX_RFC1459_ARGS 16

/* This struct represents an RFC 1459 message received from a client. Messages
   sent from clients to servers never include a source argument, so there is
   no pointer included to that portion of a message. The verb is the first
   argument. */
struct rfc1459_message {
	uint8_t *args[MAX_RFC1459_ARGS];
	unsigned nargs;
};

/* Parses the buffer at [s, s+n) and fills m with the appropriate pointers.
   This function expects to find a nonempty sequence of \r and \n characters
   at the end of the buffer, which it interprets as a line ending. Returns
   the number of bytes read, including the line ending. Returns 0 if no line
   ending was found.

   The buffer will be modified, and m's fields will point directly into the
   buffer. Specifically, \0s will be inserted where necessary. */
extern size_t parse_rfc1459(struct rfc1459_message *m, uint8_t *s, size_t n);

#endif
