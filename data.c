#include "data.h"

struct user br_all_users[MAX_USERS];

uint16_t br_free_user = NIL16;
uint16_t br_next_user = 0;

struct chan br_all_chans[MAX_CHANS];

uint16_t br_free_chan = NIL16;
uint16_t br_next_chan = 0;


