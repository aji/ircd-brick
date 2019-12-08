#ifndef __BRICK_INC_DATA_H__
#define __BRICK_INC_DATA_H__

#include <stdint.h>

#define MAX_USERS                        8192
#define MAX_NICK_SIZE                      32
#define MAX_IDENT_SIZE                     32
#define MAX_GECOS_SIZE                    128

#define UMODE_OPER                 0x00000001

extern struct user {
	uint32_t mode;
	struct user *next, *prev;

	uint8_t nick[MAX_NICK_SIZE];
	uint8_t ident[MAX_IDENT_SIZE];
	uint8_t gecos[MAX_GECOS_SIZE];

	struct chanuser *chanuser_head;
} user_pool[MAX_USERS];

#define MAX_CHANNELS                     4096
#define MAX_CHAN_NAME_SIZE                128
#define MAX_TOPIC_SIZE                    256

#define CMODE_SECRET               0x00000001

extern struct channel {
	uint32_t mode;
	struct channel *next, *prev;

	uint8_t name[MAX_CHAN_NAME_SIZE];
	uint8_t topic[MAX_TOPIC_SIZE];

	struct chanuser *chanuser_head;
} channel_pool[MAX_CHANNELS];

#define MAX_CHANUSERS                   16384

#define CUMODE_OP                        0x80
#define CUMODE_VOICE                     0x01

extern struct chanuser {
	struct user *user;
	struct chan *chan;

	struct chanuser *next_by_user;
	struct chanuser *prev_by_user;

	struct chanuser *next_by_chan;
	struct chanuser *prev_by_chan;

	uint8_t mode;
} chanuser_pool[MAX_CHANUSERS];

#define MAX_IRC_CONNS                    8192
#define IRC_CONN_INBUF                   1024

extern struct irc_conn {
	struct conn *next, *prev;

	int fd;

	uint8_t inbuf[IRC_CONN_INBUF];
	size_t insize;
} irc_conn_pool[MAX_IRC_CONNS];

#endif
