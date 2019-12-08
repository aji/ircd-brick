#ifndef __BRICK_INC_DATA_H__
#define __BRICK_INC_DATA_H__

#include <stdint.h>

#define NIL16                          0xffff

#define MAX_USERS                        8192
#define MAX_NICK_SIZE                      32
#define MAX_IDENT_SIZE                     32
#define MAX_GECOS_SIZE                    128

#define UMODE_OPER                 0x00000001

#define CUMODE_OP                        0x80
#define CUMODE_VOICE                     0x01

extern struct user {
	uint32_t mode;
	uint16_t next;
	
	uint8_t nick[MAX_NICK_SIZE];
	uint8_t ident[MAX_IDENT_SIZE];
	uint8_t gecos[MAX_GECOS_SIZE];

	uint16_t chanuser;
} br_all_users[MAX_USERS];

extern uint16_t br_free_user;
extern uint16_t br_next_user;

#define MAX_CHANS                        4096
#define MAX_CHAN_NAME_SIZE                128
#define MAX_TOPIC_SIZE                    256

#define CMODE_SECRET               0x00000001

extern struct chan {
	uint32_t mode;
	uint16_t next;

	uint8_t name[MAX_CHAN_NAME_SIZE];
	uint8_t topic[MAX_TOPIC_SIZE];

	uint16_t chanuser;
} br_all_chans[MAX_CHANS];

extern uint16_t br_free_chan;
extern uint16_t br_next_chan;

#define MAX_CHANUSERS                   16384

extern struct chanuser {
	uint16_t next_by_user;
	uint16_t prev_by_user;

	uint16_t next_by_chan;
	uint16_t prev_by_chan;
	
	uint8_t mode;
} br_all_chanusers[MAX_CHANUSERS];

extern uint16_t br_free_chanuser;
extern uint16_t br_next_chanuser;

#define MAX_LOCAL_CONNS                  8192

#endif
