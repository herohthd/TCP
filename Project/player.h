#ifndef _PLAYER_H_
#define _PLAYER_H_

struct Player{
    char name[30];
    int numOfRightAns;
    int score;
    struct Player *next;
};
typedef struct Player* PlayerDB;

PlayerDB readPlayer(char *filename);
PlayerDB appendPlayer(PlayerDB db, char *name);
PlayerDB registered(PlayerDB db,char *name);
void printPlayer(PlayerDB db);


#endif