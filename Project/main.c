#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "question.h"
#include "player.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#define MAXCHAR 100
#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8      /*maximum number of client connections */
#define SCORE 10

char str[MAXCHAR+1];
PlayerDB rootPlayer = NULL;
QuestionDB rootQuestion = NULL;
int listenfd, connfd, n;
pid_t childpid;
socklen_t clilen;
char buf[MAXLINE];
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
        fprintf(fout,"\n%s",ques->question);
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
PlayerDB appendPlayer(PlayerDB db, char *name)
{
    PlayerDB n = (PlayerDB)malloc(sizeof(struct Player));
    strcpy(n->name, name);
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
        rootPlayer = appendPlayer(rootPlayer,n->name);
        free(n);
    }
    fclose(fin);
    return rootPlayer;  
}

int main(){
    char str[MAXCHAR];
    rootQuestion = readQuestion("question.txt");
    rootPlayer = readPlayer("player.txt");
    QuestionDB cur=rootQuestion;
    char ans[5];
    char username[20];
    printf("Enter username:");
    scanf("%s",username);
    PlayerDB user = (PlayerDB)malloc(sizeof(struct Player));
    user = registered(rootPlayer,username);
    if(user!=NULL){
        printf("There is already this user\n");
        exit(0);
    }
    else {
        appendPlayer(rootPlayer,username);
        PlayerDB cur = (PlayerDB)malloc(sizeof(struct Player));
        strcpy(cur->name,username);
        cur->numOfRightAns = 0;
        cur->score = 0;
        addPlayerToDatabase("player.txt",cur);
        user = registered(rootPlayer,username);
        printf("Register successfully\n");
    }
    while(cur!=NULL){
        printOneQuestion(cur);
        printf("Enter answer:");
        fflush(stdin);
        scanf("%s",ans);
        getchar();
        if(checkAnswer(cur,ans)){
            user->numOfRightAns++;
            printf("You right!\n");
            cur=cur->next;
        }
        else{
            printf("You wrong!\n");
            break;
        }
    }
    user->score = user->numOfRightAns*SCORE;
    printf("your score is %d\n",user->score);
    QuestionDB n = (QuestionDB)malloc(sizeof(struct Question));
    printf("Enter question u want to add:");
    fflush(stdin);
    fgets(str,sizeof(str),stdin);
    sscanf(str,"%[^\n]s",n->question);
    n->question[strlen(n->question)]='\n';
    printf("%s",n->question);
    printf("Enter answer A:");
    fgets(n->answerA,sizeof(n->answerA),stdin);
    printf("Enter answer B:");
    fgets(n->answerB,sizeof(n->answerB),stdin);
    printf("Enter answer C:");
    fgets(n->answerC,sizeof(n->answerC),stdin);
    printf("Enter answer D:");
    fgets(n->answerD,sizeof(n->answerD),stdin);
    printf("Enter right answer:");
    scanf("%s",n->rightAns);
    addQuestionToDatabase("question.txt",n);
    free(n);
    free(rootPlayer);
    free(rootQuestion);
}