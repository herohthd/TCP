#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>

#define MAXLINE 1028
char recvLog[MAXLINE];
void log_in(int sockfd){
	int nbyte_recvd;
	char recv_buf[MAXLINE];
	char send_buf[MAXLINE] = "MODE:Login:USERNAME:";
	char username[MAXLINE];
	printf("Enter your username:");
	scanf("%s",username);
	strcat(send_buf,username);
	send(sockfd,send_buf,strlen(send_buf),0);
	nbyte_recvd = recv(sockfd,recvLog,MAXLINE,0);
	recvLog[nbyte_recvd]='\0';
	printf("Received from server:%s\n",recvLog);
	strtok(recvLog,":");
	strcpy(recvLog,strtok(NULL,":"));
	if (strcmp(recvLog, "100") == 0)
	{
		printf("Login successfully\n");
	}
	else
		printf("Username exists\n");
	fflush(stdout);
}
void single_play(int sockfd){
	int nbyte_recvd;
	char recv_buf[MAXLINE];
	char send_buf[MAXLINE];
	char help[MAXLINE];
	char typeHelp[MAXLINE] = "MODE:SinglePLay:HELP:";
	char typeAnswer[MAXLINE] = "MODE:SinglePlay:ANSWER:";
	int helpList[3];
	for(int i=1;i<=2;i++){
		helpList[i]=1;
	}
	while ( (nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0)) != 0) {
		// receive question and 4 ans
		// char *choice = (char *)malloc(sizeof(char));
		recv_buf[nbyte_recvd]='\0';
		strtok(recv_buf,":");
		strcpy(recv_buf,strtok(NULL,":"));
		printf("%s\n",recv_buf);
		fflush(stdout);
		// send choice
		comeback:
		printf("\nYour answer:");
		fflush(stdin);
		scanf("%s",send_buf);
		if(strcmp(send_buf,"Help") == 0){
			char *sendHelp = (char*)malloc(sizeof(char));
			do{
				if(helpList[1] && helpList[2])
					printf("1.50 50\n2.Double point\n");
				else if(helpList[1])
					printf("1.50 50\n");
				else if(helpList[2])
					printf("2.Double point\n");
				else {
					printf("You dont have any help left!");
					goto comeback;
				}
				scanf("%s",help);
			} while(strcmp(help,"1") !=0 && strcmp(help,"2") != 0);
			help[1] = 0;
			strcat(sendHelp,typeHelp);
			if(strcmp(help,"1") == 0){
				strcat(sendHelp,"5050");
			}
			else{
				strcat(sendHelp,"DoublePoint");
			}
			send(sockfd,sendHelp,strlen(sendHelp),0);
			if(strcmp(help,"1") ==0 ){
				char *answer = (char*)malloc(sizeof(char));
				helpList[1] = 0;
				nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0);
				recv_buf[nbyte_recvd]='\0';
				strtok(recv_buf,":");
				strcpy(recv_buf,strtok(NULL,":"));
				printf("%s\n",recv_buf);
				printf("\nYour answer:");
				scanf("%s",send_buf);
				strcat(answer,typeAnswer);
				strcat(answer,send_buf);
				send(sockfd,answer,strlen(answer),0);
			} else if(strcmp(help,"2") == 0) {
				helpList[2] = 0;
				printf("This question's point will be doubled");
				goto comeback;
			}
		}
		else {
			char *answer = (char*)malloc(sizeof(char));
			strcat(answer,typeAnswer);
			strcat(answer,send_buf);
			send(sockfd,answer,strlen(answer),0);
		}
		// receive message code right(200) or wrong(201) or win(202)
		nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0);
		recv_buf[nbyte_recvd]='\0';
		printf("Received from server:%s\n",recv_buf);
		fflush(stdout);
		// inform player
		if(strcmp(recv_buf,"200")==0){
			printf("Your answer is correct!\n");
		} else if(strcmp(recv_buf,"201") == 0) {
			printf("Your answer is wrong\nGame end!\n");
			break;
		} else{
			printf("Congratulations!\nYou has become a millionaire\n");
			break;
		}
	}
	char name[20];char numOfAns[20];char score[20];
	nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0);
	recv_buf[nbyte_recvd]='\0';
	strtok(recv_buf,":");
	strcpy(name,strtok(NULL,":"));
	strcpy(numOfAns,strtok(NULL,":"));
	strcpy(score,strtok(NULL,":"));
	printf("Your score is:\nName:%s\nNumber of right answer:%s\nScore:%s\n",name,numOfAns,score);
}
void duo_play(int sockfd){
	int nbyte_recvd; 
	char send_buf[MAXLINE];char recv_buf[MAXLINE];
	char typeQuestion[MAXLINE] = "MODE:DuoPlay:QUESTIONSTRING:";
	char question[MAXLINE],answerA[MAXLINE],answerB[MAXLINE],answerC[MAXLINE],answerD[MAXLINE],correct_ans[MAXLINE];
	nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0);
	recv_buf[nbyte_recvd]='\0';
	printf("Received from server:%s\n",recv_buf);
	strtok(recv_buf,":");
	strcpy(recv_buf,strtok(NULL,":"));
	if(strcmp(recv_buf,"303") == 0){
		printf("There is no player connected to you!\n");
	} else {
		printf("There is a player connected to you!\n");
		printf("Enter question u want to send:");
		fgets(question,sizeof(question),stdin);
		strcat(typeQuestion,question);
		strcat(typeQuestion,":");

		printf("Enter answer A:");
		fgets(answerA,sizeof(answerA),stdin);
		strcat(typeQuestion,answerA);
		strcat(typeQuestion,":");

		printf("Enter answer B:");
		fgets(answerB,sizeof(answerB),stdin);
		strcat(typeQuestion,answerB);
		strcat(typeQuestion,":");

		printf("Enter answer C:");
		fgets(answerC,sizeof(answerC),stdin);
		strcat(typeQuestion,answerC);
		strcat(typeQuestion,":");

		printf("Enter answer D:");
		fgets(answerD,sizeof(answerD),stdin);
		strcat(typeQuestion,answerD);
		strcat(typeQuestion,":");

		do{
			printf("Enter right answer:");
			scanf("%s",correct_ans);
			// printf("%s",correct_ans);
		} while(strcmp(correct_ans,"A") != 0 && strcmp(correct_ans,"B") != 0 && strcmp(correct_ans,"C") != 0 && strcmp(correct_ans,"D") != 0);
		strcat(typeQuestion,correct_ans);
		send(sockfd,typeQuestion,strlen(typeQuestion),0);
	}
}
void duo_ans(int sockfd) {
	char recv_buf[MAXLINE];
	int nbyte_recvd;
	char duo_ans[MAXLINE];
	char answer[MAXLINE] = "MODE:DuoAnswer:ANSWER:";
	char help[MAXLINE] = "MODE:DuoAnswer:HELP:";
	do{
	printf("Your answer:");
	scanf("%s",duo_ans);
	} while(strcmp(duo_ans,"A") != 0 && strcmp(duo_ans,"B") != 0 && strcmp(duo_ans,"C") != 0 && strcmp(duo_ans,"D") != 0);
	strcat(answer,duo_ans);
	send(sockfd,answer,strlen(answer),0);
	nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0);
	recv_buf[nbyte_recvd]='\0';
	printf("Received from server:%s\n",recv_buf);
	strtok(recv_buf,":");
	strcpy(recv_buf,strtok(NULL,":"));
	if(strcmp(recv_buf,"305") == 0) {
		printf("You have already answered this question!\n");
	} else if(strcmp(recv_buf,"300") == 0) {
		printf("Your answer is correct!\n");
	} else if(strcmp(recv_buf,"301") == 0){
		printf("Your answer is wrong\n");
	}
	fflush(stdout);
}
void add_question(int sockfd)
{
	int nbyte_recvd; 
	char send_buf[MAXLINE];char recv_buf[MAXLINE];
	char typeQuestion[MAXLINE] = "MODE:AddQuestion:QUESTIONSTRING:";
	char question[MAXLINE],answerA[MAXLINE],answerB[MAXLINE],answerC[MAXLINE],answerD[MAXLINE],correct_ans[MAXLINE];
	printf("Enter question u want to send:");
	fgets(question,sizeof(question),stdin);
	strcat(typeQuestion,question);
	strcat(typeQuestion,":");
	
	printf("Enter answer A:");
	fgets(answerA,sizeof(answerA),stdin);
	strcat(typeQuestion,answerA);
	strcat(typeQuestion,":");
	
	printf("Enter answer B:");
	fgets(answerB,sizeof(answerB),stdin);
	strcat(typeQuestion,answerB);
	strcat(typeQuestion,":");
	
	printf("Enter answer C:");
	fgets(answerC,sizeof(answerC),stdin);
	strcat(typeQuestion,answerC);
	strcat(typeQuestion,":");
	
	printf("Enter answer D:");
	fgets(answerD,sizeof(answerD),stdin);
	strcat(typeQuestion,answerD);
	strcat(typeQuestion,":");
	
	do{
		printf("Enter right answer:");
		fgets(correct_ans,sizeof(correct_ans),stdin);
		// printf("%s",correct_ans);
	} while(strcmp(correct_ans,"A\n") != 0 && strcmp(correct_ans,"B\n") != 0 && strcmp(correct_ans,"C\n") != 0 && strcmp(correct_ans,"D\n") != 0);
	strcat(typeQuestion,correct_ans);
	send(sockfd,typeQuestion,strlen(typeQuestion),0);

	nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0);
	recv_buf[nbyte_recvd]='\0';
	printf("Received from server:%s\n",recv_buf);
	strtok(recv_buf,":");
	strcpy(recv_buf,strtok(NULL,":"));
	if(strcmp(recv_buf,"400") == 0){
		printf("Submit successfully!\n");
	} else{
		printf("Submit fail!\n");
	}
}
void score_board(int sockfd){
	int nbyte_recvd; 
	char recv_buf[MAXLINE];
	nbyte_recvd = recv(sockfd,recv_buf,MAXLINE,0);
	recv_buf[nbyte_recvd]='\0';
	printf("Name\tNoAns   Score\n");
	printf("%s\n",recv_buf);
}
void send_recv(int i,int sockfd)
{
	char send_buf[MAXLINE]="MODE:";
	char recv_buf[MAXLINE];
	char mode[MAXLINE];
	int nbyte_recvd;

	if(i == 0)
	{
		fgets(mode,MAXLINE,stdin);
		if(strcmp(mode,"quit\n") == 0)
		{
			exit(0);
		} 
		else if(strcmp(mode,"Login\n") == 0){
			// strcat(send_buf,mode);
			// send(sockfd,send_buf,strlen(send_buf),0);
			do{
			log_in(sockfd);
			} while (strcmp(recvLog, "101") == 0);
		} else if(strcmp(mode,"SinglePlay\n") == 0){
			strcat(send_buf,mode);
			send(sockfd,send_buf,strlen(send_buf),0);
			single_play(sockfd);
		} else if(strcmp(mode,"DuoPlay\n") == 0){
			strcat(send_buf,mode);
			send(sockfd,send_buf,strlen(send_buf),0);
			duo_play(sockfd);
		} else if(strcmp(mode,"DuoAnswer\n") == 0){
			duo_ans(sockfd);
		} else if(strcmp(mode,"AddQuestion\n") == 0){
			add_question(sockfd);
		} else if(strcmp(mode,"ScoreBoard\n") == 0){
			strcat(send_buf,mode);
			send(sockfd,send_buf,strlen(send_buf),0);
			score_board(sockfd);
		} 
	}
	else
	{
		char recvBuf[MAXLINE];
		nbyte_recvd = recv(sockfd,recvBuf,MAXLINE,0);
		recvBuf[nbyte_recvd]='\0';
		strtok(recvBuf,":");
		strcpy(recvBuf,strtok(NULL,":"));
		if(strcmp(recvBuf,"300") == 0){
			printf("Your friend answers correctly!\n");
		} else if(strcmp(recvBuf,"301") == 0) {
			printf("Your friend's answer is wrong!\n");
		} else {
			printf("%s\n",recvBuf);
		}
	}
}

void connect_req(int *sockfd, struct sockaddr_in *server_addr)
{
	if((*sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(3000);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, '\0',sizeof server_addr->sin_zero);

	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("connect");
		exit(1);
	}

}	

int main()
{
	int sockfd,fdmax,i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;

	connect_req(&sockfd,&server_addr);
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(0,&master);
	FD_SET(sockfd,&master);

	fdmax = sockfd;
	while(1)
	{
		read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) == -1)
		{
			perror("select");
			exit(4);
		}

		for(i=0;i<=fdmax;i++)
			if(FD_ISSET(i,&read_fds))
				send_recv(i,sockfd);
	}

	printf("client-quieted\n");
	close(sockfd);
	return 0;
}