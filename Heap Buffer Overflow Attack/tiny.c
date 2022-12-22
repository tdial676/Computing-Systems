#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "junk.h"
#include "utils.h"
#include "eat.h"

FILE *stream;
int listenfd = 0;
int currfd = 0;

void process(void) {
	char request[MAX_METHOD_LENGTH + MAX_URL_LENGTH + MAX_QUERY_LENGTH + 3];
	fgets(request, MAX_METHOD_LENGTH + MAX_URL_LENGTH + MAX_QUERY_LENGTH + 3, stream);
	eat_headers();

	char method[MAX_METHOD_LENGTH];
	char url[MAX_URL_LENGTH];
	char query[MAX_QUERY_LENGTH];

	char *s = strtok(request, " ");
	strcpy(method, s);
	s = strtok(NULL, " ");
	s = strtok(s, "?");
	strcpy(url, s);
	s = strtok(NULL, "?");
	if (!s) {
		s = "";
	}
	strcpy(query, s);

	if (!strcmp(method, "GET")) {
		char *uri = remove_leading_slashes(url);
		uri = remove_dot_dot(uri);
		uri = remove_trailing_slashes(uri);

		if (!strlen(uri)) {
			uri = ".";
		}
		if (is_forbidden(uri)) {
			send_headers(403, "Forbidden");
			fprintf(stream, "<h1>403 Forbidden</h1>");
		}
		else if (!strcmp(url, "/debug")) {
			if (strlen(query) > strlen("token=")) {
				handle_check(query);
			}
			else {
				send_headers(418, "I'm a teapot");
				fprintf(stream, "Make sure your request has a token!\n"); 
			}
		}
		else  {
			if (is_directory(uri)) {
				send_headers(200, "OK");
				do_directory_listing(uri);
			}
			else {
				FILE *f = fopen(uri, "r");
				if (!f) {
					send_headers(404, "Not Found");
					fprintf(stream, "<h1>404 Not Found</h1>");
				}
				else {
					send_headers(200, "OK");
					serve(f);
					return;
				}
			}
		}
		fprintf(stream, "<hr>Proudly served by <tt>tiny</tt>.  The HTTP server using impeccable code.");
	}
	else if (!strcmp(method, "POST") && !strcmp(url, "/submit")) {
		// TODO: There's no front end interface for this endpoint...admins will just have to submit raw requests...
		char *admin_token = get_admin_token();
		if (!strcmp(query + 6, admin_token)) {
			update_grade_with_prefix(admin_token);
		}
		free(admin_token);
	}
	else {
		send_headers(501, "Not Implemented");
		fprintf(stream, "<h1>501 Not Found</h1>");
	}
}

