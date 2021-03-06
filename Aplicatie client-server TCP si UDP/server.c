#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "helpers.h"
#include <math.h>

void usage(char *file)
{
	fprintf(stderr, "Usage: %s <PORT_DORIT>\n", file);
	exit(0);
}

void close_all_sockets(fd_set *read_fds, int fdmax) {
    for (int i = 2; i <= fdmax; i++) {
        if (FD_ISSET(i, read_fds)) {
            close(i);
        }
    }
}
// clientii care dau subscribe
void client_subscribed(int *nr_online, int *nr_clienti, struct msg_to_srv *serv_msg, int filedescriptor, 
					struct client_online online[100], struct client_subscribed clienti[100]){
	int i = 0;
	int j = 0;
	int ok = 1;
	for(i = 0; i < *nr_online; i++){
		//verificam daca exista deja topicul
		if(strcmp(online[i].topic_n, serv_msg->topic) == 0){
			//adaugam filedescriptorul pe pozitia care trebuie
			int t = online[i].dim;
			online[i].cl[t] = filedescriptor;
			online[i].dim++;
			ok = 0;
			break;
		}
	}

	//topic nou, tb adaugat
	if(ok == 1){
		//adaugam topicul in vectorul de online
		strcpy(online[*nr_online].topic_n , serv_msg->topic);
		int t = online[*nr_online].dim;
		online[*nr_online].cl[t] = filedescriptor;
		online[*nr_online].dim++;
		*nr_online = *nr_online + 1;
	}


//clients
	for(j = 0; j < *nr_clienti; j++){
		//verificam daca exista deja filedescriptorul
		if(clienti[j].fd == filedescriptor){
			//adaugam topicul si sf-ul pe pozitiile care trebuie
			int t = clienti[j].dim;
			strcpy(clienti[j].tp[t].topic_n, serv_msg->topic);
			clienti[j].tp[t].SF = serv_msg->sf;
			clienti[j].dim++;
			break;

		}
	}
}
// clientii care dau unsubscribe
void client_unsubscribed(int *nr_online, int *nr_clienti, char topic[51], int filedescriptor, 
					struct client_online online[100], struct client_subscribed clienti[100]){
//online
			int i, j, ok = 1;
		for(i = 0; i < *nr_online; i++){
			//vedem unde este topicul si daca exista
			if(strcmp(online[i].topic_n, topic) == 0){
				for(j = 0; j < online[i].dim; j++){
					//am gasit filedescriptorul ce tb eliminat
					if(online[i].cl[j] == filedescriptor)
							break;	
				}
				if(j < online[i].dim){
					online[i].dim--;
					for(int k = j; k < online[i].dim; k++)
						online[i].cl[k] = online[i].cl[k+1];
				}
				//daca nu mai am niciun fd in topic, stergem tot topicul
				if(online[i].dim == 0){
					ok = 0;
					break;
				}
			}	
		}
		if(i < *nr_online && ok == 0){
			*nr_online = *nr_online - 1;
			for(int k = i; k < *nr_online; k++){
				memcpy(&online[k], &online[k+1], sizeof(struct client_online));
			}
		}

//client
		for(i = 0; i < *nr_clienti; i++){
			//vedem unde este file_descriptorul
			if(clienti[i].fd == filedescriptor){
				for(j = 0; j < clienti[i].dim; j++){
					//cautam topicul asociat unui filedescriptor
					if(strcmp(clienti[i].tp[j].topic_n, topic) == 0)
							break;	
				}
				//eliminam topicul cu tot cu sf
				if(j < clienti[i].dim){
					clienti[i].dim--;
					for(int k = j; k < clienti[i].dim; k++){
						memcpy(&clienti[i].tp[k], &clienti[i].tp[k+1], sizeof(struct topic_subscribed));
					}
				}
			}
		}
}
// caz in care primesc de la un client tcp exit, trebuie sa elimin clientul din vectorul de clienti
// creez un nou vector de topicuri in care tin toate topicele 
// creez un vector de client_la_topic care imi zice cati clienti am la un topic cu SF-ul 1
void descriptor_delete( int *nr_clienti, int *nr_topic, int *nr_la_topic, int filedescriptor, 
				 struct client_subscribed clienti[100], struct client_topic TOPIC[100],
				 struct client_la_topic NR_TOPIC[100]){
		
		int i,  ok = 1;
		for(i = 0; i < *nr_clienti; i++){
			//filedescriptorul ce trebuie eliminat
			if(clienti[i].fd == filedescriptor){
					//verific care sf este 1
				for(int j = 0; j < clienti[i].dim; j++){
							//topic ce trebuie bagat
						if(clienti[i].tp[j].SF == 1){
							//adaugam topic nou;
							strcpy(TOPIC[*nr_topic].id, clienti[i].id);
							int t = TOPIC[*nr_topic].dim;
							strcpy(TOPIC[*nr_topic].tp_cl[t].topic_n, clienti[i].tp[j].topic_n);
							TOPIC[*nr_topic].tp_cl[t].SF = 1;
							TOPIC[*nr_topic].tp_cl[t].message = 0;
							TOPIC[*nr_topic].dim++;

							//vedem daca exista deja topicul
							for(int k = 0; k < *nr_la_topic; k++){
								if(strcmp(NR_TOPIC[k].topic_n, clienti[i].tp[j].topic_n) == 0){
									NR_TOPIC[k].nr++;
									ok = 0;
									break;
									
								} 
							}
							if(ok == 1){
							strcpy(NR_TOPIC[*nr_la_topic].topic_n, clienti[i].tp[j].topic_n);
							*nr_la_topic = *nr_la_topic + 1;
							}
							
						}
						else 
						{
							strcpy( TOPIC[*nr_topic].id, clienti[i].id);
							int t = TOPIC[*nr_topic].dim;
							strcpy(TOPIC[*nr_topic].tp_cl[t].topic_n, clienti[i].tp[j].topic_n);
							TOPIC[*nr_topic].tp_cl[t].SF = 0;
							TOPIC[*nr_topic].tp_cl[t].message = -1;
							TOPIC[*nr_topic].dim++;
						}
				}
				if(clienti[i].dim != 0){
				*nr_topic = *nr_topic + 1;
				}
				break;
			}	
		}
	// elimin filedescriptorul din vectorul de clienti
		if(i < *nr_clienti){
			*nr_clienti = *nr_clienti - 1;
			for(int k = i; k < *nr_clienti; k++){
				memcpy(&clienti[i], &clienti[i+1], sizeof(struct client_subscribed));
			}
		}		
}

//adaug in vectorul de clienti un nou filedescriptor
void client_conectat(int *nr_clienti, int filedescriptor, char id[1000], struct client_subscribed clienti[100]){
			strcpy(clienti[*nr_clienti].id, id);
			clienti[*nr_clienti].fd = filedescriptor;
			*nr_clienti = *nr_clienti + 1;
}


						//ceea ce am primit		//ceea ce trimitem
void decodificare (struct msg_to_tcp *msg_tcp, struct msg_to_recv* msg_udp){
		uint32_t nr1;
		double nr2;
		// int
		if(msg_udp->type == 0){
				nr1 = ntohl(*(uint32_t*)(msg_udp->data + 1));
			strcpy(msg_tcp->type, "INT");
			if(msg_udp->data[0]) {
				nr1 = nr1 * (-1);
				sprintf(msg_tcp->data, "%d", nr1);
			
			} else sprintf(msg_tcp->data, "%d", nr1);
		}
		//short real
		if(msg_udp->type == 1) {
			strcpy(msg_tcp->type, "SHORT_REAL");
			int y = *(uint16_t*)(msg_udp->data); 
          	nr2 = abs(ntohs(y));
            nr2 = nr2 / 100;
            sprintf(msg_tcp->data, "%.2f", nr2);        
		}
		// float
        if(msg_udp->type == 2) {
			strcpy(msg_tcp->type, "FLOAT");
            nr2 = ntohl(*(uint32_t*)(msg_udp->data + 1));
			int t = pow(10,msg_udp->data[5]);
            nr2 = nr2 / t;
			if(msg_udp->data[0]) {
				nr2 = nr2 * (-1);
				sprintf(msg_tcp->data, "%lf", nr2);
			
			} else sprintf(msg_tcp->data, "%lf", nr2);
		}
		// string
       if(msg_udp->type == 3){
            strcpy(msg_tcp->type, "STRING");
            strcpy(msg_tcp->data, msg_udp->data);
    }

strcpy(msg_tcp->topic_n, msg_udp->topic_n);
}

int main(int argc, char *argv[])
{
	struct msg_to_srv* serv_msg;
	int sock_udp, sock_tcp, new_sock_tcp, port_dorit, fdmax; 
	int flag = 1, ok = 1, i = 0, ret1, ret2 , MAX_CLI = 10000000;
	char buffer[BUFLEN];
	struct sockaddr_in udp_str, tcp_str , tcp_addr;
	socklen_t tcp_length = sizeof(struct sockaddr);

//subscribe & unsubscribe & descriptor_delete------------------------------------------------------------------------
	struct client_online online[100];
	struct client_subscribed clienti[100];	
	int nr_online = 0, nr_clienti = 0, nr_topic = 0, nr_la_topic = 0;
	struct client_topic TOPIC[100];
	struct client_la_topic NR_TOPIC[100];
	struct msg_to_tcp msg_tcp;
	struct msg_to_recv* msg_udp;
//-------------------------------------------------------------------------------------------------------------------
	if (argc != 2) {
		usage(argv[0]);
	}

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
				// valoare maxima fd din multimea read_fds

	//create TCP socket
	sock_tcp = socket(PF_INET, SOCK_STREAM, 0);
	DIE(sock_tcp < 0, "Cannot create TCP socket");

	port_dorit = atoi(argv[1]);
    DIE(port_dorit < 1024, "Too small port number\n");

    //complete TCP struct
    memset((char *) &tcp_str, 0, sizeof(tcp_str));
	tcp_str.sin_family = AF_INET;
	tcp_str.sin_port = htons(port_dorit);
	tcp_str.sin_addr.s_addr = INADDR_ANY;

	sock_udp = socket(PF_INET, SOCK_DGRAM, 0);
	DIE(sock_udp < 0, "Cannot create UDP socket");

	//complete UDP struct
	memset((char *) &udp_str, 0, sizeof(udp_str));
	udp_str.sin_family = AF_INET;
	udp_str.sin_port = htons(port_dorit);
	udp_str.sin_addr.s_addr = INADDR_ANY;

	//UDP and TCP bind
	ret1 = bind(sock_udp, (struct sockaddr *) &udp_str, sizeof(struct sockaddr));
	DIE(ret1 < 0, "Cannot bind UDP");
	ret2 = bind(sock_tcp, (struct sockaddr *) &tcp_str, sizeof(struct sockaddr));
	DIE(ret2 < 0, "Cannot bind TCP");
	ret1 = listen(sock_tcp, MAX_CLI);
	DIE(ret1 < 0, "Cannot listen to TCP socket");

	// golesc multimile de descriptori si inserez socketii tcp si udp in multimea de descriptori de citire
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	FD_SET(sock_tcp, &read_fds);
	FD_SET(sock_udp, &read_fds);
	FD_SET(0, &read_fds);
	fdmax = sock_tcp;

	// atunci cand primesc exit de la un client TCP si dau break, ies din while
	while(ok){
		tmp_fds = read_fds;
		ret1 = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL); // va contine doar descriptorii pe care s-au primit date
															 // la server, ne intereseaza doar multimea de citire
		DIE(ret1 < 0, "Cannot select");
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				// daca s-a citit in server ceva de la tastatura
				if(i == 0){
					if(strcmp(fgets(buffer, BUFLEN - 1, stdin), "exit\n") == 0){
						ok = 0;
						break;
					} else {
						printf("Only exit\n");
					}
					// am primit un socket nou de la un client tcp si il accept
				} else if(i == sock_tcp){
					new_sock_tcp = accept(sock_tcp, (struct sockaddr *) &tcp_addr, &tcp_length)
					DIE(new_sock_tcp < 0, "Cannot accept new client.\n");
					FD_SET(new_sock_tcp, &read_fds);
					if (new_sock_tcp > fdmax) {
						fdmax = new_sock_tcp;
					}

                    setsockopt(sock_tcp, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
					int var = recv(new_sock_tcp, buffer, BUFLEN - 1, 0);
					DIE(var < 0, "Cannot receive client ID.\n");
					// New client (CLIENT_ID) connected from IP:PORT.
					 printf("New client %s connected from %s:%d.\n", buffer,inet_ntoa(tcp_addr.sin_addr), ntohs(tcp_addr.sin_port));
					 client_conectat(&nr_clienti, new_sock_tcp , buffer, clienti);

					// socket udp
				} else if(i == sock_udp){
					memset(buffer, 0, BUFLEN);
					int h = recvfrom(sock_udp, buffer, BUFLEN, 0, (struct sockaddr*) &udp_str, &tcp_length);
					DIE(h < 0 , "Nothing received");
					// am completat deja udp_port si ip ul;
					msg_tcp.udp_port = ntohs(udp_str.sin_port);
                    strcpy(msg_tcp.ip, inet_ntoa(udp_str.sin_addr));
                    msg_udp = (struct msg_to_recv*)buffer;
					// 
					decodificare(&msg_tcp, msg_udp);
						for(int i = 0 ; i < nr_clienti; i++){
							for(int j = 0; j < clienti[i].dim; j++){
								if(strcmp(clienti[i].tp[j].topic_n, msg_tcp.topic_n) == 0){
									int z = send(clienti[i].fd, (char*) &msg_tcp, sizeof(struct msg_to_tcp), 0);
									DIE(z < 0 , "Nu s-a putut trimite mesajul subscriberului\n");
									// setsockopt(serv_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
								}
							}
						}
				} else {
					// se primeste o comanda de la un client TCP
					int received = recv(i, buffer, BUFLEN - 1, 0);
					DIE(received < 0, "No bytes received from a TCP\n");
					// caz in care nu mai primesc niciun byte de la vreun client TCP
					if (received) {
							// s-a primit o comanda de subscribe sau unsubscribe
							serv_msg = (struct msg_to_srv*)buffer;
						if (serv_msg->type == 1) {
							client_subscribed(&nr_online, &nr_clienti, serv_msg, i, online, clienti);
								
						} else {
							client_unsubscribed(&nr_online, &nr_clienti, serv_msg->topic, i, online, clienti);
						}	
					} else {
							for(int j = 0; j < nr_clienti; j++) {
								if(clienti[j].fd == i){
										printf("Client %s disconnected.\n", clienti[j].id);
										break;
								}
							}
							// stergem filedescriptorul cu totul
						descriptor_delete(&nr_clienti, &nr_topic, &nr_la_topic, i, clienti, TOPIC, NR_TOPIC);
						FD_CLR(i, &read_fds);
						// actualizam fdmax
						  for (int l = fdmax; l > 2; l--) {
        					if(FD_ISSET(l, &read_fds)) {
           						fdmax = l;
            					break;
        					}
    					}
						close(i);
					}
				}
			}
		}
	}
	// se inchid toti socketii inca in folosinta
    close_all_sockets(&read_fds, fdmax);
	return 0;
}