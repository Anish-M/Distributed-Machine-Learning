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
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    bool end = false;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    } else {
        printf("Socket opened successfully\n");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    } else {
        printf("Host found successfully\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    else {
        printf("Connection established successfully\n");
    }

    // create a thread for sockfd to lmessages
    thread t([&](){
        while(!end) {
            bzero(buffer,256);
            n = read(sockfd,buffer,255);
            if (n < 0) 
                 error("ERROR reading from socket");
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
             error("ERROR writing to socket");
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