#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
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

#define HOST_MAIN_SERVER "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"
#define PORT 8080

#define REGISTER "/api/v1/tema/auth/register" // register
#define LOGIN  "/api/v1/tema/auth/login"   // login
#define ACCESS "/api/v1/tema/library/access" // access
#define BOOKS  "/api/v1/tema/library/books" // books
#define LOGOUT "/api/v1/tema/auth/logout" // logout

#define BUFFERSIZE 100

struct sockaddr *get_addr(char* name)
{
	int ret;
	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	ret = getaddrinfo(name, NULL, &hints, &result);
	if (ret != 0)
	{
		perror("getaddrinfo");
	}

	return result->ai_addr;
}


int main() {

   int server_socket;
   // Get main server IP
   struct sockaddr_in *serv_addr = (struct sockaddr_in *) get_addr(HOST_MAIN_SERVER);

   char ip[100];
   // obligatoriu memset
   memset(ip, 0, 100);
   inet_ntop(AF_INET, &(serv_addr->sin_addr), ip, 100);  // obtain ip in text form (converting from binary)
   // deschidem conexiunea
   server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

   char *request;
   char *response;
   char *response_JWT;
   char *username1;
   char *password1;
   char buffer[BUFFERSIZE];
   char username[BUFFERSIZE], password[BUFFERSIZE], id[BUFFERSIZE], book[BUFFERSIZE], cookie[LINELEN];
    char cookie1[LINELEN];
    char jwt[LINELEN];
    memset(cookie, 0, LINELEN);
    memset(jwt, 0, LINELEN);
    char *title1, *author1, *genre1, *publisher1, *page_count1;
    char title[BUFFERSIZE], author[BUFFERSIZE], genre[BUFFERSIZE], publisher[BUFFERSIZE], page_count[BUFFERSIZE];
  
  
   while(fgets(buffer, sizeof(buffer), stdin) != NULL){
            // comanda exit
            if(strcmp("exit\n", buffer) == 0){
                  close_connection(server_socket);
                  break;
            }
            // comanda register
            if(strcmp("register\n", buffer) == 0){
                  // citim username-ul
                  //scanf("%s", buffer);
                  fgets(buffer, sizeof(buffer), stdin);
                  username1 = strstr(buffer, "=");
                  username1 = username1 + 1;
                  strcpy(username, username1);
                  username[strlen(username) - 1] = '\0';

                  //citim password
                 // scanf("%s", buffer);
                  fgets(buffer, sizeof(buffer), stdin);
                  password1 = strstr(buffer, "=");
                  password1 = password1 + 1;
                  strcpy(password, password1);
                  password[strlen(password) - 1] = '\0';

                  char *data = calloc(LINELEN, sizeof(char));
                  //ne inregistram
                  request = compute_post_request(HOST_MAIN_SERVER, REGISTER, "application/json", data, 0, NULL, 0, username, password);
                  send_to_server(server_socket, request);
 

                  response = receive_from_server(server_socket);
                  printf("\n%s\n\n", response);
                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);
                        
            }
            // comanda login
            if(strcmp("login\n", buffer) == 0){
                  // citim username-ul
                 // scanf("%s", buffer);
                 fgets(buffer,sizeof(buffer), stdin);
                  username1 = strstr(buffer, "=");
                  username1 = username1 + 1;
                  strcpy(username, username1);
                  username[strlen(username) - 1] = '\0';

                  //citim password
                  //scanf("%s", buffer);
                  fgets(buffer, sizeof(buffer), stdin);
                  password1 = strstr(buffer, "=");
                  password1 = password1 + 1;
                  strcpy(password, password1);
                  password[strlen(password) - 1] = '\0';

                  char *data = calloc(LINELEN, sizeof(char));

                  //ne logam
                  request = compute_post_request(HOST_MAIN_SERVER, LOGIN, "application/json", data, 0, NULL, 0, username, password);
                  send_to_server(server_socket, request);

                  response = receive_from_server(server_socket);
                  
                  //am primit raspunsul de la server
                  printf("\n%s\n\n", response);

                  // caut sa vad daca am eroare
                  char *begin = calloc(LINELEN, sizeof(char));
                  begin = strstr(response, "\"error\"" );

                  // n-am gasit eroare la logare, deci actualizez cookie-ul
                  // cookie-ul de la ultimul cont introdus corect
                  if(begin == NULL){
                        memset(cookie, 0, LINELEN);
                        char *start = strstr(response, "Set-Cookie: ");
                        start += strlen("Set-Cookie: ");
                        char *end = strstr(start, "; Path=/;");
                        //obtinem cookie-ul
                        strncpy(cookie, start, end-start);
                  }
                  // deschid conexiunea
                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);
                        
            }

            // comanda enter_library
            if(strcmp("enter_library\n", buffer) == 0){

                  request = compute_get_request(HOST_MAIN_SERVER, ACCESS, NULL, cookie, NULL);
                  send_to_server(server_socket, request);
                  response_JWT = receive_from_server(server_socket);
                  printf("\n%s\n\n", response_JWT);

                  // n-am gasit eroare la enter library, deci pastrez ultimul token
                  char *begin = calloc(LINELEN, sizeof(char));
                  begin = strstr(response_JWT, "\"error\"" );

                  // token-ul de la ultimul cont introdus corect
                  if(begin == NULL){
                        memset(jwt, 0, LINELEN);
                        char *start = strstr(response_JWT,"{\"token\":\"" );
                        start = start + strlen("{\"token\":\"");
                        char *end = strstr(start, "\"}");
                        strncpy(jwt, start, end-start);
                  }

                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);
            }

            // comanda get_books
            if(strcmp("get_books\n", buffer) == 0){

                  request = compute_get_request(HOST_MAIN_SERVER, BOOKS, NULL, NULL, jwt);
                  send_to_server(server_socket, request);
                  response = receive_from_server(server_socket);
                  printf("\n%s\n\n", response);
                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);
            }

            // comanda get_book
            if(strcmp("get_book\n", buffer) == 0){

                  strcpy(cookie1, cookie);
                  //scanf("%s", buffer);
                  fgets(buffer, sizeof(buffer), stdin);
                  strcpy(cookie, cookie1);
                  
                  char *id1 = strstr(buffer, "=");
                  id1 = id1 + 1;
                  strcpy(id, id1);
                  id[strlen(id) - 1] = '\0';
                  memset(book,0,BUFFERSIZE);
                  strcat(book, BOOKS);
                  strcat(book,"/");
                  strcat(book, id);

                  request = compute_get_request(HOST_MAIN_SERVER, book, NULL, NULL, jwt);
                  send_to_server(server_socket, request);
                  response = receive_from_server(server_socket);
                  printf("\n%s\n\n", response);
                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);
                  
            }

            // comanda add_book
            if(strcmp("add_book\n", buffer) == 0){

                  // pierd cookie-ul din cauza scanf-urilor
                  strcpy(cookie1, cookie);

                  //citim title-ul
                  fgets(buffer, sizeof(buffer), stdin);
                  //scanf("%s", buffer);
                  title1 = strstr(buffer, "=");
                  title1 = title1 + 1;
                  strcpy(title, title1);
                  title[strlen(title) - 1] = '\0';

                  //citim author
                  //scanf("%s", buffer);
                  fgets(buffer, BUFFERSIZE, stdin);
                  author1 = strstr(buffer, "=");
                  author1 = author1 + 1;
                  strcpy(author, author1);
                  author[strlen(author) - 1] = '\0';

                  //citim genre
                  //scanf("%s", buffer);
                  fgets(buffer, BUFFERSIZE, stdin);
                  genre1 = strstr(buffer, "=");
                  genre1 = genre1 + 1;
                  strcpy(genre, genre1);
                  genre[strlen(genre) - 1] = '\0';

                  //citim publisher
                  //scanf("%s", buffer);
                  fgets(buffer, BUFFERSIZE, stdin);
                  publisher1 = strstr(buffer, "=");
                  publisher1 = publisher1 + 1;
                  strcpy(publisher, publisher1);
                  publisher[strlen(publisher) - 1] = '\0';

                  //citim page_count
                  //scanf("%s", buffer);
                  fgets(buffer, BUFFERSIZE, stdin);
                  page_count1 = strstr(buffer, "=");
                  page_count1 = page_count1 + 1;
                  strcpy(page_count, page_count1);
                  page_count[strlen(page_count) - 1] = '\0';
                  int x = atoi(page_count);

                  // salvez inapoi cookie-ul
                  strcpy(cookie, cookie1);

                  // folosire biblioteca cJSON
                  cJSON *root ;
                  root = cJSON_CreateObject();
                  cJSON_AddItemToObject(root, "title", cJSON_CreateString(title));
                  cJSON_AddItemToObject(root, "author", cJSON_CreateString(author));
                  cJSON_AddItemToObject(root, "genre", cJSON_CreateString(genre));
                  cJSON_AddItemToObject(root, "page_count", cJSON_CreateNumber(x));
                  cJSON_AddItemToObject(root, "publisher", cJSON_CreateString(publisher));

                  /* print everything */
                  char sir[100];
                  char *body_data;
                  body_data = cJSON_Print(root);
                  strcpy(sir, body_data);
                  int content_length = strlen(sir);

                  request = compute_post_add(HOST_MAIN_SERVER, BOOKS, "application/json", body_data, content_length, jwt);
                  send_to_server(server_socket, request);
                  response = receive_from_server(server_socket);
                  printf("\n%s\n\n", response);
                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

            }

            // comanda delete_book
            if(strcmp("delete_book\n", buffer) == 0){

                  strcpy(cookie1, cookie);
                  fgets(buffer, sizeof(buffer), stdin);
                  //scanf("%s", buffer);
                  strcpy(cookie, cookie1);
                  
                  // concatenarea id-ului la api-ul BOOK
                  char *id1 = strstr(buffer, "=");
                  id1 = id1 + 1;
                  strcpy(id, id1);
                  id[strlen(id) - 1] = '\0';
                  memset(book,0,BUFFERSIZE);
                  strcat(book, BOOKS);
                  strcat(book,"/");
                  strcat(book, id);

                  request = compute_delete_request(HOST_MAIN_SERVER, book, NULL, NULL, jwt);
                  send_to_server(server_socket, request);
                  response = receive_from_server(server_socket);
                  printf("\n%s\n\n", response);
                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);
                  
            }

            // comanda logout
            if(strcmp("logout\n", buffer) == 0){

                  request = compute_get_request(HOST_MAIN_SERVER, LOGOUT, NULL, cookie, NULL);
                  send_to_server(server_socket, request);
                  response_JWT = receive_from_server(server_socket);
                  printf("\n%s\n\n", response_JWT);
                  server_socket = open_connection(ip, PORT, AF_INET, SOCK_STREAM, 0);

                  // nu mai am nevoie de cookie si token
                  memset(jwt, 0, LINELEN);
                  memset(cookie, 0, LINELEN);
            }
      }
      
return 0;
}





