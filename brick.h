#ifndef __INC_BRICK_H__
#define __INC_BRICK_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#define perror_assert(OP, X) ((X) < 0 ? (perror(OP), abort()) : 0)

#define BUFSIZE                          4096

/* Logger
   ---------------------------------------------------------------------- */

enum log_level {
	LOG_DEBUG      = 1,
	LOG_INFO       = 2,
	LOG_WARN       = 3,
	LOG_ERROR      = 4,
	LOG_FATAL      = 5,
};

extern enum log_level log_level;
extern void loggerf(enum log_level level, const char *fn, const char *fmt, ...);

#define log_debug(X...)     loggerf(LOG_DEBUG, __func__, X)
#define log_info(X...)      loggerf(LOG_INFO,  __func__, X)
#define log_warn(X...)      loggerf(LOG_WARN,  __func__, X)
#define log_error(X...)     loggerf(LOG_ERROR, __func__, X)
#define log_fatal(X...)     loggerf(LOG_FATAL, __func__, X)

/* Network and connection handling
   ---------------------------------------------------------------------- */

#define MAX_IRC_LISTENERS                   8

#define MAX_LOCAL_IRC_CONNS              2048
#define MAX_IRC_CONN_INBUF               1024

#define MAX_RFC1459_ARGS                   16

#define MAX_FDS (MAX_IRC_LISTENERS + MAX_LOCAL_IRC_CONNS)

struct irc_listener {
	int fd;
};

struct irc_conn {
	struct irc_conn *next, *prev;
	int fd;

	uint8_t inbuf[MAX_IRC_CONN_INBUF];
	size_t insize;
};

/* This struct represents an RFC 1459 message received from a client. Messages
   sent from clients to servers never include a source argument, so there is
   no pointer included to that portion of a message. The verb is the first
   argument. */
struct rfc1459_message {
	uint8_t *args[MAX_RFC1459_ARGS];
	unsigned nargs;
};

struct verb {
	char *tada;
};

#include "verb-table.h"

extern struct irc_listener irc_listeners[MAX_IRC_LISTENERS];
extern size_t num_irc_listeners;

extern struct irc_conn irc_conn_pool[MAX_LOCAL_IRC_CONNS];
extern struct irc_conn irc_conns_active;

extern void start_ipv4_listener(uint16_t port);

extern void register_fd(int fd, void (*cb)(void*), void*);
extern void deregister_fd(int fd);
extern void poll_fds_once(void);

/* Parses the buffer at [s, s+n) and fills m with the appropriate pointers.
   This function expects to find a nonempty sequence of \r and \n characters
   at the end of the buffer, which it interprets as a line ending. Returns
   the number of bytes read, including the line ending. Returns 0 if no line
   ending was found.

   The buffer will be modified, and m's fields will point directly into the
   buffer. Specifically, \0s will be inserted where necessary. */
extern size_t parse_rfc1459(struct rfc1459_message *m, uint8_t *s, size_t n);

extern const struct verb *find_verb(const uint8_t *s);

/* IRC server
   ---------------------------------------------------------------------- */

#define MAX_USERS                        8192
#define MAX_NICK_SIZE                      32
#define MAX_IDENT_SIZE                     32
#define MAX_GECOS_SIZE                    128

#define MAX_CHANNELS                     4096
#define MAX_CHAN_NAME_SIZE                128
#define MAX_TOPIC_SIZE                    256

#define MAX_CHANUSERS                   16384

#define UMODE_OPER                 0x00000001

#define CMODE_SECRET               0x00000001

#define CUMODE_OP                        0x80
#define CUMODE_VOICE                     0x01

struct user {
	uint32_t mode;
	struct user *next, *prev;

	uint8_t nick[MAX_NICK_SIZE];
	uint8_t ident[MAX_IDENT_SIZE];
	uint8_t gecos[MAX_GECOS_SIZE];

	struct chanuser *chanuser_head;
};

struct channel {
	uint32_t mode;
	struct channel *next, *prev;

	uint8_t name[MAX_CHAN_NAME_SIZE];
	uint8_t topic[MAX_TOPIC_SIZE];

	struct chanuser *chanuser_head;
};

struct chanuser {
	struct user *user;
	struct chan *chan;

	struct chanuser *next_by_user;
	struct chanuser *prev_by_user;

	struct chanuser *next_by_chan;
	struct chanuser *prev_by_chan;

	uint8_t mode;
};

extern bool ircd_running;

extern struct user user_pool[MAX_USERS];
extern struct channel channel_pool[MAX_CHANNELS];
extern struct chanuser chanuser_pool[MAX_CHANUSERS];

#endif
