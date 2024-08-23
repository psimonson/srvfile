/*
 * main - Simple file serving program.
 *
 * Author: Philip R. Simonson
 * Date:   07/27/2024
 *
 */

#define _DEFAULT_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prs/network.h"

#define MAXBUF 256

/* Entry point for program.
 */
int main(int argc, char *argv[])
{
	short unsigned int port = 8888;
	long int total;
	long int size;
	SOCKET s, c;
	char buf[MAXBUF];
	FILE *fp;

	if(argc != 2) {
		printf("Usage: %s </path/to/file.ext>\n", argv[0]);
		return 1;
	}

	if((fp = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Error: Cannot open file '%s'.\n", argv[1]);
		return 2;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	s = server_socket_open(&port);
	if(s == INVALID_SOCKET) {
		fprintf(stderr, "Error: Cannot open server socket.\n");
		fclose(fp);
		return 3;
	}
	printf("Server listening on 0.0.0.0 port %hu...\n", port);

	c = server_socket_accept(s);
	if(c == INVALID_SOCKET) {
		fprintf(stderr, "Error: Cannot accept client.\n");
		fclose(fp);
		return 3;
	}

	total = 0;
	do {
		int nbytes_read;
		int nbytes_wrote;

		nbytes_read = fread(buf, sizeof(char), MAXBUF-1, fp);
		nbytes_wrote = send(c, buf, nbytes_read, 0);

		if(nbytes_wrote < 0) {
			fprintf(stderr, "Error: Sending data.\n");
			break;
		}

		total += nbytes_wrote;
	} while(!feof(fp));
	fclose(fp);
	socket_close(c);
	socket_close(s);

	if(total != size) {
		printf("Sending failed!\n");
		return 1;
	}

	printf("Sending ok!\n");
	return 0;
}
