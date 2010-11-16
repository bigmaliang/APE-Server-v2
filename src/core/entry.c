#include "common.h"
#include "buffer.h"
#include "string.h"
#include "events.h"

#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "hash.h"
#include "socket.h"
#include "events_loop.h"
#include "server.h"
#include "dns.h"
#include "modules.h"

ape_global *ape_init()
{
	ape_global *ape;
	struct _fdevent *fdev;
	
	if ((ape = malloc(sizeof(*ape))) == NULL) return NULL;
		
	signal(SIGPIPE, SIG_IGN);
	
	fdev = &ape->events;
	fdev->handler = EVENT_UNKNOWN;
	#ifdef USE_EPOLL_HANDLER
	fdev->handler = EVENT_EPOLL;
	#endif
	#ifdef USE_KQUEUE_HANDLER
	fdev->handler = EVENT_ KQUEUE;
	#endif
	
	ape->basemem 	= APE_BASEMEM;
	ape->is_running = 1;

	if (ape_dns_init(ape) != 0) {
		free(ape);
		return NULL;
	}
	
	ape->seed = time(NULL) ^ (getpid() << 16);
	
	events_init(ape);
	
	return ape;
}

int dns_cb(const char *ip)
{
	printf("Callback : %s\n", ip);
}

int main(int argc, char **argv)
{
	ape_global *ape;
	uint64_t h;

	int z = 0;
	
	if ((ape = ape_init()) == NULL) {
		printf("Failed to init APE\n");
		exit(1);
	}
	
	printf("    _    ____  _____   ____    ___  \n");
	printf("   / \\  |  _ \\| ____| |___ \\  / _ \\ \n");
	printf("  / _ \\ | |_) |  _|     __) || | | |\n");
	printf(" / ___ \\|  __/| |___   / __/ | |_| |\n");
	printf("/_/   \\_\\_|   |_____| |_____(_)___/ \n\t   Async Push Engine (%s)\n\n", __REV);
	printf("Build   : %s %s\n", __DATE__, __TIME__);
	printf("Author  : Anthony Catel (a.catel@weelya.com)\n\n");
	
	
	h = hash("fop", 3, ape->seed);

	
	ape_server_init(6969, "127.0.0.1", ape);
	
	for(z = 0; ape_modules[z]; z++) {
		if (ape_modules[z]->ape_module_init(ape) == 0) {
			printf("[Module] %s loaded\n", ape_modules[z]->name);
		} else {
			printf("[Module] Failed to load %s\n", ape_modules[z]->name);
		}
	}
	
	#if 0	
	for (z = 0; z < 500; z++) {
		ape_gethostbyname("lya.eu", dns_cb, ape);
	}
	#endif

	
	events_loop(ape);
	
	return 0;
}
