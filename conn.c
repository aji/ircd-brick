#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "brick.h"

struct irc_listener irc_listeners[MAX_IRC_LISTENERS];
size_t num_irc_listeners = 0;

void add_listener(int fd) {
	struct irc_listener *listener;

	assert(num_irc_listeners < MAX_IRC_LISTENERS);
	listener = &irc_listeners[num_irc_listeners++];
	listener->fd = fd;
}

void start_ipv4_listener(uint16_t port) {
	int fd;
	int one = 1;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	loggerf(LOG_INFO, "Listening on port %d", (int) port);

	perror_assert("socket",
		(fd = socket(AF_INET, SOCK_STREAM, 0)));
	perror_assert("bind",
		bind(fd, (void*)&addr, sizeof(addr)));
	perror_assert("setsockopt",
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)));
	perror_assert("listen",
		listen(fd, 5));

	add_listener(fd);
}
