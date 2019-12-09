#include <arpa/inet.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "brick.h"

struct irc_listener irc_listeners[MAX_IRC_LISTENERS];
size_t num_irc_listeners = 0;

struct irc_listener *add_listener(int fd) {
	struct irc_listener *listener;

	assert(num_irc_listeners < MAX_IRC_LISTENERS);
	listener = &irc_listeners[num_irc_listeners++];
	listener->fd = fd;

	return listener;
}

static void accept_ipv4_connection(void *param) {
	struct irc_listener *listener = param;
	struct sockaddr_in addr;
	socklen_t addrlen;
	char buf[BUFSIZE];
	int fd;

	fd = accept(listener->fd, (void*)&addr, &addrlen);
	perror_assert("accept", fd);
	inet_ntop(AF_INET, &addr.sin_addr.s_addr, buf, BUFSIZE);
	log_info("Accepted connection fd=%d addr=%s:%d",
		fd, buf, ntohs(addr.sin_port));
}

void start_ipv4_listener(uint16_t port) {
	int fd;
	int one = 1;
	struct sockaddr_in addr;
	struct irc_listener *listener;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	log_info("Listening on port %d", (int) port);

	perror_assert("socket",
		(fd = socket(AF_INET, SOCK_STREAM, 0)));
	perror_assert("bind",
		bind(fd, (void*)&addr, sizeof(addr)));
	perror_assert("setsockopt",
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)));
	perror_assert("listen",
		listen(fd, 5));

	listener = add_listener(fd);
	register_fd(fd, accept_ipv4_connection, listener);
}
