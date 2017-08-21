#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../socket.h"

/* prototypes of functions */
int hdl_client(int *, struct sockaddr_in *, const char *);

/* main() - entry point for socket test 2.
 */
int main(void) {
	sockcreate_func_t sock_func;
	struct sockaddr_in client;
	int sockfd, clientfd, retval;

	/* initialize socket func */
	socket_init(SOCKET_BIND, &sock_func);
	/* bind socket to port */
	sockfd = sock_func.socket_bind("0.0.0.0", 5555, &clientfd, &client);
	/* handle server (swap to client handler) */
	retval = handle_server(&sockfd, &clientfd, &client, NULL, &hdl_client);
	/* cleanup socket */
	close_socket(&sockfd);
	return retval;
}

/* hdl_client() - main loop for handling client.
 */
int hdl_client(int *sockfd, struct sockaddr_in *client, const char *filename) {
	char msg[BUFSIZ];
	char buf[BUFSIZ];

	do {
		memset(msg, 0, sizeof msg);
		snprintf(msg, sizeof msg, "CMD >> ");
		if(send(*sockfd, msg, strlen(msg), 0) < 0) {
			perror("send");
			goto error;
		}
		memset(buf, 0, sizeof buf);
		if(recv(*sockfd, buf, sizeof buf, 0) < 0) {
			perror("recv");
			goto error;
		}
	} while(strncmp(buf, "exit\r\n", sizeof buf) != 0);
	return 0;

 error:
	return -1;
}
