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

int checkAnswer(QuestionDB cur, char *ans);
void printQuestion(QuestionDB db);
void addQuestionToDatabase(char *filename,QuestionDB ques);
void printOneQuestion(QuestionDB db);
QuestionDB appendQuestion(QuestionDB db, char* question, char* answerA, char* answerB, char* answerC, char* answerD, char* rightAns);
QuestionDB insertAtBeginning(QuestionDB db,QuestionDB quest);



#endif