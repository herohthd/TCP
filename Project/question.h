#ifndef _QUESTION_H_
#define _QUESTION_H_

struct Question{
    char question[100];
    char answerA[50];
    char answerB[50];
    char answerC[50];
    char answerD[50];
    char rightAns[5];
    struct Question *next;
};
typedef struct Question *QuestionDB;

QuestionDB readQuestion(char *filename);
QuestionDB appendQuestion(QuestionDB db, char* question, char* answerA, char* answerB, char* answerC, char* answerD, char* rightAns);
void printQuestion(QuestionDB db);



#endif