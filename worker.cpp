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

int main(int argc, char *argv[])
{

    bool end = false;
    portno = 8000;

    open_socket();
    get_host();
    establish_connection();



    // create a thread for sockfd to lmessages
    thread t([&](){
        while(!end) {
            bzero(buffer,256);
            n = read(sockfd,buffer,255);
            if (n < 0) 
                 perror("perror reading from socket");
            else 
                 printf("Message received: %s\n",buffer);
        }
    });

    
    // send a message to the server, continuously until the user types "end"
    while(1) {
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             perror("perror writing to socket");
        else 
            printf("Message sent: %s\n",buffer);
        if (strcmp(buffer, "end\n") == 0) {
            // send 'q' to the server to tell it to quit
            n = write(sockfd, "q", 1);
            break;
        }
    }

    // kill all threads and close the socket
    end = true;
    t.join();

    close(sockfd);
    return 0;
}