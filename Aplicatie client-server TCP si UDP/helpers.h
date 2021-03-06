#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <stdbool.h>

#define DIE(assertion, call_description);				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(EXIT_FAILURE);				\
		}							\
	} while(0)	

#define STOP(assertion, call_description)				\
if (assertion) {					\
	fprintf(stderr, call_description);			\
	return 0;				\
}					
	
/* Dimensiunea maxima a calupului de date */
#define BUFLEN 1500	



// structura pentru un mesaj transmis la server de catre un client TCP
struct msg_to_srv {
    int type;
    char topic[51];
    float sf;
};
//------------------------------------------------------------------------------------------------------------------------------
struct client_online {
	int dim; // dimensiunea vectorului cl
	char topic_n[51];
	int cl[100];
}; //online
//------------------------------------------------------------------------------------------------------------------------------
struct topic_subscribed {
	char topic_n[51];
	float SF;
	
};

struct client_subscribed{
	int fd;
	int dim;  // dimensiunea vectorului tp
	char id[1000];
	struct topic_subscribed tp[100];
}; // clienti
//--------------------------------------------------------------------------------------------------------------------------------

struct topic_cl {
	int SF;
	int message;
	char topic_n[52];
	int dim;
};
//structura vector clienti topic
struct client_topic {
	char id[1000];
	struct topic_cl tp_cl[100];
	int dim;
	
};
// structura vector clienti_la_topic
struct client_la_topic {
	char topic_n[52];
	int nr;
};
//----------------------------------------------------------------------------------------------------------------------------------
// structura unui mesaj primit de la UDP
struct msg_to_recv {
    char topic_n[50];
    uint8_t type;
    char data[1501];
};

// structura pentru un mesaj transmis la un client TCP
struct msg_to_tcp {
    char ip[16];
    uint16_t udp_port;
    char topic_n[51];
    char type[11];
    char data[1501];
};

//topic
#endif //UTILS_H