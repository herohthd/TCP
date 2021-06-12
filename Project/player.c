#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
void addPlayerResult(char *filename, PlayerDB uCur)
{
    FILE *fout = fopen(filename, "a");
    if (fout == NULL)
    {
        printf("Not exist %s.\n", filename);
    }
    if (uCur)
    {
        fprintf(fout, "%s\t", uCur->name);
        fprintf(fout, "%d\t", uCur->numOfRightAns);
        fprintf(fout, "%d\n", uCur->score);
    }
    fclose(fout);
}
PlayerDB replace(PlayerDB result, PlayerDB n)
{
    PlayerDB cur = result;
    while (cur != NULL)
    {
        if (strcmp(cur->name, n->name) == 0)
        {
            if (cur->score < n->score)
            {
                swap(cur, n);
            }
        }
        cur = cur->next;
    }
    return result;
}
PlayerDB getPlayerResult(char *filename)
{
    PlayerDB result = (PlayerDB)malloc(sizeof(struct Player));
    result = NULL;
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
        fscanf(fin, "%s\t", n->name);
        fscanf(fin, "%d\t", &n->numOfRightAns);
        fscanf(fin, "%d\n", &n->score);
        // printf("%s %d %d\n",n->name,n->numOfRightAns,n->score);
        if (registered(result, n->name))
        {
            result = replace(result, n);
        }
        else
        {
            result = appendPlayer(result, n);
        }
        // free(n);
    }
    fclose(fin);
    return result;
}
PlayerDB bubbleSort(PlayerDB result)
{
    int swapped, i;
    PlayerDB ptr1;
    PlayerDB lptr = NULL;

    /* Checking for empty list */
    if (result == NULL)
        return NULL;

    do
    {
        swapped = 0;
        ptr1 = result;

        while (ptr1->next != lptr)
        {
            if (ptr1->score < ptr1->next->score)
            {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    return result;
}
void swap(PlayerDB a, PlayerDB b)
{
    int temp = a->score;
    int temp1 = a->numOfRightAns;
    char temp2[20];
    strcpy(temp2, a->name);

    strcpy(a->name, b->name);
    a->score = b->score;
    a->numOfRightAns = b->numOfRightAns;

    strcpy(b->name, temp2);
    b->score = temp;
    b->numOfRightAns = temp1;
}
char *getResult()
{
    char *scoreBoard = (char *)malloc(sizeof(char));
    strcat(scoreBoard, "Result:");
    PlayerDB result = getPlayerResult("result.txt");
    // printf("%s",result->name);
    result = bubbleSort(result);
    while (result != NULL)
    {
        char numOfRightAns[20];
        char score[20];
        strcat(scoreBoard, result->name);
        strcat(scoreBoard, "\t");

        sprintf(numOfRightAns, "%d", result->numOfRightAns);
        strcat(scoreBoard, numOfRightAns);
        strcat(scoreBoard, "\t");

        sprintf(score, "%d", result->score);
        strcat(scoreBoard, score);
        strcat(scoreBoard, "\n");

        result = result->next;
    }
    free(result);
    return scoreBoard;
}
PlayerDB registered(PlayerDB db, char *name)
{
    for (PlayerDB cur = db; cur != NULL; cur = cur->next)
    {
        if (strcmp(cur->name, name) == 0)
        {
            return cur;
        }
    }
    return NULL;
}
void printPlayer(PlayerDB db)
{
    for (PlayerDB cur = db; cur != NULL; cur = cur->next)
    {
        printf("%s\n", cur->name);
    }
}
void addPlayerToDatabase(char *filename, PlayerDB player)
{
    FILE *fout = fopen(filename, "a");
    if (fout == NULL)
    {
        printf("Not exist %s.\n", filename);
    }
    fprintf(fout, "%s\n", player->name);
    fclose(fout);
}
PlayerDB appendPlayer(PlayerDB db, PlayerDB player)
{
    PlayerDB n = (PlayerDB)malloc(sizeof(struct Player));
    strcpy(n->name, player->name);
    if (player->numOfRightAns != 0)
    {
        n->numOfRightAns = player->numOfRightAns;
    }
    else
    {
        n->numOfRightAns = 0;
    }
    if (player->score != 0)
    {
        n->score = player->score;
    }
    else
    {
        n->score = 0;
    }
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
// int main(){
//     char result[2048];
//     PlayerDB rootResult = getPlayerResult("result.txt");
//     // while(rootResult != NULL) {
//     //     printf("%s %d %d",rootResult->name,rootResult->numOfRightAns,rootResult->score);
//     //     rootResult = rootResult->next;
//     // }
//     puts(getResult());
//     return 0;
// }