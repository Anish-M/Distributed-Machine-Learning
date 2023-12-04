#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <thread>
#include <vector>
// #include "../neural_network/Network.hpp"



using namespace std;

// ################ SOCKET PROGRAMMING ATTRIBUTES ########################
int sockfd, port, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[2560000];
thread reading_t;
bool end_thread;
// ######################################################


void open_socket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("perror opening socket");
        exit(0);
    } else {
        printf("Socket opened successfully\n");
    }
}

void get_host(char* host) {
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"perror, no such host\n");
        exit(0);
    } else {
        printf("Host found successfully\n");
    }
}

void establish_connection() {

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        perror("eerror connecting");
        exit(0);
    } else {
        printf("Connection established successfully\n");
    }
}



void join_thread() {
    reading_t.join();
    close(sockfd);
}


void send_message(char* message) {
    n = write(sockfd,message,strlen(message));
    if (n < 0) 
         perror("perror writing to socket");
}


void send_results() {
    while(1) {
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             perror("perror writing to socket");
        if (strcmp(buffer, "end_thread\n") == 0) {
            // send_thread 'q' to the server to tell it to quit
            n = write(sockfd, "q", 1);
            break;
        }
    }
}