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
using namespace std;

int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];
thread reading_t;
bool end_thread;

void open_socket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("perror opening socket");
        exit(0);
    } else {
        printf("Socket opened successfully\n");
    }
}

void get_host() {
    server = gethostbyname("hydra.cs.utexas.edu");
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
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        perror("eerror connecting");
        exit(0);
    } else {
        printf("Connection established successfully\n");
    }
}

void create_reading_thread() {
    reading_t = thread([&](){
        while(!end_thread) {
            bzero(buffer,256);
            n = read(sockfd,buffer,255);
            if (n < 0) 
                 perror("perror reading from socket");
            else 
                 printf("Message received: %s\n",buffer);
        }
    });
}

void join_thread() {
    reading_t.join();
    close(sockfd);
}

int main(int argc, char *argv[])
{

    end_thread = false;
    portno = 8000;

    open_socket();
    get_host();
    establish_connection();
    create_reading_thread();

    
    // send_thread a message to the server, continuously until the user types "end_thread"
    while(1) {
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             perror("perror writing to socket");
        else 
            printf("Message sent: %s\n",buffer);
        if (strcmp(buffer, "end_thread\n") == 0) {
            // send_thread 'q' to the server to tell it to quit
            n = write(sockfd, "q", 1);
            break;
        }
    }

    // kill all threads and close the socket
    end_thread = true;
    
    join_thread();
    
    return 0;
}