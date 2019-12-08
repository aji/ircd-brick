#ifndef __INC_VERBS_H__
#define __INC_VERBS_H__

struct verb {
	uint8_t *tada;
};

#include "verb-table.h"

extern const struct verb *find_verb(const uint8_t *s);

#endif
