#include <stdio.h>

//using for debug
#define debug(ss, args...) fprintf(stderr, ss, ##args)


// struct type 1
typedef struct typePerson{
  int index; //person index
  char name[11]; //person name
 
  int manageTeam; //which team did he/she manage 
  int asMember;   //how many team did he paticipate
}person;

// struct 2
typedef struct typeTeam{
  int index;
  char name[11];
  char project[10];

  int manager; 	// manager id
  int memberCount; // how many member
  int member[4];

}team;

// struct 3
typedef struct typeEvent{
  int index;

  int teamID;
  int holdDay; 		// 0-17
  int startTime;  // 9-17
  int endTime;		// 10-18

  char name[11]; // project name
  char project[10];

}event;

extern event eventArr[200];
extern person personArr[9];
extern team teamArr[6];
extern char toDate[18][11];
extern char toTime[10][6];
extern int myCalendar[20][10];

enum {BUF = 255};

extern int trySchedule(event *e, int wfd[8][2], int rfd[8][2]);
extern int reschedule(event* e, int wfd[8][2], int rfd[8][2]);
extern char* event2str(char sig,event *e,char str[]);
extern char* team2str(char sig,team *t,char str[]);
extern int gets_s(char* str, FILE* f);
