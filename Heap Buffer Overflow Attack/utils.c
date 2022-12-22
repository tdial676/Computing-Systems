#include <arpa/inet.h>          /* inet_ntoa */
#include <signal.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>

#include "sha-256.h"
#include "utils.h"
#include "junk.h"

extern FILE *stream;

#define PASSWORD_LENGTH 10

void do_directory_listing(char *directory) {
	DIR *dp = opendir(directory);
	if (dp == NULL) 
	{
		perror("opendir");
		return;
	}

	struct dirent *entry;

	fprintf(stream, "<h1>Index of %s</h1>",  directory); 
	fprintf(stream, "<ul>");
	while((entry = readdir(dp))) {
		if (entry->d_name[0] != '.') { 
			fprintf(stream, "<li><a href='%s/%s'>%s</a></li>\n", directory, entry->d_name, entry->d_name);
		}
	}
	fprintf(stream, "</ul>");
	fflush(stream);
}

char *prep_token_with_prefix(char *token, char *prefix) {
	char concat[MAX_QUERY_LENGTH + TOKEN_LENGTH] = {0};
    concat[0] = '\0';
	strcat(concat, prefix);
	strcat(concat, token);

	uint8_t hash[32] = {0};
	calc_sha_256(hash, concat, strlen(concat));
	char *string = calloc(strlen("tokens/") + TOKEN_LENGTH + 1000, 1);
	strcpy(string, "tokens/");
    char *middle = string + strlen("tokens/");
	hash_to_string(middle, hash);
	string[strlen("tokens/") + TOKEN_LENGTH] = '\0';
	return string;
}

void update_grade_with_prefix(char *prefix) {
	char *token = malloc(PASSWORD_LENGTH + 1);
	fread(token, PASSWORD_LENGTH, 1, stream);
	send_headers(200, "OK");
	char *cprefix = malloc(strlen(prefix) + strlen("token=") + 1);
	strcpy(cprefix, prefix);
	strcat(cprefix, "token=");
	char *prepped = prep_token_with_prefix(token, cprefix); 
	free(cprefix);
	FILE *f = fopen(prepped, "w");
	fclose(f);
	fprintf(stream, "Grade Updated! Wrote token %s\n", prepped);
	fflush(stream);
	fclose(stream);
	free(token);
	close(currfd);
	exit(0);
}

void update_stage1_grade() {
	update_grade_with_prefix("adam");
}


void serve(FILE *f) {
	char c = 0;
	while (fread(&c, 1, 1, f) == 1) {
		fprintf(stream, "%c", c);
	}
	fclose(f);
}

void send_headers(int code, char *name) {
	fprintf(stream, "HTTP/1.1 %d %s\r\n", code, name);
	fprintf(stream, "Connection: close\r\n");
	fprintf(stream, "Content-Type: text/html\r\n");
	fprintf(stream, "\r\n");
	fflush(stream);
}



char *get_admin_token() {
	FILE *f = fopen("config/admin_token", "r");
	char *tok = malloc(TOKEN_LENGTH + 1);
	fread(tok, TOKEN_LENGTH, 1, f);
	return tok;
}

void handle_single_check(char *query, char *display, char *prefix) {
	char *string = prep_token_with_prefix(query, prefix);

	if (access(string, F_OK ) != -1) {
		fprintf(stream, "<b>Great job!</b>  You've completed %s!", display);
	}
	else {
		fprintf(stream, "<b>OH NOES</b>...it doesn't seem like you have completed %s!", display);
	}

}

void handle_check(char *query) {
	send_headers(200, "OK");
	fprintf(stream, "Hello User! You have submitted the following tokens for your password.");
	fprintf(stream, "<ul>");
	fprintf(stream, "<li>");
	handle_single_check(query, "stage1", "adam");
	fprintf(stream, "</li>");
	char *admin_token = get_admin_token();
	fprintf(stream, "<li>");
	handle_single_check(query, "stage2", admin_token);
	fprintf(stream, "</li>");
	fprintf(stream, "<li>");
	char *student_token = strtok(query, "=");
	student_token = strtok(NULL, "="); 
	if (strlen(student_token) != PASSWORD_LENGTH) {
		fprintf(stream, "<b>OH NOES</b>...it doesn't seem like you have completed %s!", "stage3");
	}
	else {
		char *path = malloc(strlen("tokens/") + PASSWORD_LENGTH + 1);
		strcpy(path, "tokens/");
		strcat(path, student_token);
		if (access(path, F_OK ) != -1) {
			fprintf(stream, "<b>Great job!</b>  The file for stage3 exists.  If your file does not have the correct contents, you will still not receive credit!");
		}
		else {
			fprintf(stream, "<b>OH NOES</b>...it doesn't seem like you have completed %s!", "stage3");
		}
		free(path);
	}
	fprintf(stream, "</li>");
	fprintf(stream, "</ul>");
	free(admin_token);
}

bool is_directory(const char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

char *remove_leading_slashes(char *uri) {
	while (*uri == '/') {
		uri++;
	}
	return uri;
}

char *remove_trailing_slashes(char *uri) {
	char *end = uri + strlen(uri) - 1;
	while (*end == '/' && end >= uri) {
		uri--;
	}
	*(end + 1) = '\0';
	return uri;
}

char *remove_dot_dot(char *uri) {
	char *front = uri;
	uri++;
	while (*uri != '\0') {
		if (*(uri - 1) == '.' && (*uri == '.' || *uri == '/')) {
			*(uri - 1) = '/';
			*uri = '/';
		}
		uri++;
	}
	return front;
}

bool is_forbidden(char *uri) {
	return !strncmp(uri, "tokens", strlen("tokens")) || !strncmp(uri, "config", strlen("config"));
}
