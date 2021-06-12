#ifndef _PLAYER_H_
#define _PLAYER_H_

struct Player{
    char name[30];
    int numOfRightAns;
    int score;
    struct Player *next;
};
typedef struct Player* PlayerDB;

void addPlayerToDatabase(char *filename,PlayerDB player);
PlayerDB appendPlayer(PlayerDB db, PlayerDB player);
PlayerDB registered(PlayerDB db,char *name);
void printPlayer(PlayerDB db);
void addPlayerResult(char *filename,PlayerDB uCur);
PlayerDB getPlayerResult(char *filename);
PlayerDB bubbleSort(PlayerDB result);
void swap(PlayerDB a, PlayerDB b);
char *getResult();
PlayerDB replace(PlayerDB result,PlayerDB n);
#endif