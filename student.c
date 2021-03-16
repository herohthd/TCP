#include <string.h>
#include "course.h"
#include "student.h"
#include <stdio.h>
#include <stdlib.h>

StudentDB append_student(StudentDB db, StudentDB source)
{
    StudentDB n = (StudentDB)malloc(sizeof(struct Student));
    strcpy(n->name, source->name);
    strcpy(n->password, source->password);
    n->numOfCourses = 0;
    n->next = NULL;
    if (db == NULL)
    {
        return n;
    }
    StudentDB cur = db;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    n->next = cur->next;
    cur->next = n;
    return db;
}
CourseDB find_course(CourseDB db, StudentDB source)
{
    for (CourseDB cur = db; cur != NULL; cur = cur->next)
    {
        if (strcmp(cur->idNum, source->listCourse[0].idNum) == 0)
        {
            return cur;
        }
    }
    return NULL;
}
int registered(StudentDB db, CourseDB course)
{
    if (db->numOfCourses == 0)
        return 0;
    for (int i = 0; i < db->numOfCourses; i++)
    {
        if (strcmp(db->listCourse[i].idNum, course[0].idNum) == 0)
        {
            printf("%s already register the course %s\n", db->name, course->idNum);
            return 1;
        }
    }
    return 0;
}
int conflict(StudentDB db, CourseDB course)
{
    for(int i=0;i<db->numOfCourses;i++){
        if(db->listCourse[i].timeStart[0]==course->timeStart[0]){
            int timeStart=db->listCourse[i].timeStart[2]-'0';
            int timeEnd=db->listCourse[i].timeEnd[2]-'0';
            int courseStart=course->timeStart[2]-'0';
            int courseEnd=course->timeEnd[2]-'0';
            if( (timeStart<=courseStart && courseStart<=timeEnd) ||
                (timeStart<=courseEnd && courseEnd<=timeEnd) ){
                    printf("Student %s has course %s conflicts time with course %s\n",db->name,db->listCourse[i].name,course->name);
                    return 1;
            }
        }
    }
    return 0;
}
void print_day(StudentDB acc, char day)
{
    char time[20];
    char whichDay[10];
    int flag = 0;
    printf("\n%-10s %-20s %-10s %-10s %-10s %-18s %s\n",
           "Code", "Course", "Weekday", "AM-PM", "Period", "Week", "Room");
    for (int i = 0; i < acc->numOfCourses; i++)
    {
        if (acc->listCourse[i].timeStart[0] == day)
        {
            flag = 1;
            switch (acc->listCourse[i].timeStart[0])
            {
            case '2':
                strcpy(whichDay, "Monday");
                break;
            case '3':
                strcpy(whichDay, "Tuesday");
                break;
            case '4':
                strcpy(whichDay, "Wednesday");
                break;
            case '5':
                strcpy(whichDay, "Thursday");
                break;
            case '6':
                strcpy(whichDay, "Friday");
                break;
            case '7':
                strcpy(whichDay, "Saturday");
                break;
            case '8':
                strcpy(whichDay, "Sunday");
                break;
            }
            if (acc->listCourse[i].timeStart[1] == '1')
            {
                strcpy(time, "Morning");
            }
            else
                strcpy(time, "Afternoon");
            printf("%-10s %-20s %-10s %-10s %c,%-5c %-20s %s\n",
                   acc->listCourse[i].idName, acc->listCourse[i].name,
                   whichDay,
                   time,
                   acc->listCourse[i].timeStart[2],
                   acc->listCourse[i].timeEnd[2],
                   acc->listCourse[i].week,
                   acc->listCourse[i].room);
        }
    }
    if (flag == 0)
        printf("You don't have any course on this day.\n");
}
void print_schedule(StudentDB db)
{
    printf("  |%-10s|%-10s|%-10s|%-10s|%-10s\n", "Monday", "Tuesday", "Wednesday", "Thurday", "Friday");
    printf("------------------------------------------------------\n");
    for (int i = 1; i <= 12; i++)
    {
        CourseDB n = (CourseDB)malloc(sizeof(struct Course));
        int count = 0;
        if(i>=10) printf("%d|",i); 
        else printf("%d |",i);
        // Get the day and the room that has the period i
        for (int j = 0; j < db->numOfCourses; j++)
        {
            int timeStart,timeEnd;
            if(db->listCourse[j].timeStart[1]=='1'){
                timeStart = db->listCourse[j].timeStart[2]-'0';
                timeEnd = db->listCourse[j].timeEnd[2]-'0';
            }
            else{
                timeStart = db->listCourse[j].timeStart[2]-'0';
                timeEnd = db->listCourse[j].timeEnd[2]-'0';
                timeStart+=6;
                timeEnd+=6;
            }
            if (timeStart <= i && i <= timeEnd)
            {
                n[count].timeStart[0]=db->listCourse[j].timeStart[0];
                strcpy(n[count].room,db->listCourse[j].room);
                // printf("%c %s\n",n[count].timeStart[0],n[count].room);
                count++;
            }
        }
        for(int a=2;a<=6;a++){
            int flag=0;
            for(int b=0;b<count;b++){
                int day=n[b].timeStart[0]-'0';
                if(a==day){
                    printf("%-10s|",n[b].room);
                    flag=1;
                }
            }
            if(flag==0){
                printf("%-10s|","");
            }
        }
        printf("\n");
        free(n);
    }
}