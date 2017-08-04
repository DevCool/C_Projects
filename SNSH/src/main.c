/**************************************************************************
 * main.c - main program is right in this file, though it doesn't contain *
 *          much. but you should look at it anyway, learning from this    *
 *          project is what I want anyone (who wants to learn) to be able *
 *          to learn from this. That's what I made it for.                *
 **************************************************************************
 * Created by Philip "5n4k3" Simonson                (2017)               *
 **************************************************************************
 */

/* standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#ifdef __linux
#include <fcntl.h>
#endif

#include "prs_socket/socket.h"
#include "helper.h"
#include "debug.h"
#include "snshimg.h"

/* function pointer to handle clients */
int hdl_client(int *sockfd, struct sockaddr_in *client, const char *filename);

/* main() - entry point for program.
 */
int main(int argc, char *argv[]) {
  sockcreate_func_t sock_funcs;
  struct sockaddr_in client;
  int sockfd, clientfd, retval;

  if(argc != 2) {
    printf("Usage: %s <ip-address>\n", argv[0]);
    return 1;
  }

  ERROR_FIXED(socket_init(SOCKET_BIND, &sock_funcs) < 0, "Could not initialize socket funcs.");
  sockfd = sock_funcs.socket_bind(argv[1], 0, &clientfd, &client);
#ifdef __linux
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
#endif
  retval = handle_server(&sockfd, &clientfd, &client, NULL, &hdl_client);
  close_socket(&sockfd);
  return retval;

 error:
  close_socket(&sockfd);
  return -1;
}

/* hdl_client() - handles connect client.
 */
int hdl_client(int *sockfd, struct sockaddr_in *client, const char *filename) {
#if defined(_WIN32) || (_WIN64)
  FD_SET rd;
#else
  fd_set rd;
#endif
  char msg[256];
  char *password1 = "CODE187\n";
  char *password2 = "CODE19\r\n";
  char entry[9];
  socklen_t addrlen = sizeof(*client);

  ERROR_FIXED(sendto(*sockfd, SNSH_IMGDATA, strlen(SNSH_IMGDATA), 0,
		     (struct sockaddr *)client, addrlen) != strlen(SNSH_IMGDATA),
	      "Could not send all img data.\n");
  do {
    FD_ZERO(&rd);
    FD_SET(*sockfd, &rd);

    if(select(*sockfd+1, &rd, NULL, NULL, NULL) < 0) {
      perror("select");
      goto error;
    }
    if(FD_ISSET(*sockfd, &rd)) {
      memset(msg, 0, sizeof msg);
      snprintf(msg, sizeof msg, "Enter password: ");
      ERROR_FIXED(sendto(*sockfd, msg, strlen(msg), 0, (struct sockaddr *)client,
			 addrlen) != strlen(msg),
		  "Could not send data to client.\n");
      memset(entry, 0, sizeof entry);
      ERROR_FIXED(recvfrom(*sockfd, entry, sizeof(entry), 0, (struct sockaddr *)client,
			   &addrlen) < 0,
		  "Could not recv from client.\n");
      ERROR_FIXED(strncmp(entry, "exit\n", sizeof(entry)) == 0, "You've quit SAB.\n");
      ERROR_FIXED(strncmp(entry, "exit\r\n", sizeof(entry)) == 0, "You've quit SAB.\n");
    }
  } while(strncmp(entry, password1, sizeof entry) != 0 ||
	  strncmp(entry, password2, sizeof entry) != 0);
  cmd_loop(sockfd, client);
  close_socket(sockfd);
  return 0; /* return success */

 error:
  close_socket(sockfd);
  return 1;
}
