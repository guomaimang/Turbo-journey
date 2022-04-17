#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "util.h"
#include "child.h"
#include <fcntl.h>
#include <sys/stat.h>


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

// void event2str(char sig, event e, char str[101]){
//    sprintf(str,"%c$3$%d$%d$%d$%d$%d$%s$%s",sig,e.index,e.teamID,e.holdDay,e.startTime,e.endTime,e.name,e.project);
//}

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

//int main(int argc,char* argv[]) {
int F1main(int GPfd[2][2], int Ffd[2][2]) {
/*
// ------从FF得到 FF -> F 和 F -> FF 的管道，代码整合后将删除------
    int GPfd[2][2];  // pipe 1 for F1, pipe 2 for F2, FF -> F
    char GPbuf[2][101];
    int Ffd[2][2];   // F -> FF
    char Fbuf[2][101];

    if (pipe(GPfd[0]) < 0 || pipe(GPfd[1]) < 0 || pipe(Ffd[0]) < 0 || pipe(Ffd[1]) < 0) {
        printf("F1: UNEXPECT ERROR!\n");
    }
*/
    char GPbuf[2][101];
    char Fbuf[2][101];
    close(GPfd[0][1]);
    close(GPfd[1][1]);
    close(Ffd[0][0]);
    close(Ffd[1][0]);
// ------------------------------------------------------------

// Init
    int pid, i, j;
    int eventUsage = 0;
    for (i = 0; i < 18; ++i) {
        for (j = 0; j < 9; ++j) {
            myCalendar[i][j] = -1;
        }
    } // init myCalendar[i][j] = -1

// create child pipe
    int f1fd[8][2];     // F1 -> Cx
    char f1buf[8][101];
    int cfd[8][2];      // Cx -> F1
    char cbuf[8][101];
    for (i = 0; i < 8; ++i) {
        if (pipe(f1fd[i]) < 0 || pipe(cfd[i]) < 8) {
            printf("F1: Pipe creation error!\n");
            exit(1);
        }
    }

// close useless pipe
    for (i = 0; i < 8; ++i) {
        close(f1fd[i][0]);  // close read pip of F1
        close(cfd[i][1]);   // close write pipe of child
    }


/*
 * ============================================================
 * Child Part Start
 * ============================================================
 */
    for (i = 0; i < 8; ++i) {
        pid = fork();
        if (pid < 0) {
            printf("F1: Fork fail!\n");
        } else if (pid == 0) {
            int childId = i; // from 0 to 7
            // Write Cx code
            childInput input = (childInput) {f1fd[i], cfd[i]};
            FCFSChild(&input);
        }
    }
/*
 * ===========================================================
 * Child Part End
 * ===========================================================
 */

/*
 * ===========================================================
 * F Mode: Client of FF
 * ===========================================================
 */
    while (1) {
        // get message from FF
        // sleep(1);
        int np;
        np = read(GPfd[0][0], GPbuf[0], 101);
        if (np <= 0) {
            continue;
        } // no message
        GPbuf[0][np] = 0;
        char signal = GPbuf[0][0];

        // FF pass F1 a new team/group
        if (signal == 'G') {
            team oneTeam;
            char tempStr0[101];

            fillString(tempStr0, 1, GPbuf[0]);
            oneTeam.index = atoi(tempStr0);                        // get index

            teamArr[oneTeam.index] = oneTeam;                      // copy to teamArr

            fillString(tempStr0, 2, GPbuf[0]);
            teamArr[oneTeam.index].manager = atoi(tempStr0);       // get manager index

            fillString(tempStr0, 3, GPbuf[0]);
            teamArr[oneTeam.index].memberCount = atoi(tempStr0);   // get member count

            for (i = 0; i < teamArr[oneTeam.index].memberCount; ++i) {
                fillString(tempStr0, 4 + i, GPbuf[0]);
                teamArr[oneTeam.index].member[i] = atoi(tempStr0); // get every member index
            }

            fillString(tempStr0, teamArr[oneTeam.index].memberCount + 4, GPbuf[0]);
            strcpy(teamArr[oneTeam.index].name, tempStr0);          // get team name

            fillString(tempStr0, teamArr[oneTeam.index].memberCount + 5, GPbuf[0]);
            strcpy(teamArr[oneTeam.index].project, tempStr0);       // get project name

            // response FF ack
            Fbuf[0][0] = 'D';
            write(Ffd[0][1], Fbuf[0], 101);
        }

        // FF pass F1 to a new event
        if (signal == 'E') {
            event oneEvent;
            char tempStr0[101];

            fillString(tempStr0, 1, GPbuf[0]);
            oneEvent.index = atoi(tempStr0);     // get index

            eventArr[oneEvent.index] = oneEvent;
            eventUsage++;                        // copy to eventArr

            fillString(tempStr0, 2, GPbuf[0]);
            oneEvent.teamID = atoi(tempStr0);

            fillString(tempStr0, 3, GPbuf[0]);
            oneEvent.holdDay = atoi(tempStr0);

            fillString(tempStr0, 4, GPbuf[0]);
            oneEvent.startTime = atoi(tempStr0);

            fillString(tempStr0, 5, GPbuf[0]);
            oneEvent.endTime = atoi(tempStr0);

            fillString(tempStr0, 6, GPbuf[0]);
            strcpy(oneEvent.name, tempStr0);

            fillString(tempStr0, 6, GPbuf[0]);
            strcpy(oneEvent.project, tempStr0);

            // response FF ack
            Fbuf[0][0] = 'D';
            write(Ffd[0][1], Fbuf[0], 101);
        }

        // Print: Tell every child to print their calendar
        if (signal == 'P') {
            // Init file
            int infd = open("Schedule_FCFS.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
            char inbuf[101];
            if (infd < 0) {
                printf("Error in opening input file\n");
                exit(1);
            }

            // get start date and end date
            char tempStr0[101];
            int dayStart, dayEnd;
            fillString(tempStr0, 1, GPbuf[0]);
            dayStart = atoi(tempStr0);
            fillString(tempStr0, 2, GPbuf[0]);
            dayEnd = atoi(tempStr0);

            // log if event is successfully arranged
            int eventSuccess[eventUsage];
            for (i = 0; i < eventUsage; ++i) {
                eventSuccess[i] = 0;
            }

            // schedule each event i
            for (i = 0; i < eventUsage; ++i) {
                int mod = 0;    // modify times, 0 to 37 -> -18 to +18

                while (mod < 37) {
                    int arrangement = 1;
                    char message[101];
                    int memberCount = teamArr[eventArr[i].teamID].memberCount;

                    // for each child index c
                    for (j = 0; j < memberCount; ++j) {
                        int c = teamArr[eventArr[i].teamID].member[j];
                        // 1. tell child c to try
                        event2str('E', eventArr[i], message);
                        strcpy(f1buf[c], message);
                        write(f1fd[c][1], f1buf[c], 101);
                        // 2. listen child c's feedback
                        while (1) {
                            sleep(1);
                            np = read(cfd[c][0], cbuf[c], 101);
                            if (np <= 0) {
                                continue;
                            }
                            if (cbuf[j][0] == 'N') {
                                arrangement = 0;
                                break;
                            } else if (cbuf[j][0] == 'Y') {
                                break;
                            }
                        }
                    }

                    // 3. if all feedbacks say available, tell child do and break this while
                    if (arrangement == 1) {
                        // for each child index c
                        for (j = 0; j < memberCount; ++j) {
                            int c = teamArr[eventArr[i].teamID].member[j];
                            // 1. tell child c to add
                            write(f1fd[c][1], f1buf[c], 101);
                            // 2. wait ack from C
                            while (1) {
                                np = read(cfd[c][0], cbuf[c], 101);
                                if (np <= 0) {
                                    continue;
                                }
                                if (cbuf[c][0] == 'D') {
                                    break;
                                }
                            }
                        }
                        eventSuccess[i] = 1;
                        myCalendar[eventArr[i].holdDay][eventArr[i].startTime] = i;
                        break;
                    } else {
                        // 4. if anyone feedback says unavailable or time illegal, modify time and jump to 1
                        sleep(1);
                        event *eventPtr = &eventArr[i];
                        int modOutcome = modTime(++mod, eventPtr);
                        if (modOutcome == 1 || mod > 37) {
                            break;
                        }
                    }
                }
            }

            // after scheduling, print all event
            sprintf(inbuf, "*** Project Meeting ***\n");
            write(infd, inbuf, 101);
            sprintf(inbuf, "\nAlgorithm used: FCFS\n");
            write(infd, inbuf, 101);
            sprintf(inbuf, "Period: %s to %s\n", toDate[dayStart], toDate[dayEnd]);
            write(infd, inbuf, 101);
            sprintf(inbuf, "\nDate          Start    End      Team          Project   \n");
            write(infd, inbuf, 101);
            sprintf(inbuf, "========================================================\n");
            write(infd, inbuf, 101);
            for (i = 0; i < 18; ++i) {
                for (j = 0; j < 9; ++j) {
                    int eventIndex = myCalendar[i][j];
                    if (eventIndex == -1) {
                        continue;
                    }
                    sprintf(inbuf, "%s    %s    %s    %s    %s\n", toDate[eventArr[eventIndex].holdDay],
                            toTime[eventArr[eventIndex].startTime],
                            toTime[eventArr[eventIndex].endTime],
                            teamArr[eventArr[eventIndex].teamID].name,
                            teamArr[eventArr[eventIndex].teamID].project);
                    write(infd, inbuf, 101);
                }

            }
            sprintf(inbuf, "========================================================\n");
            write(infd, inbuf, 101);

            // count rejected events
            int rejectCount;
            for (i = 0; i < eventUsage; ++i) {
                if (eventSuccess[i] == 0) {
                    rejectCount++;
                }
            }
            sprintf(inbuf, "*** Meeting Request–REJECTED ***\n");
            write(infd, inbuf, 101);
            sprintf(inbuf, "\nThere are %d requests rejected for the required period.\n\n", rejectCount);
            write(infd, inbuf, 101);
            int rowNum = 0;
            for (i = 0; i < eventUsage; ++i) {
                if (eventSuccess[i] == 0) {
                    printf("%d\t%s\t%s\t%s\t%d\n", ++rowNum,
                           eventArr[i].name,
                           toDate[eventArr[i].holdDay],
                           toTime[eventArr[i].startTime],
                           eventArr[i].endTime - eventArr[i].startTime);
                    write(infd, inbuf, 101);
                }

            }
            printf("========================================================\n");
            write(infd, inbuf, 101);

            // tell all child to print
            for (i = 0; i < 8; ++i) {
                sprintf(cbuf[i], "%d$%d$%d", infd, dayStart, dayEnd);
                write(f1fd[i][1], f1fd[i], 101);
                while (1) {
                    // wait ack from C
                    np = read(cfd[i][0], cbuf[i], 101);
                    if (np <= 0) {
                        continue;
                    }
                    if (cbuf[i][0] == 'D') {
                        break;
                    }
                }
            }
        }

        // End: Tell every child to end process
        if (signal == 'F') {
            for (i = 0; i < 8; ++i) {
                cbuf[i][0] = 'F';
                cbuf[i][1] = 0;
                write(cfd[i][1], cbuf, 101);
            }
            break;
        }
        }

        // close all pipe in the end
        for (i = 0; i < 8; ++i) {
            close(f1fd[i][1]);
            close(cfd[i][0]);
        }

        // wait for all children
        for (i = 0; i < 8; ++i) {
            wait(NULL);
        }

        return 0;
    }


