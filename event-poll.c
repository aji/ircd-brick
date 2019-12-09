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
	size_t center;

	if (min == max) {
		return min;
	}

	center = (min + max) / 2;

	if (pollfds[center].fd == fd) {
		return center;
	} else if (fd < pollfds[center].fd) {
		return find_fd_in(fd, min, center);
	} else {
		return find_fd_in(fd, center, max);
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

	memmove(pollfds+at, pollfds+at+1, nafter*sizeof(pollfds[0]));
	memmove(handlers+at, handlers+at+1, nafter*sizeof(handlers[0]));
	num_fds--;
}

void poll_fds_once(void) {
	int i, num_ready;

	assert(num_fds > 0);

	num_ready = poll(pollfds, num_fds, 1000);

	for (i=0; 0<num_ready && i<num_fds; i++) {
		if (pollfds[i].revents & POLLIN) {
			num_ready--;
			handlers[i].cb(handlers[i].data);
		}
	}
}
