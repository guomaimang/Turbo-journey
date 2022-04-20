#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "child.h"
#include "util.h"


int hasmeeting[eventSize];


int Child(childInput *input){
    close(input->c2f[0]);
    close(input->f2c[1]);
    char buf[BUF] = ""; 
    person me = personArr[input->id];

    while(1){
        int readsize = read(input->f2c[0], buf, BUF); 
        while(readsize == 0) 
            readsize = read(input->f2c[0], buf, BUF); 
        buf[readsize] = 0;
//        char **rem = NULL;
//        char *token = strtok_r(buf, "$", rem);
        if(buf[0] == 'F')
            break;
        else if(buf[0] == 'T'){
            //try to add
            int idx;
            printf("C: buf = %s\n", buf);

            sscanf(buf, "T$3$%d", &idx);
            eventArr[idx].index = idx;
            sscanf(buf, "T$3$%d$%d$%d$%d$%d$%[^$]$%s", &eventArr[idx].index, &eventArr[idx].teamID, &eventArr[idx].holdDay, &eventArr[idx].startTime, 
                    &eventArr[idx].endTime, eventArr[idx].name, eventArr[idx].project);
            int nowday = eventArr[idx].holdDay;
            int success = 1;
            int i;
            int ret[idx];
            memset(ret, 0, sizeof(ret));
            for(i = eventArr[idx].startTime; i <eventArr[idx].endTime; ++i){
                if(myCalendar[nowday][i-9] >= 0){
                    success = 0; 
                    ret[myCalendar[nowday][i-9]] = 1;
                }
            }
            char buf2[BUF] = "";
            if(success){
                sprintf(buf2, "Y");
                write(input->c2f[1], buf2, 1);
            }
            else{
                sprintf(buf2, "N$50$");
                int ss[idx];
                memset(ss, 0, sizeof(ss));
                int sscnt = 0;
                for(i=0; i<idx; ++i)
                    if(ret[i])
                        ss[sscnt++] = i;
                sprintf(buf2 + 5, "%d", sscnt);
                for(i=0; i<sscnt; ++i)
                    sprintf(buf2 + strlen(buf2), "$%d", ss[i]);
                write(input->c2f[1], buf2, strlen(buf2));
            }
            readsize = read(input->f2c[0], buf, BUF);
            buf[readsize] = 0;
            if(buf[0] == 'A'){
                
                for(i = eventArr[idx].startTime; i <eventArr[idx].endTime; ++i){
                    int nowevent = myCalendar[nowday][i-9]; 
                    if(nowevent >= 0){
                        int j;
                        for(j=0; j<9; ++j)
                            if(nowevent == myCalendar[nowday][j])
                                myCalendar[nowday][j] = 0;
                        hasmeeting[nowevent] = 0;
                    }
                    myCalendar[nowday][i-9] = idx;
                }
                hasmeeting[idx] = 1;
            }else if(buf[0] == 'C'){

            }else return -1;
            sprintf(buf2, "D");
            write(input->c2f[1], buf2, 1);
        }
        else if(buf[0] == 'P'){
            int tfd = 0;
            int d1, d2;
            char fileName[30] = "";
            sscanf(buf, "P$%[^$]$%d$%d", fileName, &d1, &d2);
            tfd = open(fileName, O_WRONLY | O_APPEND);
            debug("C: get buf %s\n", buf);
            debug("C: get int %d %d %d\n",tfd, d1, d2);
            write(tfd, "===========================================================================\n", 76);
            char wbuf[BUF] = "";
            sprintf(wbuf, "Staff: %s\n", me.name);
            write(tfd, wbuf, strlen(wbuf));
            sprintf(wbuf, "Date\t\t\tStart\t\tEnd\t\tTeam\t\tProject\n");
            write(tfd, wbuf, strlen(wbuf));
            sprintf(wbuf, "===========================================================================\n");
            write(tfd, wbuf, strlen(wbuf));
            int i;
            int vis[200] = {0};
            for(i = d1; i <= d2; ++i){
               int j;
               for(j = 0; j < 9; ++j){
                   int idx = myCalendar[i][j];
                    if(idx >= 0 && !vis[idx]){
                        vis[idx] = 1;
                        sprintf(wbuf, "%s\t\t%s\t\t%s\t%s\t\t%s\n", toDate[i], toTime[eventArr[idx].startTime-9], toTime[eventArr[idx].endTime-9], eventArr[idx].name, eventArr[idx].project);
                        write(tfd, wbuf, strlen(wbuf));
                    }
               }
            }
            write(tfd, "\n", 1);
            close(tfd);
            write(input->c2f[1], "D", 1);
			//puts("finished");
        }
//        else if(token[0] == 'I'){
//            token = strtok_r(NULL, "$", rem);
//            if(token[0] !='1')
//                return -1;
//            sscanf(*rem, "%d$%d$%d$%s", &me.index, &me.manageTeam, &me.asMember, me.name);
 //           write(input->c2f[1], "D", 1);
//        }
        else if(buf[0] == 'R'){
            int xx, yy;
            sscanf(buf, "R$%d$%d", &xx, &yy);
            int i;
            int anss = 0;
            int vis[200] = {0};
            for(i = xx; i <= yy; ++i){
               int j;
               for(j = 0; j < 9; ++j){
                    if(myCalendar[i][j] >= 0 && !vis[myCalendar[i][j]]){
                        vis[myCalendar[i][j]] = 1;
                        ++anss;
                    }
               }
            }
            char buf3[10];
            sprintf(buf3, "A$%d", anss);
            write(input->c2f[1], buf3, 10);
        }
    }
    exit(0);
    return 0;
}
