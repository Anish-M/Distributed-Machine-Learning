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
void error(const char *msg)
{
    perror(msg);
}

int main(int argc, char *argv[])
{
     int sockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }


     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
	else 
		printf("Socket opened successfully\n");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);


	// change this to try every port from 8000 to 8005 until the bind is successful
	for (int i = 0; i < 5; i++) {
		if (bind(sockfd, (struct sockaddr *) &serv_addr,
			  sizeof(serv_addr)) < 0) {
			error("ERROR on binding");
			portno++;
			serv_addr.sin_port = htons(portno);
		} else {
			printf("Socket bound successfully to port %d\n", portno);
			break;
		}
	}

    //  listen(sockfd,5);
    //  clilen = sizeof(cli_addr);
    //  newsockfd = accept(sockfd, 
    //              (struct sockaddr *) &cli_addr, 
    //              &clilen);
    //  if (newsockfd < 0) 
    //       error("ERROR on accept");
	// else
	// 	printf("Socket accepted successfully from client %s\n", inet_ntoa(cli_addr.sin_addr));

    //  bzero(buffer,256);
    //  n = read(newsockfd,buffer,255);
    //  if (n < 0) 
	//  	error("ERROR reading from socket");
	// else
	// 	printf("Here is the message: %s\n",buffer);
	
	int n_clients = 2;
	vector<int> newsockfds;
	vector<thread> threads;

	// mapping of sockets to ip addresses
	vector<string> ip_addrs;

	listen(sockfd, n_clients);
	clilen = sizeof(cli_addr);
	for (int i = 0; i < n_clients; i++) {
		newsockfds.push_back(accept(sockfd, (struct sockaddr *) &cli_addr, &clilen));
		if (newsockfds[i] < 0) {
			error("ERROR on accept");
		} else {
			printf("Socket accepted successfully from client %s\n", inet_ntoa(cli_addr.sin_addr));
			ip_addrs.push_back(inet_ntoa(cli_addr.sin_addr));

		}
	}

	// now all the client connections have been received, create a thread for each client to continuously read from the socket
	vector<thread> client_threads;
	for (int i = 0; i < n_clients; i++) {
		printf("Creating thread for client %s\n", ip_addrs[i].c_str());
		client_threads.push_back(thread([](int newsockfd, string ip_addr) {
			char buffer[256];
			int n;
			
			while (true || buffer[0] != 113) {
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255);
				if (n < 0) {
					error("ERROR reading from socket");
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


	// can write to the same sockets from the main thread
	// write ack to each client
	for (int i = 0; i < n_clients; i++) {
		n = write(newsockfds[i], "ack", 3);
		if (n < 0) {
			error("ERROR writing to socket");
		} else {
			printf("ack sent successfully to client %s\n", ip_addrs[i].c_str());
		}
	}


	printf("Waiting for threads to join\n");
	for (int i = 0; i < n_clients; i++) {
		client_threads[i].join();
		printf("Thread joined for client %s\n", ip_addrs[i].c_str());
	}


     for (int i = 0; i < n_clients; i++) {
		close(newsockfds[i]);
	}
     close(sockfd);
     return 0; 
}