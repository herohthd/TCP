#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <stdbool.h>
#include<netdb.h>
#include "question.h"
#include "player.h"

#define MAXLINE 1028
#define LISTENQ 8      
#define SERV_PORT 3000 /*port*/
#define MAXCLIENT 2
#define MAXQUESTION 5
#define NUMBEROFANSWER 4
PlayerDB rootPlayer = NULL;
QuestionDB rootQuestion = NULL;
PlayerDB user = NULL;
char flag[MAXLINE];
char duo_answer[MAXLINE];
int countQuestion = 0;
int isWrong = 0;
int fds[20];
int duo_fds[20];
int countfd = 0;
int max_single_score[20] = {0};
QuestionDB readQuestion(char *filename){
    FILE *fin = fopen(filename, "r");
    if (fin == NULL)
    {
        printf("Not exist %s.\n", filename);
        return 0;
    }
    while (!feof(fin))
    {
        QuestionDB n = (QuestionDB)malloc(sizeof(struct Question));
        // READ SPECIAL INPUT WITH COMMA AND SPACE
        fgets(n->question,sizeof(n->question),fin);
        fgets(n->answerA,sizeof(n->answerA),fin);
        fgets(n->answerB,sizeof(n->answerB),fin);  
        fgets(n->answerC,sizeof(n->answerC),fin);  
        fgets(n->answerD,sizeof(n->answerD),fin);   
        fscanf(fin,"%s\n",n->rightAns); 
        rootQuestion = appendQuestion(rootQuestion,n->question,
                                        n->answerA,n->answerB,
                                        n->answerC,n->answerD,
                                        n->rightAns);
        free(n);
    }
    fclose(fin);
    return rootQuestion;
}
PlayerDB readPlayer(char *filename)
{
  FILE *fin = fopen(filename, "r");
    if (fin == NULL)
    {
        printf("Not exist %s.\n", filename);
        return 0;
    }
    while (!feof(fin))
    {
        PlayerDB n = (PlayerDB)malloc(sizeof(struct Player));
        // READ SPECIAL INPUT WITH COMMA AND SPACE
        fscanf(fin,"%s\n",n->name);
        rootPlayer = appendPlayer(rootPlayer,n);
        free(n);
    }
    fclose(fin);
    return rootPlayer;  
}
void send_to_all(int j,int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master)
{
	if(FD_ISSET(j,master)) 
	{
		if(j != sockfd && j != i)
		{
			if(send(j,recv_buf,nbytes_recvd,0) == -1)
			{
				printf("perror");
			}
		}
	}
}
void log_in(int i,char username[]){
    int nbytes_recvd,j;
	char recv_buf[MAXLINE];
	user = registered(rootPlayer, username);
	if (!user) {
        char buf[MAXLINE] = "CODE:";
        strcat(buf, "100");
        strcpy(flag,"100");
        PlayerDB cur = (PlayerDB)malloc(sizeof(struct Player));
        strcpy(cur->name,username);
        cur->numOfRightAns = 0;
        cur->score = 0;
        appendPlayer(rootPlayer,cur);
        addPlayerToDatabase("player.txt",cur);
        user = registered(rootPlayer,username);
        // puts("SUCCESS");
        send(i, buf, strlen(buf), 0);
    }else {
        char buf[MAXLINE] = "CODE:";
        // puts("FAIL");
        strcat(buf, "101");
        strcpy(flag,"101");
        send(i, buf, strlen(buf), 0);
    }
}
char* fiftyFifty(int i,QuestionDB qCur,PlayerDB uCur){
	int nbytes_recvd,j;
	char recv_buf[MAXLINE];char type[MAXLINE];
    char question[MAXLINE] = "QUESTIONSTRING:";
	char *ans =(char*)malloc(sizeof(char));
    int indexOfRightAns = 0;int deletedAns1;int deletedAns2;
    char ansA[MAXLINE];char ansB[MAXLINE];char ansC[MAXLINE];char ansD[MAXLINE];
    if(strcmp(qCur->rightAns,"A")==0){
        indexOfRightAns = 0;
    } else if(strcmp(qCur->rightAns,"B")==0){
        indexOfRightAns = 1;
    } else if(strcmp(qCur->rightAns,"C")==0){
        indexOfRightAns = 2;
    } else{
        indexOfRightAns = 3;
    }
    do{
        deletedAns1 = rand() % (NUMBEROFANSWER - 1);
    } while(deletedAns1 == indexOfRightAns);
    do{
        deletedAns2 = rand() & (NUMBEROFANSWER - 1);
    } while(deletedAns2 == deletedAns1 || deletedAns2 == indexOfRightAns);
    strcpy(ansA,qCur->answerA);
    strcpy(ansB,qCur->answerB);
    strcpy(ansC,qCur->answerC);
    strcpy(ansD,qCur->answerD);
    switch(deletedAns1){
        case 0:
            strcpy(ansA,"");
            break;
        case 1:
            strcpy(ansB,"");
            break;
        case 2:
            strcpy(ansC,"");
            break;
        case 3:
            strcpy(ansD,"");
            break;
        default:
            break;
    }
    switch(deletedAns2){
        case 0:
            strcpy(ansA,"");
            break;
        case 1:
            strcpy(ansB,"");
            break;
        case 2:
            strcpy(ansC,"");
            break;
        case 3:
            strcpy(ansD,"");
            break;
        default:
            break;
    }
    char *temp1 = strdup(qCur->question);
    char *temp2 = strdup(ansA);
    char *temp3 = strdup(ansB);
    char *temp4 = strdup(ansC);
    char *temp5 = strdup(ansD);
    char *cat1 = strcat(temp1,temp2);
    char *cat2 = strcat(cat1,temp3);
    char *cat3 = strcat(cat2,temp4);
    char *cat4 = strcat(cat3,temp5);
    char *cat5 = strcat(question,cat4);
    send(i,cat5,strlen(cat5),0);
    nbytes_recvd = recv(i,recv_buf,MAXLINE,0);
	recv_buf[nbytes_recvd]='\0';
    puts(recv_buf);
    strtok(recv_buf,":");
    strtok(NULL,":");
	strcpy(type,strtok(NULL,":"));
    strcpy(ans,strtok(NULL,":"));
	strcpy(ans,strtok(ans,"\n"));
	return ans;
}
void checkAndUpdate(int i,QuestionDB qCur,PlayerDB uCur,char *ans,int doubleOrNot){
    char correct[MAXLINE] = "200";
    char wrong[MAXLINE] = "201";
    char win[MAXLINE] = "202"; 
    if(checkAnswer(qCur,ans)){
        if( countQuestion == MAXQUESTION ) {
            uCur->numOfRightAns++;
            if(doubleOrNot){
                uCur->score += 10*countQuestion*2;
            } else {
                uCur->score +=10*countQuestion;
            }
            // Win the game
            send(i,win,strlen(win),0);
        }
        else{
            uCur->numOfRightAns++;
            if(doubleOrNot){
                uCur->score += 10*countQuestion*2;
            } else {
                uCur->score +=10*countQuestion;
            }
            qCur = qCur->next;
            send(i,correct,strlen(correct),0);
        }
    }
    else{
        send(i,wrong,strlen(wrong),0);
        isWrong = 1;
    }
}
void send_a_question(int i, fd_set *master,int sockfd,int fdmax,QuestionDB qRoot, PlayerDB uCur, int num) {
    char single_play_ans[MAXLINE]; 
	int nbytes_recvd,j;
	char recv_buf[MAXLINE];
    char question[MAXLINE] = "QUESTIONSTRING:";
    int index = 0;
    char type[MAXLINE];
    QuestionDB qCur = qRoot;
    while(index != num) {
        index++;
        qCur = qCur->next;
    }
    char *temp1 = strdup(qCur->question);
    char *temp2 = strdup(qCur->answerA);
    char *temp3 = strdup(qCur->answerB);
    char *temp4 = strdup(qCur->answerC);
    char *temp5 = strdup(qCur->answerD);
    char *cat1 = strcat(temp1,temp2);
    char *cat2 = strcat(cat1,temp3);
    char *cat3 = strcat(cat2,temp4);
    char *cat4 = strcat(cat3,temp5);
    char *cat5 = strcat(question,cat4);
    send(i,cat5,strlen(cat5),0);
    fflush(stdout);

    // receive answer and check
    nbytes_recvd = recv(i,recv_buf,MAXLINE,0);
	recv_buf[nbytes_recvd]='\0';
	printf("%s\n",recv_buf);
    strtok(recv_buf,":");
    strtok(NULL,":");
    strcpy(type,strtok(NULL,":"));
    strcpy(single_play_ans,strtok(NULL,":"));
    if(strcmp(type,"HELP") == 0){
        if(strcmp(single_play_ans,"5050") == 0){
            char *helpAns = fiftyFifty(i,qCur,uCur);
            checkAndUpdate(i,qCur,uCur,helpAns,0);
        }
        else if(strcmp(single_play_ans,"DoublePoint") == 0){
            char *answer = (char*)malloc(sizeof(char));
            nbytes_recvd = recv(i,recv_buf,MAXLINE,0);
            recv_buf[nbytes_recvd]='\0';
            printf("%s\n",recv_buf);
            strtok(recv_buf,":");
            strtok(NULL,":");
            strtok(NULL,":");
            strcpy(answer,strtok(NULL,":"));
            checkAndUpdate(i,qCur,uCur,answer,1);
        }
    }
    else {
        checkAndUpdate(i,qCur,uCur,single_play_ans,0);
    }
}

void single_play(int i, fd_set *master,int sockfd,int fdmax,QuestionDB qRoot,PlayerDB uCur){
	bool arr[21] = {0};
    int r;
    char result[MAXLINE]="RESULT:";char numOfAns[10];char score[10];
    countQuestion = 0;
	isWrong = 0;  // If  wrong write result and wait for menu choice
    uCur->numOfRightAns = 0;
    uCur->score = 0;
    while(countQuestion < MAXQUESTION && isWrong == 0) {
        printf("Number of question:%d\n",countQuestion);
        countQuestion++;
        do{
            r = rand() % MAXQUESTION;
        } while(arr[r]);
        arr[r] = 1;
        send_a_question(i,master,sockfd,fdmax,qRoot,uCur,r);
	}
    // if(uCur->score > max_single_score[i]) {
    //     max_single_score[i] = uCur->score;
	    addPlayerResult("result.txt",uCur);
    // }
    sprintf(numOfAns,"%d",uCur->numOfRightAns);
    sprintf(score,"%d",uCur->score);
    strcat(result,uCur->name);
    strcat(result,":");
    strcat(result,numOfAns);
    strcat(result,":");
    strcat(result,score);
    send(i,result,strlen(result),0);
}
int get_index_fd(int i){
    for(int j=0;j<LISTENQ;j++){
        if(fds[j] ==i){
            return j;
        }
    }
    return -1;
}
int get_pair_fd(int i){
    int index = get_index_fd(i);
    // printf("%d",index);
    if(index%2 ==0){
        if(fds[index+1]){
            return fds[index+1];
        }
    }
    if(index%2==1){
        if(fds[index-1]){
            return fds[index-1];
        }
    }
    return -1;
}
void duo_play(int i, fd_set *master,int sockfd,int fdmax){
	int nbytes_recvd,j;
    char question_string[20] = "QUESTIONSTRING:";
	char recv_buf[MAXLINE],buf[MAXLINE],send_buf[MAXLINE];
	char question[MAXLINE],ansA[20],ansB[20],ansC[20],ansD[20];
    // mark the other player hasnt answer
    duo_fds[get_pair_fd(i)] = 0;

	nbytes_recvd = recv(i,recv_buf,MAXLINE,0);
	recv_buf[nbytes_recvd]='\0';
	printf("%s\n",recv_buf);
    strtok(recv_buf,":");
    strtok(NULL,":");
    strtok(NULL,":");
    strcpy(question,strtok(NULL,":"));
    strcpy(ansA,strtok(NULL,":"));
    strcpy(ansB,strtok(NULL,":"));
    strcpy(ansC,strtok(NULL,":"));
    strcpy(ansD,strtok(NULL,":"));
    strcpy(duo_answer,strtok(NULL,":"));
    
    // concatenating question and 4 answers
    char sendBuf[MAXLINE] = "";
	strcat(sendBuf,question);
	strcat(sendBuf,ansA);
	strcat(sendBuf,ansB);
	strcat(sendBuf,ansC);
	strcat(sendBuf,ansD);

    // send question to the other player
    char *sendQuestion=(char*)malloc(sizeof(char));
    strcat(sendQuestion,question_string);
    strcat(sendQuestion,sendBuf);
    // printf("%s\n",sendQuestion);
	send(get_pair_fd(i),sendQuestion,strlen(sendQuestion),0);
    // free(sendQuestion);
    fflush(stdout);
}
void duo_ans(int i, fd_set *master,int sockfd,int fdmax) {
	int nbytes_recvd,j;
	char recv_buf[MAXLINE],flag[MAXLINE],send_buf[MAXLINE];
	char question[MAXLINE],ansA[MAXLINE],ansB[MAXLINE],ansC[MAXLINE],ansD[MAXLINE];
    // If the other player has answered
    if(duo_fds[i] == 1)
    {
        char code[MAXLINE] = "CODE:";
        strcat(code,"305");
        send(i, code, strlen(code), 0);
    } else{
        strtok(NULL,":");
        strcpy(recv_buf,strtok(NULL,":"));
        duo_fds[i] = 1;
        if(strcmp(recv_buf,duo_answer) == 0){
            // puts("CORRECT!");
            char code[MAXLINE] = "CODE:";
            strcat(code,"300");
            send(i, code, strlen(code), 0);
        } else {
            // puts("FAIL!");
            char code[MAXLINE] = "CODE:";
            strcat(code,"301");
            send(i, code, strlen(code), 0);
        }
        send_to_all(get_pair_fd(i),i,sockfd,strlen(flag),flag,master);
    }
}
void add_question(int i, fd_set *master,int sockfd,int fdmax,QuestionDB quest)
{
    char code[MAXLINE] = "CODE:";
    addQuestionToDatabase("question.txt",quest);
    rootQuestion = insertAtBeginning(rootQuestion,quest);
    //send code
    strcat(code,"400");
    send(i,code,strlen(code),0);
}
void score_board(int i, fd_set *master,int sockfd,int fdmax){
    char *send_buf = (char*)malloc(sizeof(char));
    strtok(getResult(),":");
    strcpy(send_buf,strtok(NULL,":"));
    send(i,send_buf,strlen(send_buf),0);
    free(send_buf);
}
void send_recv(int i, fd_set *master,int sockfd,int fdmax)
{
	int nbytes_recvd,j;
	char recv_buf[MAXLINE],buf[MAXLINE];
    char type[MAXLINE],mode[MAXLINE];
    login:
	if((nbytes_recvd = recv(i,recv_buf,MAXLINE,0)) <= 0)
	{
		if(nbytes_recvd == 0)
		{
			printf("socket %d hung up\n",i);
		}
		else
		{
			perror("recv");
		}
		close(i);
		FD_CLR(i,master);
	} else
	{
		recv_buf[nbytes_recvd]='\0';
		printf("%s\n",recv_buf);
        strcpy(mode, strtok(recv_buf , ":"));
        strcpy(type, strtok(NULL, ":"));
        if(strcmp(type,"Login") == 0) {
            char username[MAXLINE];
            strtok(NULL,":");
            strcpy(username,strtok(NULL, ":"));
            log_in(i,username);
            if(strcmp(flag,"101") == 0)
                goto login;
        } else if(strcmp(type,"SinglePlay\n") == 0) {
			single_play(i,master,sockfd,fdmax,rootQuestion,user);
		} else if(strcmp(type,"DuoPlay\n") == 0) {
            if(get_pair_fd(i) == -1){
                char code[MAXLINE] = "CODE:";
                strcat(code,"303");
                send(i,code,strlen(code),0);
            } else{
                char code[MAXLINE] = "CODE:";
                strcat(code,"304");
                send(i,code,strlen(code),0);
			    duo_play(i,master,sockfd,fdmax);
            }
		} else if(strcmp(type,"DuoAnswer") == 0) {
			duo_ans(i,master,sockfd,fdmax);
		} else if(strcmp(type,"AddQuestion") == 0) {
            QuestionDB quest = (QuestionDB)malloc(sizeof(struct Question));
            strtok(NULL,":");
            strcpy(quest->question,strtok(NULL,":"));
            strcpy(quest->answerA,strtok(NULL,":"));
            strcpy(quest->answerB,strtok(NULL,":"));
            strcpy(quest->answerC,strtok(NULL,":"));
            strcpy(quest->answerD,strtok(NULL,":"));
            strcpy(quest->rightAns,strtok(NULL,":"));
            strcpy(quest->rightAns,strtok(quest->rightAns,"\n"));
            add_question(i,master,sockfd,fdmax,quest);
        } else if(strcmp(type,"ScoreBoard\n") == 0){ 
            score_board(i,master,sockfd,fdmax);
        }
		fflush(stdout);
  }
}
	
		  
		
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd;

	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1)
	{
		perror("accept");
		exit(1);
	} else
	  {
        // printf("fdmax:%d newsockfd:%d",*fdmax,newsockfd);
        for(int i=0;i<LISTENQ;i++){
            if(fds[i] == 0){
                fds[i] = newsockfd;
                break;
            }
        }
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax)
		{
			*fdmax = newsockfd;
            // printf("fdmax:%d newsockfd:%d",*fdmax,newsockfd);
		}
		
	printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));

    }
}


void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
	if((*sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(3000);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	bind(*sockfd, (struct sockaddr *)my_addr,sizeof(struct sockaddr));
	listen(*sockfd, LISTENQ);

	printf("%s\n", "Server running...waiting for connections.");
	fflush(stdout);
}

int main()
{
	rootQuestion = readQuestion("question.txt");
    rootPlayer = readPlayer("player.txt");
	fd_set master;
	fd_set read_fds;
    struct timeval tv;
	int fdmax,i,rv;
	int sockfd=0;
	struct sockaddr_in my_addr,client_addr;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&sockfd,&my_addr);
	FD_SET(sockfd,&master);
    tv.tv_sec = 5;
    tv.tv_usec = 0;

	fdmax = sockfd;
    for(int i=0;i<LISTENQ;i++){
        fds[i] = 0;
    }
    for(int i=0;i<LISTENQ;i++){
        duo_fds[i] = 0;
    }
	while(1)
	{
		read_fds = master;
        rv = select(fdmax+1,&read_fds,NULL,NULL,NULL);
		if(rv == -1)
		{
			perror("select");
			exit(4);
		}
        
		else if(rv){
            for(i=0;i<= fdmax;i++)
            {
                if(FD_ISSET(i,&read_fds)) 
                {
                    if(i == sockfd)
                        connection_accept(&master,&fdmax,sockfd,&client_addr);
                    else
                        send_recv(i,&master,sockfd,fdmax);
                }
            }
        }
        // else{
        //     printf("There is no data in 5 seconds!");
        // }
	}
return 0;
}