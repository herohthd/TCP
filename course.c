#include <string.h>
#include "course.h"
#include <stdio.h>
#include <stdlib.h>

CourseDB append_course(CourseDB db, char *idNum, char *idName, char *name, char *timeStart, char *timeEnd, char *week, char *room)
{

    CourseDB n = (CourseDB)malloc(sizeof(struct Course));
    strcpy(n->idNum, idNum);
    strcpy(n->idName, idName);
    strcpy(n->name, name);
    strcpy(n->timeStart, timeStart);
    strcpy(n->timeEnd, timeEnd);
    strcpy(n->week, week);
    strcpy(n->room, room);
    n->next = NULL;
    if (db == NULL)
    {
        return n;
    }
    CourseDB cur = db;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    n->next = cur->next;
    cur->next = n;
    return db;
}
void print_course(CourseDB db)
{
    char time[20];
    char day[10];
    printf("\n%-10s %-20s %-10s %-10s %-10s %-18s %s\n",
           "Code", "Course", "Weekday", "AM-PM", "Period", "Week", "Room");
    CourseDB cur = db;
    for (CourseDB cur = db; cur != NULL; cur = cur->next)
    {
        switch (cur->timeStart[0])
        {
        case '2':
            strcpy(day, "Monday");
            break;
        case '3':
            strcpy(day, "Tuesday");
            break;
        case '4':
            strcpy(day, "Wednesday");
            break;
        case '5':
            strcpy(day, "Thursday");
            break;
        case '6':
            strcpy(day, "Friday");
            break;
        case '7':
            strcpy(day, "Saturday");
            break;
        case '8':
            strcpy(day, "Sunday");
            break;
        }
        if (cur->timeStart[1] == '1')
        {
            strcpy(time, "Morning");
        }
        else
            strcpy(time, "Afternoon");
        printf("%-10s %-20s %-10s %-10s %c,%-5c %-20s %s\n",
               cur->idName,
               cur->name,
               day,
               time,
               cur->timeStart[2], cur->timeEnd[2],
               cur->week, cur->room);
    }
}
