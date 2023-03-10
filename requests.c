#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char **tokens, int tokens_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    // add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        for (int i = 0; i < cookies_count; ++i) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    if (tokens != NULL) {
        for (int i = 0; i < tokens_count; ++i) {
            sprintf(line, "Authorization: Bearer %s", tokens[i]);
            compute_message(message, line);
        }
    }

    // add final new line
    compute_message(message, "");
    return message;
}


char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count,
                            char **tokens, int tokens_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    //add necessary headers (Content-Type and Content-Length are mandatory)
    int body_data_size = 0;
    for (int i = 0; i < body_data_fields_count - 1; i++) {
        strcat(body_data_buffer, body_data[i]);
        strcat(body_data_buffer, "&");
        body_data_size += strlen(body_data[i]);
        body_data_size++; //for '&
    }

    strcat(body_data_buffer, body_data[body_data_fields_count - 1]);
    body_data_size += strlen(body_data[body_data_fields_count - 1]);

    sprintf(line, "Content-Type: %s\r\nContent-Length: %d", content_type, body_data_size);
    compute_message(message, line);

    // add cookies
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        for (int i = 0; i < cookies_count; ++i) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    if (tokens != NULL) {
        memset(line, 0, LINELEN);
        for (int i = 0; i < tokens_count; ++i) {
            sprintf(line, "Authorization: Bearer %s", tokens[i]);
            compute_message(message, line);
        }
    }

    // add new line at end of header
    compute_message(message, "");

    // add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}


char *compute_delete_request(char *host, char *url, char *query_params,
                                char **cookies, int cookies_count,
                                char **token, int token_len) {
    char *message =(char*) calloc(BUFLEN, sizeof(char));
    char *line =(char*) calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        for (int i = 0; i < cookies_count; ++i) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }
    
    if (token != NULL) {
        memset(line, 0, LINELEN);
        for (int i = 0; i < token_len; ++i) {
            sprintf(line, "Authorization: Bearer %s", token[i]);
            compute_message(message, line);
        }
    }

    // add final new line
    compute_message(message, "");
    return message;
}
