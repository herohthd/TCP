#ifndef _COURSE_H_
#define _COURSE_H_

struct Course{
    char idNum[20];
    char idName[20];
    char name[20];
    char timeStart[10];
    char timeEnd[10];
    char week[20];
    char room[20];
    struct Course *next;
};
typedef struct Course *CourseDB;

CourseDB append_course(CourseDB db, char *idNum, char *idName, char *name, char *timeStart, char *timeEnd, char *week, char *room);
void print_course(CourseDB db);
CourseDB readCourse(char *filename);
CourseDB find_course_by_day(CourseDB db,char* day);

#endif