#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define __INCLUDE_VERB_TABLE_DEFINITION
#include "brick.h"

static uint8_t hash_verb(const uint8_t *s) {
	uint8_t hash;
	int i;

	for (i=0; i<VERB_SIZE; (*s ? s++ : s), i++) {
		hash += verb_table_param[i] * (*s);
	}

	return hash;
}

static bool equals_verb(const uint8_t *s, const uint8_t *verb) {
	int i;
	
	for (i=0; i<VERB_SIZE; (*s ? s++ : s), i++) {
		if (*s != verb[i]) {
			return false;
		}
	}
	
	return true;
}

const struct verb *find_verb(const uint8_t *s) {
	uint8_t idx = hash_verb(s) & VERB_TABLE_MASK;
	
	if (equals_verb(s, verb_table[idx].verb)) {
		return verb_table[idx].data;
	} else {
		return NULL;
	}
}

const struct verb verb_AWAY = { "marks you away" };
const struct verb verb_CAP = { "set client capabilities" };
const struct verb verb_CHALLENGE = { "used for SASL" };
const struct verb verb_CONNECT = { "oper command to do connections" };
const struct verb verb_HELP = { "user command to get help" };
const struct verb verb_INVITE = { "invite users to channels" };
const struct verb verb_JOIN = { "join channels" };
const struct verb verb_KICK = { "kick a user from a channel" };
const struct verb verb_KILL = { "disconnect a user from the network" };
const struct verb verb_LIST = { "list channels on the irc network" };
const struct verb verb_MAP = { "get a map of the irc network" };
const struct verb verb_PRIVMSG = { "send a message to a channel or user" };
const struct verb verb_NOTICE = { "send a notice to a channel or user" };
const struct verb verb_MODE = { "set the mode of a user or channel" };
const struct verb verb_NAMES = { "get the list of users in a channel" };
const struct verb verb_NICK = { "set your nickname" };
const struct verb verb_OPER = { "authenticate as an irc operator" };
const struct verb verb_PART = { "leave an irc channel "};
const struct verb verb_PASS = { "set a password when connecting" };
const struct verb verb_PING = { "pong!" };
const struct verb verb_QUIT = { "disconnect from IRC" };
const struct verb verb_TOPIC = { "set a channel topic" };
const struct verb verb_USER = { "set user parameters when connecting" };
const struct verb verb_WHO = { "get info about users in a channel" };
const struct verb verb_WHOIS = { "get info about a user" };
