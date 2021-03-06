#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "cJSON.h"
#include "helpers.h"
#include "requests.h"
#include "buffer.h"


char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // get
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaug jwt daca nu este null
    if(jwt != NULL){

       memset(line, 0, LINELEN);
       sprintf(line, "Authorization: Bearer ");
       sprintf(line + strlen(line), "%s", jwt);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    // adaug cookie-ul daca nu este null
    if (cookies != NULL)
    {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       sprintf(line + strlen(line), "%s", cookies);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char username[], char password[])
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    cJSON *root ;
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "username", cJSON_CreateString(username));
    cJSON_AddItemToObject(root, "password", cJSON_CreateString(password));
   
    char sir[100];
    body_data = cJSON_Print(root);
    strcpy(sir, body_data);
    int content_length = strlen(sir);
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    compute_message(message, "");
    compute_message(message, body_data);
    compute_message(message, "");

    free(body_data);
    free(line);
    return message;
}

char *compute_post_add(char *host, char *url, char* content_type, char *body_data,
                            int content_length, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // post
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);


    if(jwt != NULL){

       memset(line, 0, LINELEN);
       sprintf(line, "Authorization: Bearer ");
       sprintf(line + strlen(line), "%s", jwt);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    // concatenez payload-ul
    compute_message(message, "");
    compute_message(message, body_data);
    compute_message(message, "");
    free(body_data);
    free(line);
    return message;
}


char *compute_delete_request(char *host, char *url, char *query_params,
                            char *cookies, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }
    
    compute_message(message, line);

    // host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    
    if(jwt != NULL){

       memset(line, 0, LINELEN);
       sprintf(line, "Authorization: Bearer ");
       sprintf(line + strlen(line), "%s", jwt);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    // adaug cookie-ul in mesaj daca nu este null
    if (cookies != NULL) {

       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       sprintf(line + strlen(line), "%s", cookies);
       line[strlen(line)] = '\0';
       compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}