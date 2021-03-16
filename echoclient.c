#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "student.h"
#include "course.h"
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/

StudentDB root_student = NULL;
int sockfd;
int mark;
int n;
struct sockaddr_in servaddr;
char sendline[MAXLINE], recvline[MAXLINE];
FILE *fp;
char filename[20];
char recvLog[MAXLINE];

void send_file(FILE *fp, int sockfd){
  int n;
  char data[MAXLINE] = {0};

  while(fgets(data, MAXLINE, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, MAXLINE);
  }
}
void login(){
  char username[MAXLINE];
  char password[MAXLINE];
  printf("Enter the user name:");scanf("%s",username);
  send(sockfd,strtok(username,"\n"),strlen(username),0);
  printf("Enter the password:");scanf("%s",password);
  send(sockfd,strtok(password,"\n"),strlen(password),0);
  recv(sockfd,recvLog,MAXLINE,0);
  if(strcmp(recvLog,"success")==0){
    printf("Login successfully");
  }
  else printf("Username or password doesn't exists\n");
}
int main(int argc, char **argv) 
{
  //basic check of the arguments
  //additional checks can be inserted
  if (argc !=2) {
  perror("Usage: TCPClient <IP address of the server"); 
  exit(1);
  }

  //Create a socket for the client
  //If sockfd<0 there was an error in the creation of the socket
  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
  }

  //Creation of the socket
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr= inet_addr(argv[1]);
  servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order

  //Connection of the client to the socket 
  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
  }

  // // Receive ip address and port number from server
  // char recvIP[MAXLINE];
  // recv(sockfd, recvIP, MAXLINE,0);
  // printf("%s", "IP address and port of client are: ");
  // fputs(recvIP, stdout);
  
  do{
  login();
  }while(strcmp(recvLog,"fail")==0);
  comeback:
  do{
    printf("\nEnter the day:");
    scanf("%s",sendline);
    // printf("%s\n",sendline);
    //  send(sockfd, sendline, strlen(sendline), 0);
    if(strcmp(sendline,"Monday")==0){
      send(sockfd,strtok(sendline,"\n"), strlen(sendline), 0);
    }
    else if(strcmp(sendline,"Tuesday")==0){
      send(sockfd,strtok(sendline,"\n"), strlen(sendline), 0);
    }
    else if(strcmp(sendline,"Wednesday")==0){
      send(sockfd,strtok(sendline,"\n"), strlen(sendline), 0);
    }
    else if(strcmp(sendline,"Thursday")==0){
      send(sockfd,strtok(sendline,"\n"), strlen(sendline), 0);
    }
    else if(strcmp(sendline,"Friday")==0){
      send(sockfd,strtok(sendline,"\n"), strlen(sendline), 0);
    }
    else if(strcmp(sendline,"All")==0){
      send(sockfd,strtok(sendline,"\n"), strlen(sendline), 0);
    }
    else{
        printf("Not a day of the week\n");
        goto comeback;
    }
    if ( (n = recv(sockfd, recvline, MAXLINE,0)) == 0){
      //error: server terminated prematurely
      perror("The server terminated prematurely"); 
      exit(4);
    }
    char *buffer = (char*)malloc(n);
    strncpy(buffer,recvline,n);
    fputs(buffer, stdout);
    free(buffer);
    }while (fgets(sendline, MAXLINE, stdin) != NULL);

  // Send file to server

  // printf("Choose the name file to send to server:");
  // scanf("%s",filename);
  // fp = fopen(filename,"r");
  // if(fp == NULL){
  //   printf("%s","Cannot open file\n");
  //   exit(1);
  // }
  // send_file(fp, sockfd);
  // printf("\nFile data sent successfully.\n");

  exit(0);
}