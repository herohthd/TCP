#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define MAXCHAR 1028

int sockfd;
int mark;
int n;
struct sockaddr_in servaddr;
char sendline[MAXLINE], recvline[MAXLINE];
FILE *fp;
char filename[20];
char recvLog[MAXLINE];
char choice[MAXLINE];

void login()
{
	char username[MAXLINE];
	printf("Enter the user name:");
	scanf("%s", username);
	send(sockfd, strtok(username, "\n"), strlen(username), 0);
	recv(sockfd, recvLog, MAXLINE, 0);
	printf("Received from server:%s\n",recvLog);
	if (strcmp(recvLog, "100") == 0)
	{
		printf("Login successfully");
	}
	else
		printf("Username exists\n");
}
void singlePlay(){
	while ((n = recv(sockfd, recvline, MAXLINE, 0)) != 4) {
		// receive question and 4 ans
		char *buffer = (char *)malloc(n);
		strncpy(buffer, recvline, n);
		buffer[n]=0;
		fputs(buffer, stdout);
		free(buffer);
		// send choice
		printf("\nYour answer:");
		fflush(stdin);
		scanf("%s",choice);
		choice[1] = 0;
		send(sockfd,choice,strlen(choice),0);
		// receive message code right(200) or wrong(201)
		n = recv(sockfd, recvline, MAXLINE, 0);
		char *buffer1 = (char *)malloc(n);
		strncpy(buffer1, recvline, n);
		buffer1[n]=0;
		printf("Received from server:%s\n",buffer1);
		// inform player
		if(strcmp(buffer1,"200")==0){
			printf("Your answer is correct!\n");
		} else if(strcmp(buffer1,"201") == 0) {
			printf("Your answer is wrong\nGame end!\n");
			break;
		} else {
			printf("You has become a millionaire\nCongratulations!\n");
			break;
		}
	}
	// goto comeback;
}
void addQuestion() {
	char str[MAXCHAR];
	char answerA[MAXCHAR];
	char answerB[MAXCHAR];
	char answerC[MAXCHAR];
	char answerD[MAXCHAR];
	char rightAnswer[MAXCHAR];
	printf("Enter question u want to add:");
	getchar();
	fgets(str,sizeof(str),stdin);
	send(sockfd,str,strlen(str),0);
	printf("Enter answer A:");
	fgets(answerA,sizeof(answerA),stdin);
	send(sockfd,answerA,strlen(answerA),0);
	printf("Enter answer B:");
	fgets(answerB,sizeof(answerB),stdin);
	send(sockfd,answerB,strlen(answerB),0);
	printf("Enter answer C:");
	fgets(answerC,sizeof(answerC),stdin);
	send(sockfd,answerC,strlen(answerC),0);
	printf("Enter answer D:");
	fgets(answerD,sizeof(answerD),stdin);
	send(sockfd,answerD,strlen(answerD),0);
	printf("Enter right answer:");
	fgets(rightAnswer,sizeof(rightAnswer),stdin);
	send(sockfd,rightAnswer,strlen(rightAnswer),0);
}
int main(int argc, char **argv)
{
	//basic check of the arguments
	//additional checks can be inserted
	if (argc != 2)
	{
		perror("Usage: TCPClient <IP address of the server");
		exit(1);
	}

	//Create a socket for the client
	//If sockfd<0 there was an error in the creation of the socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Problem in creating the socket");
		exit(2);
	}

	//Creation of the socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(SERV_PORT); //convert to big-endian order

	//Connection of the client to the socket
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("Problem in connecting to the server");
		exit(3);
	}
    do
	{
		login();
	} while (strcmp(recvLog, "101") == 0);
	comeback:
		// do{
			printf("\n1.Single play");
			printf("\n2.Add question to database");
			printf("\n3.Score board");
			printf("\nYour choice:");
			scanf("%s",choice);
		// } 
		// while( choice <= '0' || choice > '4');
		choice[1] = 0;
		send(sockfd,choice,strlen(choice),0);
		if(strcmp(choice,"1") == 0){
			singlePlay();
			goto comeback;
		} else if(strcmp(choice,"2") == 0) {
			addQuestion();
			goto comeback;
		}
}