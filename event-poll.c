#include <assert.h>
#include <poll.h>
#include <string.h>

#include "brick.h"

struct handler_info {
	void (*cb)(void*);
	void *data;
};

static struct handler_info handlers[MAX_FDS];
static struct pollfd pollfds[MAX_FDS];
static size_t num_fds = 0;

static size_t find_fd_in(int fd, size_t min, size_t max) {
	size_t i;

	if (min == max) {
		return min;
	} else if (max < min + 4) {
		for (i=min; i<max; i++) {
			if (fd <= pollfds[i].fd)
				break;
		}
		return i;
	}

	i = (min + max) / 2;

	if (pollfds[i].fd == fd) {
		return i;
	} else if (pollfds[i].fd < fd) {
		return find_fd_in(fd, i, max);
	} else {
		return find_fd_in(fd, min, i);
	}
}

static size_t find_fd(int fd) {
	if (num_fds > 0 && fd > pollfds[num_fds-1].fd) {
		return num_fds;
	}
	return find_fd_in(fd, 0, num_fds);
}

void register_fd(int fd, void (*cb)(void*), void *data) {
	size_t at = find_fd(fd);
	size_t nafter = num_fds - at;

	if (at != num_fds && pollfds[at].fd == fd) {
		log_warn("Re-registering file descriptor! "
			"fd=%d at=%d next(cb=%p data=%p) "
			"prev(cb=%p data=%p)",
			fd, at, cb, data,
			handlers[at].cb, handlers[at].data);
	} else {
		assert(num_fds < MAX_FDS);
		memmove(pollfds+at+1, pollfds+at, nafter*sizeof(pollfds[0]));
		memmove(handlers+at+1, handlers+at, nafter*sizeof(handlers[0]));
		num_fds++;
	}

	handlers[at].cb = cb;
	handlers[at].data = data;
	pollfds[at].fd = fd;
	pollfds[at].events = POLLIN;

	log_info("Registered file descriptor at=%d fd=%d cb=%p data=%p",
		at, fd, cb, data);
}

void deregister_fd(int fd) {
	size_t at = find_fd(fd);
	size_t nafter = num_fds - at - 1;

	if (num_fds <= at || pollfds[at].fd != fd) {
		log_warn("Attempted to deregister a file descriptor "
			"that wasn't registered! fd=%d", fd);
		return;
	}

	log_info("Deregistering file descriptor at=%d fd=%d cb=%p data=%p",
		at, fd, handlers[at].cb, handlers[at].data);

	memmove(pollfds+at, pollfds+at+1, nafter*sizeof(pollfds[0]));
	memmove(handlers+at, handlers+at+1, nafter*sizeof(handlers[0]));
	num_fds--;
}

void poll_fds_once(void) {
	int i, num_ready;

	log_info("polling:");
	for (i=0; i<num_fds; i++) {
		log_info("  [%d] fd=%d cb=%p data=%p",
			i, pollfds[i].fd, handlers[i].cb, handlers[i].data);
	}

	assert(num_fds > 0);
	num_ready = poll(pollfds, num_fds, 1000);

	if (num_ready < 0) {
		perror("poll");
	} else if (num_ready == 0) {
		return;
	}

	/* The tables can change while we're handling an event, but it's
	   rare enough to encounter multiple simultaneous events that we
	   choose the simplicity of simply polling a second time, by
	   breaking out of the loop as soon as we handle one event. */
	for (i=0; i<num_fds; i++) {
		if (pollfds[i].revents & POLLIN) {
			handlers[i].cb(handlers[i].data);
			break;
		}
	}
}
