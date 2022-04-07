#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include "util.h"


void fillString(char* output, int index, const char buf[101]){
    int cursor = -1;
    int counter = 0;
    int outputUsage = 0;

    while (1){
        cursor++;
        if(buf[cursor] == '$'){
            counter++;
        }

        if (counter == index){
            if(buf[cursor] == '$'){
                cursor++;
            }
            while (buf[cursor] != '$' && buf[cursor] != 0){
                output[outputUsage++] = buf[cursor++];
            }
            output[outputUsage] = 0;
            break;
        }

        if (buf[cursor] == 0){
            break;
        }
    }

}

// from main.c
char* event2str(char sig, event e, char str[100]){
    sprintf(str,"%c$3$%d$%d$%d$%d$%d$%s$%s",sig,e.index,e.teamID,e.holdDay,e.startTime,e.endTime,e.name,e.project);
    return str;
}

int modTime(int mod, event* targetEvent){
    // mod: from 0 to 37
    // modify time 0 -> 1 -> -1 -> 2 -> -2 ...... -> 18 -> -18 -> 0
    int addTime = 0;
    if (0 < mod && mod < 37){
        addTime = mod/2;
        if(mod%2 == 0){
            addTime = 0 - addTime;
        }
    }
    int timeNow = targetEvent -> holdDay * 18 + targetEvent -> startTime;
    // modify successfully with 1, else 0
    if (timeNow + addTime > 90){
        return 0;
    } else{
        timeNow = timeNow + addTime;
        targetEvent -> holdDay = timeNow / 18;
        targetEvent -> startTime = timeNow / 9;
        return 1;
    }
}

int main(int argc,char* argv[]) {

// ------从FF得到 FF -> F 和 F -> FF 的管道，代码整合后将删除------
    int GPfd[2][2];  // pipe 1 for F1, pipe 2 for F2, FF -> F
    char GPbuf[2][101];
    int Ffd[2][2];   // F -> FF
    char Fbuf[2][101];

    if (pipe(GPfd[0]) < 0 || pipe(GPfd[1]) < 0 || pipe(Ffd[0]) < 0 || pipe(Ffd[1]) < 0){
        printf("F1: UNEXPECT ERROR!\n");
    }
    close(GPfd[0][1]);
    close(GPfd[1][1]);
    close(Ffd[0][0]);
    close(Ffd[1][0]);
// ------------------------------------------------------------

// Init
    int pid;
    int eventUsage;

// create child pipe
    int f1fd[8][2];     // F1 -> Cx
    char f1buf[8][101];
    int cfd[8][2];      // Cx -> F1
    char cbuf[8][101];
    for (int i = 0; i < 8; ++i) {
        if(pipe(f1fd[i]) < 0 || pipe(cfd[i]) < 8){
            printf("F1: Pipe creation error!\n");
            exit(1);
        }
    }

// close useless pipe
    for (int i = 0; i < 8; ++i) {
        close(f1fd[i][0]);  // close read pip of F1
        close(cfd[i][1]);   // close write pipe of child
    }


/*
 * ============================================================
 * Child Part Start
 * ============================================================
 */
    for (int i = 0; i < 8; ++i) {
        pid = fork();
        if (pid < 0){
            printf("F1: Fork fail!\n");
        } else if (pid == 0){
            int childId = i; // from 0 to 7
            // Write Cx code
        }
    }
/*
 * ===========================================================
 * Child Part End
 * ===========================================================
 */

/*
 * ===========================================================
 * Mode: Client of FF
 * ===========================================================
 */
    while (1){
        // get message from FF
        sleep(1);
        int np;
        np = read(GPfd[0][0],GPbuf[0],101);
        if (np < 0){
            continue;
        } // no message
        char signal = GPbuf[0][0];

        // FF pass F1 a new team/group
        if (signal == 'G'){
            team oneTeam;
            char tempStr0[101];

            fillString(tempStr0,1,GPbuf[0]);
            oneTeam.index = atoi(tempStr0);     // get index
            fillString(tempStr0,2,GPbuf[0]);
            oneTeam.manager = atoi(tempStr0);   // get manager index
            fillString(tempStr0,3,GPbuf[0]);
            oneTeam.manager = atoi(tempStr0);   // get member1 index
            fillString(tempStr0,4,GPbuf[0]);
            oneTeam.manager = atoi(tempStr0);   // get member2 index
            fillString(tempStr0,5,GPbuf[0]);
            oneTeam.manager = atoi(tempStr0);   // get member3 index
            fillString(tempStr0,6,GPbuf[0]);
            strcpy(oneTeam.name,tempStr0);   // get team name

            teamArr[oneTeam.index] = oneTeam;
        }

        // FF pass F1 to a new event
        if (signal == 'E'){
            event oneEvent;
            char tempStr0[101];

            fillString(tempStr0,1,GPbuf[0]);
            oneEvent.index = atoi(tempStr0);     // get index
            fillString(tempStr0,2,GPbuf[0]);
            oneEvent.teamID = atoi(tempStr0);
            fillString(tempStr0,3,GPbuf[0]);
            oneEvent.holdDay = atoi(tempStr0);
            fillString(tempStr0,4,GPbuf[0]);
            oneEvent.startTime = atoi(tempStr0);
            fillString(tempStr0,5,GPbuf[0]);
            oneEvent.endTime = atoi(tempStr0);
            fillString(tempStr0,6,GPbuf[0]);
            strcpy(oneEvent.name,tempStr0);
            fillString(tempStr0,6,GPbuf[0]);
            strcpy(oneEvent.project,tempStr0);

            eventArr[oneEvent.index] = oneEvent;
            eventUsage++;
        }

        // Print: Tell every child to print their calendar
        if (signal == 'P'){

            // log if event is successful
            int eventSuccess[eventUsage];
            for (int i = 0; i < eventUsage; ++i) {
                eventSuccess[i] = 0;
            }

            // schedule event i
            for (int i = 0; i < eventUsage; ++i) {
                int mod = 0;

                while (mod < 37){
                    int arrangement = 1;
                    char message[101];
                    for (int j = 0; j < teamArr[i].memberCount; ++j) {

                        // 1. tell child j to try
                        event2str('E',eventArr[i],message);
                        strcpy(f1buf[j],message);
                        write(f1fd[j][1], f1buf[j],101);

                        // 2. listen child j's feedback
                        while(1){
                            sleep(1);
                            np = read(cfd[j][0],cbuf[j],101);
                            if (np < 0){
                                continue;}
                            if (cbuf[j][0] == 'N'){
                                arrangement = 0;
                                break;
                            } else if (cbuf[j][0] == 'Y'){
                                break;
                            }
                        }

                        // 3. if all feedbacks say available, tell child do and break
                        if (arrangement == 1){
                            while (1){
                                sleep(1);
                                // 1. tell child j to add
                                write(f1fd[j][1], f1buf[j],101);
                                // 2. wait ack
                                if (np < 0){
                                    continue;} else {break;}
                            }
                            eventSuccess[i] = 1;
                        }

                        // 4. if anyone feedback says unavailable or time illegal, modify time and jump to 1
                        if (arrangement == 0){
                            while (1){
                                sleep(1);
                                event* eventPtr = &eventArr[i];
                                int modOutcome = modTime(++mod,eventPtr);
                                if (modOutcome == 1 || mod > 37){
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // after schedule, print all event
            for (int i = 0; i < 8; ++i) {
                cbuf[i][0] = 'P';
                cbuf[i][1] = 0;
                write(cfd[i][1],cbuf,101);
            }
        }

        // End: Tell every child to end process
        if (signal == 'F'){
            for (int i = 0; i < 8; ++i) {
                cbuf[i][0] = 'F';
                cbuf[i][1] = 0;
                write(cfd[i][1],cbuf,101);
            }
            break;
        }
    }


// close all pipe in the end
    for (int i = 0; i < 8; ++i) {
        close(f1fd[i][1]);
        close(cfd[i][0]);
    }

// wait for all children
    for (int i = 0; i < 8; ++i) {
        wait(NULL);
    }

    return 0;
}

