#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "question.h"
#include "player.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#define MAXCHAR 100
#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8      /*maximum number of client connections */
#define SCORE 10
#define MAXQUESTION 5

PlayerDB rootPlayer = NULL;
QuestionDB rootQuestion = NULL;
int listenfd, connfd, n;
pid_t childpid;
socklen_t clilen;
char buf[MAXLINE];
char flag[MAXCHAR];
char mark[MAXCHAR];
int countQuestion = 0;
struct sockaddr_in cliaddr, servaddr;

// Read questions
int checkAnswer(QuestionDB cur, char *ans){
    if(strcmp(cur->rightAns,ans)==0){
        return 1;
    }
    return 0;
}
void printQuestion(QuestionDB db){
    for (QuestionDB cur = db; cur != NULL; cur = cur->next)
    {
        printf("%s%s%s%s%sRightAnswer: %s\n",cur->question,
                                        cur->answerA,cur->answerB,
                                        cur->answerC,cur->answerD,
                                        cur->rightAns);
    }
}
void addQuestionToDatabase(char *filename,QuestionDB ques){
    FILE *fout = fopen(filename, "a");
    if (fout == NULL)
    {
        printf("Not exist %s.\n", filename);
    }
    if(ques){
        fprintf(fout,"%s",ques->question);
        fprintf(fout,"%s",ques->answerA);
        fprintf(fout,"%s",ques->answerB);
        fprintf(fout,"%s",ques->answerC);
        fprintf(fout,"%s",ques->answerD);
        fprintf(fout,"%s",ques->rightAns);
    }
    fclose(fout);
}
void printOneQuestion(QuestionDB db){
    QuestionDB cur=db;
    if(cur!=NULL){
        printf("%s%s%s%s%s\n",cur->question,
                                        cur->answerA,cur->answerB,
                                        cur->answerC,cur->answerD);
    }
    else printf("There is no more question\n");
}
QuestionDB appendQuestion(QuestionDB db, char* question, char* answerA, char* answerB, char* answerC, char* answerD, char* rightAns)
{

    QuestionDB n = (QuestionDB)malloc(sizeof(struct Question));
    strcpy(n->question, question);
    strcpy(n->answerA, answerA);
    strcpy(n->answerB, answerB);
    strcpy(n->answerC, answerC);
    strcpy(n->answerD, answerD);
    strcpy(n->rightAns, rightAns);
    n->next = NULL;
    if (db == NULL)
    {
        return n;
    }
    QuestionDB cur = db;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    n->next = cur->next;
    cur->next = n;
    return db;
}
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

//Read Player
PlayerDB registered(PlayerDB db,char *name)
{
    for (PlayerDB cur = db; cur != NULL; cur = cur->next)
    {
        if(strcmp(cur->name,name)==0){
            return cur;
        }
    }
    return NULL;
}
void printPlayer(PlayerDB db){
    for (PlayerDB cur = db; cur != NULL; cur = cur->next)
    {
        printf("%s\n",cur->name);
    }
}
void addPlayerToDatabase(char *filename,PlayerDB player){
    FILE *fout = fopen(filename, "a");
    if (fout == NULL)
    {
        printf("Not exist %s.\n", filename);
    }
    fprintf(fout,"%s\n",player->name);
    fclose(fout);
}
PlayerDB appendPlayer(PlayerDB db, PlayerDB player)
{
    PlayerDB n = (PlayerDB)malloc(sizeof(struct Player));
    strcpy(n->name, player->name);
    n->numOfRightAns = 0;
    n->score = 0;
    n->next = NULL;
    if (db == NULL)
    {
        return n;
    }
    PlayerDB cur = db;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    n->next = cur->next;
    cur->next = n;
    return db;
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
void sendAQuestion(QuestionDB qRoot, PlayerDB uCur, int num) {
    char correct[MAXCHAR] = "200";
    char wrong[MAXCHAR] = "201";
    char win[MAXCHAR] = "202"; 
    int index = 0;
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
    send(connfd,cat4,strlen(cat4),0);
    n = recv(connfd,buf,MAXLINE,0);
    char *ans = (char *)malloc(n);
    strncpy(ans,buf,n);
    ans[n]=0;
    puts(ans);
    if(checkAnswer(qCur,ans)){
        if( countQuestion == MAXQUESTION ) {
            uCur->numOfRightAns++;
            // Win the game
            send(connfd,win,strlen(win),0);
        }
        else{
            uCur->numOfRightAns++;
            qCur = qCur->next;
            send(connfd,correct,strlen(correct),0);
        }
    }
    else{
        send(connfd,wrong,strlen(wrong),0);
    }
}
void sendQuestions(QuestionDB qRoot, PlayerDB uCur) {
    bool arr[21] = {0};
    int r;
    countQuestion = 0;
    while(countQuestion < MAXQUESTION) {
        printf("Number of question until now:%d\n",countQuestion);
        countQuestion++;
        do{
            r = rand() % MAXQUESTION;
        } while(arr[r]);
        arr[r] = 1;
        sendAQuestion(qRoot,uCur,r);
    }
    
}
void addQuestion(QuestionDB qRoot) {
    QuestionDB quest = (QuestionDB)malloc(sizeof(struct Question));
    n = recv(connfd,buf,MAXLINE,0);
    // printf("%d\n",n);
    char *str = (char *)malloc(n);
    strncpy(str,buf,n);
    str[n]=0;
    puts(str);
    strcpy(quest->question,str);
    free(str);
    
    n = recv(connfd,buf,MAXLINE,0);
    // printf("%d\n",n);
    char *str1 = (char *)malloc(n);
    strncpy(str1,buf,n);
    str1[n]=0;
    puts(str1);
    strcpy(quest->answerA,str1);
    free(str1);

    n = recv(connfd,buf,MAXLINE,0);
    // printf("%d\n",n);
    char *str2 = (char *)malloc(n);
    strncpy(str2,buf,n);
    str2[n]=0;
    puts(str2);
    strcpy(quest->answerB,str2);
    free(str2);

    n = recv(connfd,buf,MAXLINE,0);
    // printf("%d\n",n);
    char *str3 = (char *)malloc(n);
    strncpy(str3,buf,n);
    str3[n]=0;
    puts(str3);
    strcpy(quest->answerC,str3);
    free(str3);

    n = recv(connfd,buf,MAXLINE,0);
    // printf("%d\n",n);
    char *str4 = (char *)malloc(n);
    strncpy(str4,buf,n);
    str4[n]=0;
    puts(str4);
    strcpy(quest->answerD,str4);
    free(str4);

    n = recv(connfd,buf,MAXLINE,0);
    // printf("%d\n",n);
    char *str5 = (char *)malloc(n);
    strncpy(str5,buf,n);
    str5[n]=0;
    puts(str5);
    strcpy(quest->rightAns,str5);
    free(str5);

    addQuestionToDatabase("question.txt",quest);
}
int main(){
    PlayerDB user = (PlayerDB)malloc(sizeof(struct Player));
    rootQuestion = readQuestion("question.txt");
    rootPlayer = readPlayer("player.txt");

    //creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);
    printf("%s\n", "Server running...waiting for connections.");

    for (;;)
    {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0)
            exit(0);
        printf("%s\n", "Received request...");

        if ((childpid = fork()) == 0)
        { //if it’s 0, it’s child process

            printf("%s\n", "Child created for dealing with client requests");

            //close listening socket
            close(listenfd);
            do
            {
                // read username
                char *buf = (char *)malloc(MAXLINE);
                int n = recv(connfd, buf, MAXLINE, 0);
                char *username = (char *)malloc(n);
                strncpy(username, buf, n);
                username[n] = 0;
                fflush(stdout);
                puts(username);
                user = registered(rootPlayer, username);
                free(buf);
                if (!user) {
                    strcpy(flag, "100");
                    PlayerDB cur = (PlayerDB)malloc(sizeof(struct Player));
                    strcpy(cur->name,username);
                    cur->numOfRightAns = 0;
                    cur->score = 0;
                    appendPlayer(rootPlayer,cur);
                    addPlayerToDatabase("player.txt",cur);
                    user = registered(rootPlayer,username);
                    // puts("SUCCESS");
                    send(connfd, flag, strlen(flag), 0);
                }else {
                    free(username);
                    // puts("FAIL");
                    strcpy(flag, "101");
                    send(connfd, flag, strlen(flag), 0);
                }
            }while(strcmp(flag,"101")==0);

            // receive menu choice: single play, multi play,
            //  score board or add question to database
            while( (n = recv(connfd,buf,MAXLINE,0)) > 0){
                // printf("%d\n",n);
                char *buffer = (char *)malloc(n);
                strncpy(buffer,buf,n);
                buffer[n]=0;
                puts(buffer);
                // puts(buf);
                if(strcmp(buffer,"1") == 0) {
                    sendQuestions(rootQuestion,user);
                } else if(strcmp(buffer,"2") == 0) {
                    addQuestion(rootQuestion);
                } else if(strcmp(buffer,"3") == 0) {

                } else {

                }
                free(buffer);
            }
            // free(buffer);
        }
    }
    free(rootPlayer);
    free(rootQuestion);
}