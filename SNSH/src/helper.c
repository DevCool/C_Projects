/***************************************************************
 * helper.c - this contains pretty much only stuff to do with  *
 *				a command line interface, for remote machines.	 *
 ***************************************************************
 * Created by Philip "5n4k3" Simonson			(2017)			 *
 ***************************************************************
 */

/* standard c language header */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

/* my headers */
#include "prs_socket/socket.h"
#include "debug.h"
#include "helper.h"
#include "transfer.h"

/* all linux headers */
#ifdef __linux__
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <espeak/speak_lib.h>
#include "espeech.h"
#endif

/* builtin command strings (compared to what you enter) */
cmd_t commands[CMD_COUNT] = {
	{ "cd",	"change directory to a new one.\r\n", &cmd_cd, CMD_ARGS },
	{ "ls",	"list directory contents.\r\n", &cmd_ls, CMD_ARGS },
	{ "rm",	"delete a file from the system.\r\n", &cmd_rm, CMD_ARGS },
	{ "mkdir", "make a directory in the current one.\r\n", &cmd_mkdir, CMD_ARGS },
	{ "rmdir", "delete an empty directory.\r\n", &cmd_rmdir, CMD_ARGS },
	{ "clear", "fills the output buffer with null characters.\r\n", &cmd_clear, CMD_ARGS },
	{ "touch", "create a blank file.\r\n", &cmd_touch, CMD_ARGS },
	{ "type", "displays a text file 20 lines at a time.\r\n", &cmd_type, CMD_ARGS },
	{ "write", "allows you to write text to a file.\r\n", &cmd_write, CMD_ARGS },
	{ "hostup", "checks if a host is available on specified port.\r\n", &cmd_hostup, CMD_ARGS },
	{ "transfer", "transfers data from one computer to another.\r\n", &cmd_transfer, CMD_ARGS },
#ifdef __linux__
	{ "speak", "speaks the text you type.\r\n", &cmd_speak, CMD_ARGS },
	{ "term", "launches a command that is not builtin.\r\n", &cmd_term, CMD_ARGS },
#endif
	{ "pivot", "launches a new SNSH_client.\r\n", &cmd_pivot, CMD_ARGS },
	{ "help", "print this message.\r\n", &cmd_help, CMD_VOID },
	{ "exit", "exit back to echo hello name.\r\n", &cmd_exit, CMD_VOID }
};

/* sendall() - send an entire block of data until the end.
 */
int sendall(int sd, char *s, int *len) {
	int size = *len;
	int total = 0;
	int bytes;

	while(total < size) {
		bytes = send(sd, s+total, size, 0);
		if(bytes == -1)
			return -1;
		total += bytes;
		size -= total;
	}
	return total;
}

/* cmd_cd() - change directory on remote machine.
 */
int cmd_cd(int sockfd, char **args) {
	char data[BUFSIZ];
	
	memset(data, 0, sizeof data);
	if(args[1] == NULL) {
		snprintf(data, sizeof data, "Usage: %s <dirname>\r\n", args[0]);
		ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
			"Could not send data to client.\n");
	} else {
		ERROR_FIXED(chdir(args[1]) != 0, "Could not change to new directory.\n");
		snprintf(data, sizeof data, "Changed directory to %s\r\n", args[1]);
		ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
			"Could not send data to client.\n");
	}
	return 1;

	error:
	return 1;
}

/* cmd_ls() - list directory on remote machine.
 */
int cmd_ls(int sockfd, char **args) {
	DIR *d;
	struct dirent *dir;
	char msg[1024];
	int i;
	
	memset(msg, 0, sizeof msg);
	if(args[1] != NULL)
		d = opendir(args[1]);
	else
		d = opendir(".");
	
	if(d != NULL) {
		memset(msg, 0, sizeof msg);
		if(args[1] != NULL)
			snprintf(msg, sizeof msg, "Directory listing of %s\r\n", args[1]);
		else
			snprintf(msg, sizeof msg, "Directory listing of ./\r\n");
		if(send(sockfd, msg, strlen(msg), 0) != strlen(msg))
			puts("Error: Could not send data to client.");
		i = 0;
		while((dir = readdir(d))) {
			memset(msg, 0, sizeof msg);
			snprintf(msg, sizeof msg, "%s ", dir->d_name);
			if(send(sockfd, msg, strlen(msg), 0) != strlen(msg)) {
				puts("Error: Could not send directory entry.");
				if(closedir(d) != 0) {
					if(send(sockfd, "Error: listing diectories..\r\n", 30, 0) != 30)
						return 1;
				}
			}
			if(i < 4)
				++i;
			else {
				i = 0;
				if(send(sockfd, "\r\n", 2, 0) != 2)
					puts("Error: Could not send data to client.");
			}
		}
		if(send(sockfd, "\r\n", 2, 0) != 2)
			puts("Error: Could not send data to client.");
		memset(msg, 0, sizeof msg);
		if(closedir(d) != 0) {
			snprintf(msg, sizeof msg, "End of listing.\r\n");
			if(send(sockfd, msg, strlen(msg), 0) != strlen(msg))
				puts("Error: Could not send data to client.");
		}
	} else {
		snprintf(msg, sizeof msg, "Could not list directory, maybe it doesn't exist.\r\n");
		if(send(sockfd, msg, strlen(msg), 0) != strlen(msg))
			puts("Error: Could not send data to client.");
	}
	return 1;
}

/* cmd_rm() - remove a file or many files from remote machine.
 */
int cmd_rm(int sockfd, char **args) {
	char data[BUFSIZ];
	int i = 1;
	
	if(args[1] == NULL) {
		memset(data, 0, sizeof data);
		snprintf(data, sizeof data, "Usage: %s file1.ext ... [files]\r\n", args[0]);
		ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
			"Could not send message.\n");
	} else {
		while(args[i] != NULL) {
			if(remove(args[i]) != 0) {
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "Cannot remove file %s\n", args[i]);
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send message.\n");
			} else {
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "File %s removed.\r\n", args[i]);
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send message.\n");
			}
			++i;
		}
	}
	memset(data, 0, sizeof data);
	snprintf(data, sizeof data, "Total files removed %d.\r\n", i-1);
	ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
		"Could not send message.\n");
	return 1;

	error:
	return 1;
}

/* cmd_mkdir() - makes a directory on the remote machine.
 */
int cmd_mkdir(int sockfd, char **args) {
	char data[BUFSIZ];
	int i = 1;
	
	if(args == NULL || sockfd < 0) {
		return -1;
	} else if(args[1] == NULL) {
		return -1;
	} else {
		while(args[i] != NULL) {
			memset(data, 0, sizeof data);
#if defined(_WIN32) || (_WIN64)
			if(mkdir(args[i]) != 0)
#else
				if(mkdir(args[i], 0755) != 0)
#endif
					snprintf(data, sizeof data, "Directory [%s] not created.\r\n", args[i]);
				else
					snprintf(data, sizeof data, "Created [%s] directory.\r\n", args[i]);
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send data to client.\n");
				++i;
			}
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "Total directories created: %d\r\n", i-1);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
		}
		return 1;

		error:
		return 1;
}

/* cmd_rmdir() - removes a directory on the remote machine.
 */
int cmd_rmdir(int sockfd, char **args) {
		char data[BUFSIZ];
		int i = 1;
		
		if(args == NULL || sockfd < 0) {
			return -1;
		} else if(args[1] == NULL) {
			return -1;
		} else {
			while(args[i] != NULL) {
				memset(data, 0, sizeof data);
				if(rmdir(args[i]) != 0)
					snprintf(data, sizeof data, "[%s] : %s.\r\n", args[i], strerror(errno));
				else
					snprintf(data, sizeof data, "[%s] : Removed successfully.\r\n", args[i]);
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send data to client.\n");
				++i;
			}
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "Total directories removed: %d\r\n", i-1);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
		}
		return 1;

		error:
		return 1;
}

#define TERM_BUFLEN ((int)1024*6)

/* cmd_clear() - fills the output buffer with null characters.
 */
int cmd_clear(int sockfd, char **args) {
		char data[TERM_BUFLEN];
		char msg[128];

		memset(data, 0x20, sizeof data);
		memset(msg, 0, sizeof msg);
		if(args[0] != NULL && args[1] == NULL) {
			char test[3] = "\r\n";
			int datalen = sizeof(data);
			memcpy(&data[datalen-3], test, 3);
			if(sendall(sockfd, data, &datalen) < 0)
				puts("Error: Could not clear buffer.\n");
		} else {
			int msglen;
			snprintf(msg, sizeof msg, "Command takes no arguments.\r\n");
			msglen = strlen(msg);
			if(sendall(sockfd, msg, &msglen) < 0)
				puts("Error: Could not send message.\n");
		}
		return 1;
}

/* cmd_touch() - creates new files on remote machine.
 */
int cmd_touch(int sockfd, char **args) {
		char data[BUFSIZ];
		int i = 1;
		
		if(args[1] == NULL) {
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "Usage: %s file1 file2 ... [files]\r\n", args[0]);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
		} else {
			while(args[i] != NULL) {
				FILE *fp = NULL;
				memset(data, 0, sizeof data);
				if((fp = fopen(args[i], "wb")) == NULL) {
					snprintf(data, sizeof data, "File [%s] failed to create.\r\n", args[i]);
					ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
						"Could not send data to client.\n");
				} else {
					snprintf(data, sizeof data, "File [%s] created successfully.\r\n", args[i]);
					ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
						"Could not send data to client.\n");
					fclose(fp);
					++i;
				}
			}
		}
		memset(data, 0, sizeof data);
		snprintf(data, sizeof data, "Total count of files created: %d\r\n", i-1);
		ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
			"Could not send data to client.\n");
		return 1;

		error:
		return 1;
}

/* cmd_type - displays a text file 20 lines at a time.
 */
int cmd_type(int sockfd, char **args) {
		FILE *fp = NULL;
		char data[BUFSIZ];

		if(args[1] != NULL) {
			char line[256];
			int count = 0;
			ERROR_FIXED((fp = fopen(args[1], "rt")) == NULL, "Could not open file.\n");
			memset(data, 0, sizeof(data));
			snprintf(data, sizeof(data), "Type 'quit' and press 'Enter' to stop reading.\r\n"
				"Press 'Enter' to continue...\r\nFile contents below...\r\n\r\n");
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
			while(fgets(line, sizeof(line), fp) != NULL) {
				if(strchr(line, '\n') != NULL)
					++count;
				if(count >= 20) {
					char getln[64];
					memset(getln, 0, sizeof(getln));
					ERROR_FIXED(recv(sockfd, getln, sizeof(getln), 0) < 0,
						"Could not recv data from client.\n");
					if(strncmp(getln, "quit\r\n", sizeof getln) == 0
						|| strncmp(getln, "quit\n", sizeof getln) == 0)
						break;
					count = 0;
				}
				ERROR_FIXED(send(sockfd, line, strlen(line), 0) != strlen(line),
					"Could not send data to client.\n");
			}
			fclose(fp);
		} else {
			memset(data, 0, sizeof(data));
			snprintf(data, sizeof data, "Usage: %s <file.txt>\r\n", args[0]);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
		}
		return 1;

		error:
		if(fp != NULL)
			fclose(fp);
		return -1;
}

/* cmd_write() - allows you to write text to a file.
 */
int cmd_write(int sockfd, char **args) {
		FILE *fp = NULL;
		char data[BUFSIZ];

		if(args[1] != NULL) {
			char line[256];
			int bytes = -1;
			
			ERROR_FIXED((fp = fopen(args[1], "wt")) == NULL, "Could not open file for writing.\n");
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "Type 'EOF' on a blank without quotes, to write...\r\n");
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
			do {
				memset(line, 0, sizeof line);
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "> ");
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send data to client.\n");
				ERROR_FIXED((bytes = recv(sockfd, line, sizeof(line), 0)) < 0,
					"Could not recv data from client.\n");
				if(strncmp(line, "EOF\r\n", sizeof(line)) == 0
					|| strncmp(line, "EOF\n", sizeof(line)) == 0)
					break;
				else
					fprintf(fp, "%s", line);
			} while(bytes > 0);
			fclose(fp);
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "File written successfully.\r\n");
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
		} else {
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "Usage: %s <file.txt>\r\n", args[0]);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
		}
		return 1;

		error:
		if(fp != NULL)
			fclose(fp);
		return -1;
}

/* cmd_hostup() - checks host status on specific port.
 */
int cmd_hostup(int sockfd, char **args) {
		struct addrinfo hints, *server, *p;
		socklen_t addrlen;
		char data[BUFSIZ];
		int rv, error, clientfd;

		if(args[1] != NULL && args[2] != NULL) {
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;

			if(getaddrinfo(args[1], args[2], &hints, &server) < 0) {
				snprintf(data, sizeof data, "Error: Could not get host info.\r\n");
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send data to client.");
			}
			for(p = server; p != NULL; p = p->ai_next) {
				if((clientfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
					fprintf(stderr, "Error cannot create socket: %s\n", strerror(errno));
					errno = 0;
					continue;
				}

				if((rv = getsockopt(clientfd, SOL_SOCKET, SO_ERROR, (char *)&error, &addrlen)) != 0) {
					fprintf(stderr, "Error connecting to socket: %s\n", strerror(rv));
					errno = 0;
					break;
				}

				if(connect(clientfd, p->ai_addr, p->ai_addrlen) < 0) {
					memset(data, 0, sizeof data);
					snprintf(data, sizeof data, "Error: Could not connect to %s on port %d.\r\n",
						args[1], atoi(args[2]));
					ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
						"Could not send data to client.\n");
					errno = 0;
					goto error;
				}
				break;
			}
			freeaddrinfo(server);
			/* connection successful */
			memset(data, 0, sizeof(data));
			snprintf(data, sizeof(data), "Host: %s\r\nPort: %d\r\nStatus: %s\r\n",
				args[1], atoi(args[2]), (error) ? "[FAILED]" : "[SUCCESS]");
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.\n");
			close(clientfd);
		} else {
			snprintf(data, sizeof data, "Usage: %s <hostname|ipaddress> <port>\r\n", args[0]);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.");
		}
		return 1;

		error:
		if(clientfd > 0)
			close(clientfd);
		return -1;
}

/* cmd_transfer() - download/upload from/to remote machine.
 */
int cmd_transfer(int sockfd, char **args) {
		char data[BUFSIZ];
		int i = 2;

		if(args[0] == NULL) {
			return -1;
		} else if(args[0] != NULL && args[1] == NULL) {
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "Usage: %s <upload|download> file1.ext ... [files]\r\n",
				args[0]);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Could not send data to client.");
		} else {
			if(strcmp(args[1], "upload") == 0) {
				while(args[i] != NULL) {
					upload("0.0.0.0", args[i]);
					++i;
				}
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "Total files uploaded: %d\r\n", i-2);
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send data to client.");
			} else if(strcmp(args[1], "download") == 0) {
				while(args[i] != NULL) {
					download("0.0.0.0", args[i]);
					++i;
				}
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "Total files downloaded: %d\r\n", i-2);
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send data to client.");
			} else {
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "Usage: %s <upload|download> file1.ext ... [files]\r\n",
					args[0]);
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Could not send data to client.");
			}
		}
		return 1;

		error:
		return -1;
}

#ifdef __linux__
/* cmd_speak() - speaks text you enter on remote machine.
 */
int cmd_speak(int sockfd, char **args) {
		char data[2048];
		char msg[1024];
		int i = 1;

		memset(msg, 0, sizeof msg);
		memset(data, 0, sizeof data);
		if(args == NULL || sockfd < 0)
			return -1;

		snprintf(data, sizeof data, "Speaking: ");
		strncpy(msg, args[i], sizeof msg);
		while(args[++i] != NULL) {
			strncat(msg, " ", sizeof msg);
			strncat(msg, args[i], sizeof msg);
		}
		strncat(data, msg, sizeof data);
		strncat(data, "\r\n", sizeof data);
		ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
			"Could not send data to client.");
		/* Speak the message */
		speak(msg, strlen(msg));
		return 1;

		error:
		return 1;
}

/* cmd_term() - executes an external command.
 */
int cmd_term(int sockfd, char **args) {
		char data[BUFSIZ];
		int pid = 0;

		memset(data, 0, sizeof data);
		pid = fork();
		ERROR_FIXED(pid < 0, "Could not fork to background.\n");
		if(pid == 0) {
			dup2(sockfd, 0);
			dup2(sockfd, 1);
			dup2(sockfd, 2);
			++args;
			ERROR_FIXED(execvp(*args, args) != strlen(data), "Could not execute command.\n");
		} else {
			waitpid(0, NULL, 0);
		}
		return 1;

		error:
		return -1;
}
#endif

/* cmd_pivot() - executes an external command.
 */
int cmd_pivot(int sockfd, char **args) {
		char data[BUFSIZ];

#if defined(_WIN32) || (_WIN64)
		if(args[1] != NULL && (args[2] == NULL || args[2] != NULL)) {
			char cdir[BUFSIZ];
			PROCESS_INFORMATION pi;
			STARTUPINFO si;

			ZeroMemory(&pi, sizeof(pi));
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
			si.wShowWindow = FALSE;

			if(getcwd(cdir, sizeof cdir) == NULL) {
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "Error: Cannot get current working directory.\r\n");
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Cannot send data to client.\n");
				return 1;
			}

			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "C:\\Windows\\System32\\cmd.exe /c %s\\SNSH_client.exe",
				cdir);
			while(*++args != NULL) {
				strncat(data, " ", sizeof data);
				strncat(data, *args, sizeof data);
			}

			if(CreateProcess(NULL, data, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) != 0) {
				memset(data, 0, sizeof data);
				snprintf(data, sizeof data, "Cannot create the process.\r\n");
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Cannot send data to client.\n");
				return 1;
			}

			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		} else {
			memset(data, 0, sizeof data);
			snprintf(data, sizeof data, "Usage: %s <ipaddress> [port]\r\n", args[0]);
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Cannot send data to client.\n");
		}
#elif __linux__
		int pid = 0;

		memset(data, 0, sizeof data);
		if(args[1] == NULL) {
			snprintf(data, sizeof data, "Usage: pivot <ipaddress> <port>\r\n");
			ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
				"Cannot send data to client.\n");
		} else {
			if(args[1] != NULL && args[2] != NULL) {
				pid = fork();
				ERROR_FIXED(pid < 0, "Could not fork to background.\n");
				if(pid == 0) {
					dup2(sockfd, 0);
					dup2(sockfd, 1);
					dup2(sockfd, 2);
					ERROR_FIXED(execvp("SNSH_client", args) < 0, "Could not execute command.\n");
				} else {
					waitpid(0, NULL, 0);
				}
			} else {
				snprintf(data, sizeof data, "Usage: pivot <ipaddress> <port>\r\n");
				ERROR_FIXED(send(sockfd, data, strlen(data), 0) != strlen(data),
					"Cannot send data to client.\n");
			}
		}
#endif
		return 1;

		error:
		return -1;
}

/* cmd_help() - displays help about the list of commands available.
 */
int cmd_help(int sockfd) {
		char msg[BUFSIZ];
		int i;

		memset(msg, 0, sizeof msg);
		snprintf(msg, sizeof msg, "*** Help Below ***\r\n");
		for(i = 0; i < CMD_COUNT; i++) {
			strncat(msg, commands[i].cmd, sizeof msg);
			strncat(msg, " - ", sizeof msg);
			strncat(msg, commands[i].help, sizeof msg);
		}
		strncat(msg, "*** End Help ***\r\n", sizeof msg);
		if(send(sockfd, msg, strlen(msg), 0) != strlen(msg))
			puts("Error: Could not send data to client.");
		return 1;
}

/* cmd_exit() - exits the remote shell.
 */
int cmd_exit(int sockfd) {
		return 0;
}

/* cmd_split() - split an entire command string into tokens.
 */
char **cmd_split(char line[]) {
		char **tokens = NULL;
		char *token = NULL;
		int i = 0, size;

		size = CMD_TOK_SIZE * sizeof(char *);
		tokens = (char **)malloc(size);
		CHECK_MEM(tokens);

		token = strtok(line, CMD_TOK_DELIMS);
		while(token != NULL) {
			tokens[i] = token;
			++i;
			if(i >= size) {
				size += CMD_TOK_SIZE * sizeof(char *);
				tokens = (char **)realloc(tokens, size);
				CHECK_MEM(tokens);
			}
			token = strtok(NULL, CMD_TOK_DELIMS);
		}
		tokens[i] = NULL;
		return tokens;
		
		error:
		return NULL;
}

/* cmd_execute() - executes all builtin commands.
 */
int cmd_execute(int sockfd, char **args) {
		char data[BUFSIZ];
		int i, found = 0;

		memset(data, 0, sizeof(data));
		if(args[0] == NULL) {
			snprintf(data, sizeof data, "Error: No data entered.\r\n");
			if(send(sockfd, data, strlen(data), 0) != strlen(data)) {
				puts("Error: Could not send message to client.");
			}
		} else if(args[0] != NULL) {
			if(strncmp(args[0], "", strlen(args[0])) == 0) {
				snprintf(data, sizeof data, "Error: No command entered.\r\n");
				if(send(sockfd, data, strlen(data), 0) < 0) {
					puts("Error: Could not receive data from client.");
				}
			} else {
				for(i = 0; i < CMD_COUNT; i++) {
					if(strncmp(args[0], commands[i].cmd, strlen(args[0])) == 0) {
						found = 1;
						break;
					} else {
						found = 0;
					}
				}

				if(found) {
					if(commands[i].type == CMD_ARGS)
						return commands[i].func(sockfd, args);
					else if(commands[i].type == CMD_VOID)
						return commands[i].func(sockfd);
					else {
						snprintf(data, sizeof data, "Error: Command type not valid.\r\n");
						if(send(sockfd, data, strlen(data), 0) < 0)
							puts("Error: Cannot send data to client.");
					}
				} else {
					snprintf(data, sizeof data, "Error: Command not found.\r\n");
					if(send(sockfd, data, strlen(data), 0) < 0)
						puts("Error: Cannot send data to client.");
				}
			}
		}

		return 1;
}

/* cmd_loop() - main command interface loop.
 */
void cmd_loop(int *sockfd, struct sockaddr_in *client) {
		char line[CMD_LEN];
		char msg[1024];
		char **args = NULL;
		int status, msg_len;

#ifdef __linux__
		speakInit();
#endif
		
		do {
			memset(msg, 0, sizeof msg);
			snprintf(msg, sizeof msg, "CMD >> ");
			msg_len = strlen(msg);
			if(sendall(*sockfd, msg, &msg_len) < 0)
				puts("Warning: Couldn't send prompt to client.");
			memset(line, 0, sizeof line);
			if(recv(*sockfd, line, sizeof line, 0) < 0) {
				puts("Error: Cannot recv from client.");
			} else {
				args = cmd_split(line);
				status = cmd_execute(*sockfd, args);
				free(args);
			}
		} while(status);

#ifdef __linux__
		speakCleanup();
#endif
}
