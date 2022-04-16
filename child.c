#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "child.h"
#include "util.h"


int myCalandar[20][10];
int hasmeeting[200];


int FCFSChild(childInput *input){
    close(input->c2f[0]);
    close(input->f2c[1]);
    char buf[BUF] = ""; 
    person me;

    while(1){
        int readsize = read(input->f2c[0], buf, BUF); 
        buf[readsize] = 0;
        char **rem = NULL;
        char *token = strtok_r(buf, "$", rem);
        if(token == NULL)
            return -1;
        if(token[0] == 'F')
            break;
        else if(token[0] == 'T'){
            //try to add
            token = strtok_r(NULL, "$", rem);
            if(token == NULL || token[0] != '3')
                return -1;
            else token = strtok_r(NULL, "$", rem);
            int idx;
            sscanf(token, "%d", &idx);
            eventArr[idx].index = idx;
            sscanf(*rem, "%d$%d$%d$%d$%s$%s", &eventArr[idx].teamID, &eventArr[idx].holdDay, &eventArr[idx].startTime, 
                    &eventArr[idx].endTime, eventArr[idx].name, eventArr[idx].project);
            int nowday = eventArr[idx].holdDay;
            int success = 1;
            int i;
            int ret[idx];
            memset(ret, 0, sizeof(ret));
            for(i = eventArr[idx].startTime; i <eventArr[idx].endTime; ++i){
                if(myCalandar[nowday][i-9] != 0){
                    success = 0; 
                    ret[myCalandar[nowday][i-9]] = 1;
                }
            }
            char buf2[BUF] = "";
            if(success){
                sprintf(buf2, "Y");
                write(input->c2f[1], buf2, BUF);
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
                write(input->c2f[1], buf2, BUF);
            }
            readsize = read(input->f2c[0], buf, BUF);
            buf[readsize] = 0;
            if(buf[0] == 'A'){
                
                for(i = eventArr[idx].startTime; i <eventArr[idx].endTime; ++i){
                    int nowevent = myCalandar[nowday][i-9]; 
                    if(nowevent != 0){
                        int j;
                        for(j=0; j<9; ++j)
                            if(nowevent == myCalandar[nowday][j])
                                myCalandar[nowday][j] = 0;
                        hasmeeting[nowevent] = 0;
                    }
                    myCalandar[nowday][i-9] = idx;
                }
                hasmeeting[idx] = 1;
            }else if(buf[0] == 'C'){

            }else return -1;
            sprintf(buf2, "D");
            write(input->c2f[1], buf2, BUF);
        }
        else if(token[0] == 'P'){
            token = strtok_r(NULL, "$", rem);
            int tfd = 0;
            sscanf(token, "%d", &tfd);
            dprintf(tfd, "=================================================\n");
            dprintf(tfd, "Staff: %s\n", me.name);
            dprintf(tfd, "Date\tStart\tEnd\tTeam\tProject\n");
            dprintf(tfd, "=================================================\n");
            int i;
            for(i=0; i<200; ++i)
                if(hasmeeting[i]){
                    dprintf(tfd, "%s\t%s\t%s\t%s\t%s\n", toDate[eventArr[i].holdDay], toTime[eventArr[i].startTime-9], toTime[eventArr[i].endTime-9], eventArr[i].name, eventArr[i].project);
                }
             dprintf(tfd, "\n");
            write(input->c2f[1], "D", 1);
        }
        else if(token[0] == 'T'){
            token = strtok_r(NULL, "$", rem);
            if(token[0] !='1')
                return -1;
            sscanf(*rem, "%d$%d$%d$%s", &me.index, &me.manageTeam, &me.asMember, me.name);
            write(input->c2f[1], "D", 1);
        }else if(token[0] == 'R'){
            int xx, yy;
            sscanf(*rem, "%d$%d", &xx, &yy);
            int i;
            int anss = 0;
            int vis[200] = {0};
            for(i = xx; i <= yy; ++i){
               int j;
               for(j = 0; j < 9; ++j){
                    if(myCalandar[i][j] > 0 && !vis[myCalandar[i][j]]){
                        vis[myCalandar[i][j]] = 1;
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
