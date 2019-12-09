#include "brick.h"

bool ircd_running = true;

int main(int argc, char *argv[]) {
	loggerf(LOG_INFO, "Starting up");
	start_ipv4_listener(6667);

	loggerf(LOG_INFO, "Entering event loop");
	while (ircd_running) {
		poll_fds_once();
	}

	loggerf(LOG_INFO, "Goodbye...");
	return 0;
}
