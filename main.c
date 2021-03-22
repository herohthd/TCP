#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "student.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

CourseDB root = NULL;
StudentDB root_student = NULL;
int listenfd, connfd, n;
pid_t childpid;
socklen_t clilen;
char buf[MAXLINE];
struct sockaddr_in cliaddr, servaddr;

// Read course
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
CourseDB readCourse(char *filename)
{
    FILE *fin = fopen(filename, "r");
    if (fin == NULL)
    {
        printf("Khong ton tai tep %s.\n", filename);
        return 0;
    }
    while (!feof(fin))
    {
        CourseDB n = (CourseDB)malloc(sizeof(struct Course));
        // READ SPECIAL INPUT WITH COMMA AND SPACE
        fscanf(fin, "%s %s %19[^\t]\t%4[^,],%4[^,],%14s,%s\n",
               n->idNum, n->idName,
               n->name, n->timeStart,
               n->timeEnd, n->week,
               n->room);
        root = append_course(root, n->idNum, n->idName, n->name, n->timeStart, n->timeEnd, n->week, n->room);
        free(n);
    }
    fclose(fin);
    return root;
}

// READ STUDENT
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
            if(db->listCourse[i].timeStart[1]==course->timeStart[1]){
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
StudentDB append_student_course(StudentDB db, StudentDB source)
{
    int flag=0;
    StudentDB n = (StudentDB)malloc(sizeof(struct Student));
    for (n = db; n != NULL; n = n->next)
    {
        if (strcmp(n->name, source->name) == 0)
        {
            flag=1;
            CourseDB subToAdd = find_course(root, source);
            if (subToAdd)
            {
                if (!registered(n, subToAdd)&&!conflict(n,subToAdd))
                {
                    n->listCourse[n->numOfCourses] = subToAdd[0];
                    n->numOfCourses++;
                    return db;
                }
            }
        }
    }
    if(flag==0) printf("Student %s doesn't exist\n",source->name);
    return db;
}
StudentDB login(StudentDB db, char *name, char *password)
{
    for (StudentDB cur = db; cur != NULL; cur = cur->next)
    {
        if (strcmp(cur->name, name) == 0)
        {
            if (strcmp(cur->password, password) == 0)
            {
                return cur;
            }
        }
    }
    return NULL;
}
StudentDB readStudent(char *filename1, char *filename2)
{
    FILE *fin1 = fopen(filename1, "r");
    FILE *fin2 = fopen(filename2, "r");
    if (fin1 == NULL)
    {
        printf("Don't exist %s.\n", filename1);
        return 0;
    }
    if (fin2 == NULL)
    {
        printf("Don't exist %s.\n", filename2);
        return 0;
    }
    while (!feof(fin1))
    {
        StudentDB source = (StudentDB)malloc(sizeof(struct Student));
        fscanf(fin1, "%s %s\n", source->name, source->password);
        root_student = append_student(root_student, source);
        free(source);
    }
    while (!feof(fin2))
    {
        StudentDB source = (StudentDB)malloc(sizeof(struct Student));
        fscanf(fin2, "%s %s\n", source->name, source->listCourse[0].idNum);
        // printf("%s %s\n",source->name, source->listCourse[0].idNum);
        root_student = append_student_course(root_student, source);
        free(source);
    }
    fclose(fin1);
    fclose(fin2);
    return root_student;
}

void send_day(StudentDB acc,char *day){
    int flag = 0;
    char time[20];
    char whichDay[20];
    int size = 0;
    char mes[MAXLINE]="You don't have course on this day";
    for (int i = 0; i < acc->numOfCourses; i++)
    {
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
        }
        if (strcmp(whichDay,day)==0)
        {
            flag = 1;
            if (acc->listCourse[i].timeStart[1] == '1')
                strcpy(time, "Morning");
            else
                strcpy(time, "Afternoon");
            char *temp1 = strdup(acc->listCourse[i].idName);
            char *temp2 = strdup(acc->listCourse[i].name);
            char *temp3 = strdup(whichDay);
            char *temp4 = strdup(time);

            char *temp5 = strdup(acc->listCourse[i].week);
            char *temp6 = strdup(acc->listCourse[i].room);
            char *cat1 = strcat(temp1, temp2);
            char *cat2 = strcat(cat1, temp3);
            char *cat3 = strcat(cat2, temp4);
            char *cat4 = strcat(cat3, temp5);
            char *cat5 = strcat(cat4, temp6);
            printf("Schedule:%s\n",cat5);
            send(connfd,cat5,strlen(cat5),0);
        }
    }
    if(flag == 0) send(connfd,mes,strlen(mes),0);
}
int main()
{
/*
    int running = 1;
    int k;
    char day;
    int success = 1;
    char name[20], password[20];
    root = readCourse("course.txt");
    root_student = readStudent("student_account.txt", "student_registration.txt");
    StudentDB account = (StudentDB)malloc(sizeof(struct Student));
    comeback:
    do
    {
        printf("Enter your username:");
        scanf("%s", name);
        printf("Enter your password:");
        scanf("%s", password);
        account = login(root_student, name, password);
        if (!account)
        {
            printf("Your username or password is incorrect. Please try again!\n");
        }
    } while (!account);
    printf("\nLogin successfully\n");
    while (running)
    {
        printf("\n1.Print all the course\n");
        printf("2.Enter day:\n");
        printf("3.Print your time shedule\n");
        printf("4.Log out\n");
        printf("5.Exit");
        do
        {
            printf("\n Choose option:");
            scanf("%d", &k);
        } while (k < 1 || k > 5);
        switch (k)
        {
        case 1:
            print_course(root);
            break;
        case 2:
            printf("Enter the day you want to show:");
            fflush(stdin);
            scanf("%c", &day);
            printf("%s %s", account->name, account->listCourse[1].idName);
            print_day(account, day);
            break;
        case 3:
            print_schedule(account);
            break;
        case 4:
            goto comeback;
            break;
        case 5:
            free(root);
            free(root_student);
            running = 0;
            break;
        default:
            running = 0;
            break;
        }
    }
*/
    //Server side
    char flag[MAXLINE];
    char username[MAXLINE];
    char password[MAXLINE]; 
    root = readCourse("course.txt");
    root_student = readStudent("student_account.txt", "student_registration.txt");
    StudentDB account = (StudentDB)malloc(sizeof(struct Student));
    //creation of the socket
    listenfd = socket (AF_INET, SOCK_STREAM, 0);
    //preparation of the socket address 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen (listenfd, LISTENQ);
    printf("%s\n","Server running...waiting for connections.");

    for ( ; ; ) {     
        clilen = sizeof(cliaddr);
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
        printf("%s\n","Received request...");
        do{       
            recv(connfd, username, MAXLINE,0);
            recv(connfd, password, MAXLINE,0);
            puts(username);
            puts(password);
            account = login(root_student, username, password);
            if(account){
                strcpy(flag,"success");
                puts("SUCCESS");
                send(connfd, flag, strlen(flag), 0);
            }
            else{
                puts("FAIL");
                strcpy(flag,"fail");
                send(connfd, flag, strlen(flag), 0);
            }    
        }while(strcmp(flag,"fail")==0);
        while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
            // printf("%d\n",n);
            char *buffer = (char*)malloc(n);
            strncpy(buffer,buf,n);
            buffer[n]=0;
            // printf("%s","String received from and resent to the client:");
            puts(buffer);
            // if(strcmp(buffer,"All") == 0) {
            //     send_schedule(account,buffer);
            // }
            send_day(account,buffer);
            free(buffer);
        }
        if (n < 0) {
        perror("Read error"); 
        exit(1);
        }
        close(connfd);
    }
    //close listening socket
    close (listenfd); 
}