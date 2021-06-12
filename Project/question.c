#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "question.h"

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
        fprintf(fout,"%s\n",ques->rightAns);
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
QuestionDB insertAtBeginning(QuestionDB db,QuestionDB quest)
{
    if(quest == NULL) {
        return db;
    }
    QuestionDB cur = db;
    quest->next = cur;
    return quest;
}
