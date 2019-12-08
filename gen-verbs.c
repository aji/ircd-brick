/* This file is a stand-alone program that generates a static hash table for
   IRC verbs. It generates hash functions at random until it finds one that
   can hash all the valid verb names without introducing any collisions, then
   it writes a .h file with this information. The generated tables are used
   by verbs.c */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define VERB_SIZE 12

static const uint8_t verbs[][VERB_SIZE] = {
	"AWAY",
	"CAP",
	"CHALLENGE",
	"CONNECT",
	"HELP",
	"INVITE",
	"JOIN",
	"KICK",
	"KILL",
	"LIST",
	"MAP",
	"PRIVMSG",
	"NOTICE",
	"MODE",
	"NAMES",
	"NICK",
	"OPER",
	"PART",
	"PASS",
	"PING",
	"QUIT",
	"TOPIC",
	"USER",
	"WHO",
	"WHOIS",
	""
};

static unsigned attempts = 0;
static uint8_t param[VERB_SIZE];

#define TABLE_BITS 6
#define TABLE_SIZE (1<<TABLE_BITS)
#define TABLE_MASK (TABLE_SIZE-1)

static const uint8_t *table[TABLE_SIZE];

static uint8_t hash_verb(const uint8_t *verb) {
	uint8_t hash = 0;
	int i;

	for (i=0; i<VERB_SIZE; i++) {
		hash += param[i] * verb[i];
	}

	return hash & TABLE_MASK;
}

static void choose_params(void) {
	int i;
	
	for (i=0; i<VERB_SIZE; i++) {
		attempts++;
		param[i] = rand() | 1; // force odd value
	}
}

static bool has_collisions(void) {
	uint8_t hash;
	int i;
	
	for (i=0; i<TABLE_SIZE; i++) {
		table[i] = NULL;
	}

	for (i=0; verbs[i][0]; i++) {
		hash = hash_verb(verbs[i]);
		if (table[hash] != NULL) {
			return true;
		} else {
			table[hash] = verbs[i];
		}
	}

	return false;
}

void emit_verbs_h() {
	int i;

	printf("#ifndef __INC_VERB_TABLE_H__\n");
	printf("#define __INC_VERB_TABLE_H__\n");
	printf("\n");
	printf("#include <stdint.h>\n");
	printf("\n");
	printf("#define VERB_SIZE %d\n", VERB_SIZE);
	printf("#define VERB_TABLE_BITS %d\n", TABLE_BITS);
	printf("#define VERB_TABLE_MASK %d\n", TABLE_MASK);
	printf("#define VERB_TABLE_SIZE %d\n", TABLE_SIZE);
	printf("\n");
	for (i=0; verbs[i][0]; i++) {
		printf("extern const struct verb verb_%s;\n", verbs[i]);
	}
	printf("\n");
	printf("extern const struct verb_table {\n");
	printf("\tconst uint8_t verb[VERB_SIZE];\n");
	printf("\tconst struct verb *data;\n");
	printf("} verb_table[VERB_TABLE_SIZE];\n");
	printf("\n");
	printf("extern const uint8_t verb_table_param[VERB_SIZE];\n");
	printf("\n");
	printf("#ifdef __INCLUDE_VERB_TABLE_DEFINITION\n");
	printf("const struct verb_table verb_table[VERB_TABLE_SIZE] = {\n");
	for (i=0; i<TABLE_SIZE; i++) {
		if (table[i]) {
			printf("\n{ .verb = \"%s\", .data = &verb_%s },",
			       table[i], table[i]);
		} else {
			printf("{},");
		}
	}
	printf("};\n");
	printf("const uint8_t verb_table_param[VERB_SIZE] =\n{ ");
	for (i=0; i<VERB_SIZE; i++) {
		printf("0x%02x, ", param[i]);
	}
	printf("};\n");
	printf("#endif\n");
	printf("\n");
	printf("#endif\n");
}

int main(int argc, char *argv[]) {
	fprintf(stderr, "Finding verb table parameters...\n");
	srand(time(NULL));
	do { choose_params(); } while (has_collisions());
	fprintf(stderr, "Took %d attempts\n", attempts);
	fprintf(stderr, "Writing verb table...\n");
	emit_verbs_h();
}
