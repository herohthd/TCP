#ifndef _STUDENT_H_
#define _STUDENT_H_
#include "course.h"

struct Student{
    char name[20];
    char password[20];
    int numOfCourses;
    struct Course listCourse[10];
    struct Student *next;
};
typedef struct Student* StudentDB;


void print_day(StudentDB acc, char day);
StudentDB append_student(StudentDB db, StudentDB source);
CourseDB find_course(CourseDB db, StudentDB source);
int registered(StudentDB db, CourseDB course);
int conflict(StudentDB db, CourseDB course);
void print_schedule(StudentDB db);
#endif