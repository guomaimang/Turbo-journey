#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"


char toDate[18][11] = {
    "2022-04-25",   
    "2022-04-26",   
    "2022-04-27",   
    "2022-04-28",   
    "2022-04-29",   
    "2022-04-30",   
    "2022-05-02",   
    "2022-05-03",   
    "2022-05-04",   
    "2022-05-05",   
    "2022-05-06",   
    "2022-05-07",   
    "2022-05-09",   
    "2022-05-10",   
    "2022-05-11",   
    "2022-05-12",   
    "2022-05-13",   
    "2022-05-14"   
};
char toTime[10][6] = {
    "09:00",
    "10:00",
    "11:00",
    "12:00",
    "13:00",
    "14:00",
    "15:00",
    "16:00",
    "17:00",
    "18:00",
};

event eventArr[200]={
		{0, 0, 1, 9, 10, "0","0"}, // 5
		{1, 0, 5, 12, 13, "0","0"},// 7
		{2, 0, 1, 12, 13, "0","0"},// 6
		{3, 1, 2, 13, 18, "0","0"},// 9
		{4, 1, 2, 10, 12, "0","0"},// 8
		{5, 2, 1, 13, 14, "0","0"}, // 1
		{6, 2, 3, 10, 15,"0","0"}, // 2
		{7, 3, 4, 9, 10, "0","0"},  // 3
		{8, 3, 4, 12, 13, "0","0"}, // 5
		{9, 3, 4, 11, 13, "0","0"}, // 4
		// 5 6 7 9 8 0 2 1 4 3
};
team teamArr[6]={
		{0,"team1","project1", 3, 0, {0,0,0,0}},
		{1,"team2","project2", 4, 0, {0,0,0,0}},
		{2,"team3","project3", 1, 0, {0,0,0,0}},
		{3,"team4","project4", 2, 0, {0,0,0,0}},
		{4,"team5","project5", 8, 0, {0,0,0,0}},
		{5,"team6","project6", 5, 0, {0,0,0,0}},
};
person personArr[9]={
		{0,"Alan",-1,0},
		{1,"Billy",-1,0},
		{2,"Cathy",-1,0},
		{3,"David",-1,0},
		{4,"Eva",-1,0},
		{5,"Fanny",-1,0},
		{6,"Gray",-1,0},
		{7,"Helen",-1,0}
};

// person personArr[9] = 
// {
    // {0, "Alan", -1, 0},
    // {1, "Billy", -1, 0},
    // {2, "Cathy", -1, 0},
    // {3, "David", -1, 0},
    // {4, "Eva", -1, 0},
    // {5, "Fanny", -1, 0},
    // {6, "Gary", -1, 0},
    // {7, "Helen", -1, 0},
// };
// event eventArr[200]={};
// team teamArr[6]={};
int myCalendar[20][10];


int gets_s(char str[], FILE* f){
//    fgets(str, 100, stdin);
	int c,i;
    memset(str, 0, strlen(str));
	for(i=0;(c=getc(f))!='\n'&&c!=EOF;++i){
	    str[i]=c;
    }
	str[i]=0;
    if(c == EOF) return -1;
	return i;
}

char* team2str(char sig,team *t,char str[]){
    sprintf(str,"%c$2$%d$%s$%s$%d$%d$%d$%d$%d$%d",sig,t->index,t->name,t->project,t->manager,t->memberCount,t->member[0],t->member[1],t->member[2], t->member[3]);
    return str;
}

char* event2str(char sig,event *e,char str[]){
    sprintf(str,"%c$3$%d$%d$%d$%d$%d$%s$%s",sig,e->index,e->teamID,e->holdDay,e->startTime,e->endTime,e->name,e->project);
    return str;
}

int reschedule(event* e, int wfd[8][2], int rfd[8][2]){
    int bias;
    int day = e->holdDay;
    int st = e->startTime;
    int et = e->endTime;
    int dur = e->endTime - e->startTime;
    int i;
    
    for(i = st + 1; i < 10-dur; ++i){
        e->startTime = i;
        e->endTime = i + dur;
        if (trySchedule(e, wfd, rfd) == 1) {
            myCalendar[e->holdDay][e->startTime-9] = e->index;
            return 1;
        }
    }
    for(i = st - 1; i>=0; --i){
        e->startTime = i;
        e->endTime = i + dur;
        if (trySchedule(e, wfd, rfd) == 1) {
            myCalendar[e->holdDay][e->startTime-9] = e->index;
            return 1;
        }
    }
    for (bias = 1; bias <= 3; ++bias) {
        e->holdDay = day + bias;
        for(i=0; i<10-dur; ++i){
            e->startTime = i;
            e->endTime = i + dur;
            if (trySchedule(e, wfd, rfd) == 1) {
                myCalendar[e->holdDay][e->startTime-9] = e->index;
                return 1;
            }
        }
        e->holdDay = day - bias;
        for(i = 9-dur; i>=0; --i){
            e->startTime = i;
            e->endTime = i + dur;
            if (trySchedule(e, wfd, rfd) == 1) {
                myCalendar[e->holdDay][e->startTime-9] = e->index;
                return 1;
            }
        }
    }
    return 0;
}

int trySchedule(event *e, int wfd[8][2], int rfd[8][2]){
    char buf[BUF] = "";
    char buf2[BUF] = "";
    sprintf(buf, "T$3$%d$%d$%d$%d$%d$%s$%s", e->index, e->teamID, e->holdDay, e->startTime, e->endTime, e->name, e->project);
    int i;
    int succ = 1;
    debug("trySchedule: buf = %s\n", buf);
    debug("trySchedule: id=%d\n", e->teamID);
    int memberCount = teamArr[e->teamID].memberCount;
    debug("F1: memberCount = %d\n", teamArr[e->teamID].memberCount);
    for(i = 0; i < memberCount; ++i){
        int c = teamArr[e->teamID].member[i];
        debug("c=%d, buf=%s\n", c, buf); 
        write(wfd[c][1], buf, BUF);
        read(rfd[c][0], buf2, BUF);
        debug("%d return: %s\n", i, buf2);
        if(buf2[0] == 'Y') continue;
        succ = 0;
        int j;
        for(j=0; j<=i; ++j){
            int c2 = teamArr[e->teamID].member[j];
            write(wfd[c2][1], "C", 1);
            read(rfd[c2][0], buf2, BUF);
        }
        return 0;
    }
    if(succ){
        myCalendar[e->holdDay][e->startTime-9] = e->index;
        debug("trySchedule: day=%d time=%d index=%d\n", e->holdDay, e->startTime, e->index);
        for(i=0; i<memberCount; ++i){
            int c = teamArr[e->teamID].member[i];
            write(wfd[c][1], "A", 1);
            read(rfd[c][0], buf2, BUF);
        }
    }else return -1;
    return 1; 
}
