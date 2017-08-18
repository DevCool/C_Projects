/*********************************************************
 * main.c - SNSH Client main source file.
 *********************************************************
 * Created by Philip "5n4k3" Simonson          (2017)
 *********************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../debug.h"
#include "../prs_socket/socket.h"

int hdl_client(int *, struct sockaddr_in *, const char *);

int main(int argc, char *argv[]) {
  sockcreate_func_t sockfunc;
  struct sockaddr_in client;
  int sockfd, clientfd, retval;

  if(argc < 2 || argc > 3) {
    printf("Usage: %s <ipaddress> [port]\n", argv[0]);
    return 1;
  }

  if(argc == 2) {
    ERROR_FIXED(socket_init(SOCKET_CONN, &sockfunc) < 0, "socket init failed.\n");
    ERROR_FIXED((sockfd = create_conn(argv[1], 8888, &clientfd, &client)) < 0,
		"Could not create socket.\n");
    retval = handle_server(&sockfd, &clientfd, &client, NULL, &hdl_client);
  } else {
    ERROR_FIXED(socket_init(SOCKET_CONN, &sockfunc) < 0, "Socket init failed.\n");
    ERROR_FIXED((sockfd = create_conn(argv[1], atoi(argv[2]), &clientfd, &client)) < 0,
		"Could not create socket.\n");
    retval = handle_server(&sockfd, &clientfd, &client, NULL, &hdl_client);
  }
  close_socket(&sockfd);
  return retval;

 error:
  close_socket(&sockfd);
  return 1;
}

#define COMPLETE 0
#define DATALEN 1024

int recv_splash(int sd) {
	char splash[1024];
	int bytes;
	
	do {
		memset(splash, 0, sizeof splash);
		bytes = recvfrom(sd, splash, sizeof(splash)-1, 0, NULL, 0);
		if(bytes > 0) {
			splash[bytes] = 0;
			printf("%s", splash);
		} else {
			return -1;
		}
	} while(bytes);
	return 0;
}

int hdl_client(int *sockfd, struct sockaddr_in *client, const char *filename) {
  char msg[DATALEN];
  char buf[BUFSIZ];
  unsigned int bytes;
  socklen_t addrlen;
  int ret;

  ERROR_FIXED(recv_splash(*sockfd) < 0, "Error: Could not receive splash screen.\n");
  fflush(stdout);
  
  while(1) {
    memset(msg, 0, sizeof msg);
    ERROR_FIXED((ret = recvfrom(*sockfd, msg, sizeof(msg)-1, 0,
	  (struct sockaddr *)client, &addrlen)) < 0,
	  "Could not recv data.\n");
    if(ret == 0) {
		puts("Connection closed.");
		break;
    } else {
		printf("%s", msg);
		fflush(stdout);
    }
    memset(buf, 0, sizeof buf);
	if(fgets(buf, sizeof buf, stdin) != NULL) {
		ERROR_FIXED((bytes = sendto(*sockfd, buf, strlen(buf), 0,
			(struct sockaddr *)client, addrlen)) != strlen(buf),
			"Could not send data.\n");
		if(bytes == 0) {
		puts("Connection closed.");
		break;
		}
	} else {
		puts("Error: Could not get a line from standard input.");
		break;
	}
  }
  close_socket(sockfd);
  return 0;

 error:
  close_socket(sockfd);
  return 1;
}
