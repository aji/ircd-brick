#include "brick.h"

int main(int argc, char *argv[]) {
	loggerf(LOG_INFO, "Starting up");
	start_ipv4_listener(6667);
	sleep(20);
}
