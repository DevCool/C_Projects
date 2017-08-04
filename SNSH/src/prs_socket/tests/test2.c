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
  sockfd = sock_func.socket_bind(NULL, 5555, &clientfd, &client);
  /* handle server (swap to client handler) */
  retval = handle_server(&sockfd, &clientfd, &client, NULL, &hdl_client);
  /* cleanup socket */
  close_socket(&sockfd);
  return retval;
}

int find_network_newline(char *msg, int size) {
  int i;
  for(i = 0; i < size; i++)
    if(msg[i] == '\n')
      return i;
  return -1;
}

static int inbuf;

/* getline_network() - gets a line of data from socket.
 */
int getline_network(char *msg) {
  int bytes_read = read(STDIN_FILENO, msg, 256-inbuf);
  short flag = -1;
  int where = -1;

  inbuf += bytes_read;
  where = find_network_newline(msg, inbuf);
  if(where >= 0) {
    inbuf = 0;
    flag = 0;
  }
  return flag;
}

/* hdl_client() - main loop for handling client.
 */
int hdl_client(int *sockfd, struct sockaddr_in *client, const char *filename) {
#if defined(_WIN32) || (_WIN64)
  FD_SET read_fds;
#else
  fd_set read_fds;
#endif
  char msg[BUFSIZ];
  char buf[BUFSIZ];
  int maxfd = *sockfd, i;
  
  while(1) {
    FD_ZERO(&read_fds);
    FD_SET(*sockfd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    if(maxfd < STDIN_FILENO) {
      maxfd = STDIN_FILENO;
    }

    select(maxfd+1, &read_fds, NULL, NULL, NULL);

    for(i = 0; i <= maxfd; i++) {
      if(i != *sockfd) {
	if(FD_ISSET(STDIN_FILENO, &read_fds)) {
	  memset(buf, 0, sizeof buf);
	  if(getline_network(buf) == 0) {
	    if(send(*sockfd, buf, strlen(buf), 0) < 0) {
	      perror("send");
	      goto error;
	    }
	  }
	}
	else {
	  if(FD_ISSET(*sockfd, &read_fds)) {
	    memset(msg, 0, sizeof msg);
	    if(recv(*sockfd, msg, sizeof msg, 0) < 0) {
	      perror("recv");
	      goto error;
	    }
	  }
	}
      }
    }
  }
  return 0;

 error:
  return -1;
}
