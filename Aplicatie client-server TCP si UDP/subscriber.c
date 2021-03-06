#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"
#include <math.h>

void usage(char *file) {
	fprintf(stderr, "Usage: %s <ID_Client> <IP_Server> <Port_Server>\n", file);
	exit(0);
}

// creez structura cu topicul si SF-ul si o trimit in main catre server
int create_message(char* buffer, struct msg_to_srv *msg) {
	char cmp1[20] ,cmp2[1000];
	float cmp3;
	// separ linia citita
	sscanf(buffer, "%s %s %f", cmp1, cmp2, &cmp3);
	STOP((cmp1 == NULL || cmp2 == NULL) , "Command is: 'subscribe topic SF' or 'unsubcribe topic.\n");
	STOP(strlen(cmp2) > 50, "Topicul are lungimea maxima de 50.\n");
	if(strcmp(cmp1, "subscribe") == 0){
		msg->type = 1;
		strcpy(msg->topic, cmp2);
		STOP((cmp3 != 1 && cmp3 != 0), "SF trebuie sa fie 1 sau 0.\n");
		msg->sf = cmp3;
	}
	if(strcmp(cmp1, "unsubscribe") == 0){
		msg->type = 0;
		strcpy(msg->topic, cmp2);
	}
	return 1;
}

int main(int argc, char *argv[]) {

	if (argc != 4) {
		usage(argv[0]);
	}

	int sockfd, ret;
	int flag = 1;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];
	if (argc < 3) {
		usage(argv[0]);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // creare socket 
	DIE(sockfd < 0, "socket");
	 struct msg_to_tcp* msg1;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));  // portul serverului in Network Byte Order
	ret = inet_aton(argv[2], &serv_addr.sin_addr);  // adresa IP in Network Byte Order
	DIE(ret == 0, "Wrong IP_Server");

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "Cannot connect to server");  // nu s-a putut conecta

	fd_set read_fds;
	// adauga file descriptorul STDIN_FILENO in multimea read_fds(multime de descriptori pentru citire)	
	FD_SET(STDIN_FILENO, &read_fds); 
	// adauga socketul in multimea read_fds
	FD_SET(sockfd, &read_fds); 
	int fdmax = sockfd; 
	// trimite id-ul catre server
	int ret2 = send(sockfd, argv[1], strlen(argv[1]) + 1, 0)
	DIE(ret2 < 0, "Cannot send ID to server");
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

	while (1) {
		fd_set tmp_fds;
		tmp_fds = read_fds; // multimea de descriptori de citire
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);  // tmp_fds va contine doar descriptorii de pe care s-au primit date
		// se citeste de la tastatura, 
		if(FD_ISSET(STDIN_FILENO, &tmp_fds)) {  // se verifica daca file descriptorul STDIN_FILENO este in multimea tmp_fds, daca intoarce 0 nu este
		memset(buffer, 0, BUFLEN);
		fgets(buffer, BUFLEN - 1, stdin);
			// clientul a dat exit
			if (!strcmp(buffer, "exit\n")) {
                break;
            }
            // creez structura subscribe si unsubscribe
			struct msg_to_srv msg;
            if(create_message(buffer, &msg) == 1) {
                DIE(send(sockfd, (char*) &msg, sizeof(msg), 0) < 0,
                       "Cannot send message to server.\n");
				// verific care mesaj trebuie afisat
                if (msg.type == 1) {
                    printf("subscribed %s\n", msg.topic);
                } else {
                    printf("unsubscribed %s\n", msg.topic);
                }
            }
		}
		if (FD_ISSET(sockfd, &tmp_fds)) {
            memset(buffer, 0, BUFLEN);
           	int received = recv(sockfd, buffer, sizeof(struct msg_to_tcp), 0);
            DIE(received < 0, "Error receiving from server.\n");
			setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
			// e clar ca s-au primit bytes
            if (received) {
                 msg1 = (struct msg_to_tcp*)buffer;
           		 printf("%s:%hu - %s - %s - %s\n", msg1->ip, msg1->udp_port, msg1->topic_n, msg1->type, msg1->data);    
			} else break;
        } 
	}
	close(sockfd);
	return 0;
}
