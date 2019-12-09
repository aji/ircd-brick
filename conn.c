#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "brick.h"

struct irc_listener irc_listeners[MAX_IRC_LISTENERS];
size_t num_irc_listeners = 0;

size_t next_irc_conn = 0;
size_t irc_conns_used = 0;
struct irc_conn irc_conn_pool[MAX_LOCAL_IRC_CONNS];
struct irc_conn irc_conns_active = {
	.next = &irc_conns_active,
	.prev = &irc_conns_active,
};

static struct irc_conn *alloc_irc_conn(void) {
	struct irc_conn *added;
	int i, j;

	if (MAX_LOCAL_IRC_CONNS <= irc_conns_used) {
		return NULL;
	}

	added = NULL;
	for (i=0; i<MAX_LOCAL_IRC_CONNS; i++) {
		j = (next_irc_conn + i) % MAX_LOCAL_IRC_CONNS;
		if (irc_conn_pool[j].next == NULL) {
			added = irc_conn_pool + j;
			next_irc_conn = (j + 1) % MAX_LOCAL_IRC_CONNS;
			irc_conns_used++;
			break;
		}
	}

	if (added == NULL) {
		return NULL;
	}

	added->next = &irc_conns_active;
	added->prev = irc_conns_active.prev;
	added->next->prev = added;
	added->prev->next = added;

	return added;
}

static void release_irc_conn(struct irc_conn *conn) {
	if (conn->next != NULL) {
		conn->next->prev = conn->prev;
		conn->prev->next = conn->next;
		conn->next = NULL;
		conn->prev = NULL;
		irc_conns_used--;
	}
}

static void handle_irc_conn_input(void *param) {
	struct irc_conn *conn = param;
	ssize_t n;
	size_t m;
	struct rfc1459_message msg;

	if (conn->insize == MAX_IRC_CONN_INBUF) {
		char *msg = "ERROR :Inbuf full\r\n";
		write(conn->fd, msg, strlen(msg));
		close(conn->fd);
		deregister_fd(conn->fd);
		release_irc_conn(conn);
	}

	n = read(conn->fd,
		conn->inbuf + conn->insize,
		MAX_IRC_CONN_INBUF - conn->insize);

	if (n <= 0) {
		log_info("Connection closed fd=%d", conn->fd);
		close(conn->fd);
		deregister_fd(conn->fd);
		release_irc_conn(conn);
	}

	conn->insize += n;
	m = parse_rfc1459(&msg, conn->inbuf, conn->insize);
	if (m > 0) {
		conn->insize -= m;
		memmove(conn->inbuf, conn->inbuf+m, conn->insize);
	}
}

static struct irc_listener *add_listener(int fd) {
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

	struct irc_conn *conn = alloc_irc_conn();

	if (conn == NULL) {
		log_warn("Closing connection prematurely");
		const char *msg = "ERROR :No connection capacity\r\n";
		write(fd, msg, strlen(msg));
		close(fd);
		return;
	}

	conn->fd = fd;
	conn->insize = 0;

	register_fd(fd, handle_irc_conn_input, conn);
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
