#include <poll.h>

#include "brick.h"

bool ircd_running = true;

int main(int argc, char *argv[]) {
	log_info("Starting up");
	start_ipv4_listener(6667);

	log_info("Entering event loop");
	while (ircd_running) {
		poll_fds_once();
	}

	log_info("Goodbye...");
	return 0;
}
