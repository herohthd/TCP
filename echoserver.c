#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

int main (int argc, char **argv)
{
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    //creation of the socket
    listenfd = socket (AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen (listenfd, LISTENQ);

    printf("%s\n","Server running...waiting for connections.");

    for ( ; ; ) {
        
        clilen = sizeof(cliaddr);
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
        

        // GET IP ADDRESS OF CLIENT AND SEND BACK
        // char str[INET_ADDRSTRLEN]; 
        // inet_ntop(AF_INET, &(cliaddr.sin_addr), str, INET_ADDRSTRLEN);
        // // printf("%s\n", str);
        // strcat(str," ");
        // int port = (int) ntohs(cliaddr.sin_port);     
        // char portStr[INET_ADDRSTRLEN];
        // sprintf(portStr, "%d", port);
        // strcat(str,portStr);
        // // printf("%s\n", str);
        // send(connfd,str,sizeof(str),0);
        // // printf("%s\n",portStr); 
        // // printf("Port is: %d\n", (int) ntohs(cliaddr.sin_port));


        printf("%s\n","Received request...");                   
        while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
            char *buffer = (char*)malloc(n);
            strncpy(buffer,buf,n);
            // printf("%s","String received from and resent to the client:");
            puts(buffer);
            send(connfd, buffer, n, 0);
            free(buffer);
        }
        if (n < 0) {
        perror("Read error"); 
        exit(1);
        }
        close(connfd);

    }
    //close listening socket
    close (listenfd); 
}