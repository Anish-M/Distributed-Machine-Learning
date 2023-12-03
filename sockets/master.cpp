
#ifndef master_cpp
#define master_cpp
/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <thread>
#include <vector>

using namespace std;


// ################ SOCKET PROGRAMMING ATTRIBUTES ########################
int sockfd, portno;
socklen_t clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;

vector<int> newsockfds;
vector<thread> threads;

// mapping of sockets to ip addresses
vector<string> ip_addrs;

vector<thread> client_threads;

int n_clients;
// ######################################################


void open_socket() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("perror opening socket");
	} else {
		printf("Socket opened successfully\n");
	}
}

void bind_socket() {
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 8000;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	for (int i = 0; i < 5; i++) {
		if (bind(sockfd, (struct sockaddr *) &serv_addr,
			  sizeof(serv_addr)) < 0) {
			perror("perror on binding");
			portno++;
			serv_addr.sin_port = htons(portno);
		} else {
			printf("Socket bound successfully to port %d\n", portno);
			break;
		}
	}
}

void map_sockets_to_ip_addrs() {
	listen(sockfd, n_clients);
	clilen = sizeof(cli_addr);
	cout << "Waiting for " << n_clients << " clients to connect" << endl;
	for (int i = 0; i < n_clients; i++) {
		newsockfds.push_back(accept(sockfd, (struct sockaddr *) &cli_addr, &clilen));
		if (newsockfds[i] < 0) {
			perror("perror on accept");
		} else {
			printf("Socket accepted successfully from client %s\n", inet_ntoa(cli_addr.sin_addr));
			ip_addrs.push_back(inet_ntoa(cli_addr.sin_addr));

		}
	}
}

void create_client_threads() {
	// now all the client connections have been received, create a thread for each client to continuously read from the socket
	for (int i = 0; i < n_clients; i++) {
		printf("Creating thread for client %s\n", ip_addrs[i].c_str());
		client_threads.push_back(thread([](int newsockfd, string ip_addr) {
			char buffer[256];
			int n;
			
			while (true || buffer[0] != 113) {
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255);
				if (n < 0) {
					perror("perror reading from socket");
				} else {
					printf("%s: %s\n", ip_addr.c_str(), buffer);
					if (buffer[0] == 113) {
						printf("buffer[0] = %d\n", buffer[0]);
						printf("Client %s has disconnected\n", ip_addr.c_str());
						break;
					}
				}
			}
			printf("Thread exiting for client %s\n", ip_addr.c_str());
		}, newsockfds[i], ip_addrs[i]));
	}
}

void send_message_to_client(int client_index, string message) {
	n = write(newsockfds[client_index], message.c_str(), message.length());
	if (n < 0) {
		perror("perror writing to socket");
	} else {
		printf("Message sent successfully to client %s\n", ip_addrs[client_index].c_str());
	}
}

void join_threads() {
	printf("Waiting for threads to join\n");
	for (int i = 0; i < n_clients; i++) {
		client_threads[i].join();
		printf("Thread joined for client %s\n", ip_addrs[i].c_str());
	}
}

void close_sockets() {
	for (int i = 0; i < n_clients; i++) {
		close(newsockfds[i]);
	}
	close(sockfd);
}
// int main(int argc, char *argv[])
// {

//     open_socket();

//     bind_socket();
	
// 	map_sockets_to_ip_addrs();

// 	create_client_threads();


// 	// can write to the same sockets from the main thread
// 	// write ack to each client
// 	for (int i = 0; i < n_clients; i++) {
// 		send_message_to_client(i, "ack");
// 	}


// 	join_threads();

	
//     close_sockets();
//      return 0; 
// }

#endif