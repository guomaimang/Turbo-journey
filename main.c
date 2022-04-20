#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "util.h"
#include "F1.h"
#include "F2.h"
//1
//Team Pro Eva Fanny Gray Billy
//0
//2
//Team 2002-04-25 09:00 2

event str2event(char *str,team team_list[6]) {
    event ans = {};
    int day;
    char *p = strtok(str, " -");
    strcpy(ans.name,p);
    ans.teamID=-1;
    int i;
    for(i=0;i<6;i++){
//        printf("name: %s\n", team_list[i].name);
        if(strcmp(team_list[i].name,ans.name)==0){
            ans.teamID=i;
        }
    }
    if(ans.teamID==-1){
        ans.holdDay=-1;
//        puts("error 1");
        return ans;
    }
    p = strtok(NULL, " -");
    if (strcmp(p, "2022") != 0) {
        ans.holdDay = -1;
//        puts("error 2");
        return ans;
    }
    p = strtok(NULL, " -");
    if (strcmp(p, "05") == 0) {
        p = strtok(NULL, " -");
        day = atoi(p);
        if (day == 1 || day == 8 || day <= 2 || day > 14) {
            ans.holdDay = -1;
            return ans;
        } else if(day<8){
            ans.holdDay = day+4;
        }else{
            ans.holdDay = day+3;
        }
    } else if (strcmp(p, "04") == 0) {
        p = strtok(NULL, " -");
        ans.holdDay = atoi(p) - 25;
    } else {
        ans.holdDay = -1;
//        puts("error 3");
        return ans;
    }
    p = strtok(NULL, " -:");
    int start_time=atoi(p);
    if(start_time<9 || start_time>17){
        ans.holdDay = -1;
//        puts("error 4");
        return ans;
    }else{
        ans.startTime = start_time;
    }
    p = strtok(NULL, " -:");
    p = strtok(NULL, " -:");
    ans.endTime = ans.startTime+atoi(p);

    for (i=0;i<6;i++){
        if (strcmp(team_list[i].name,ans.name)==0){
            strcpy(ans.project ,team_list[i].project);
        }
    }
    return ans;
}
void print_event(event e, person person_list[9]) {
    printf("team: %s\nhold date: %d\n start_time: %d\n Endtime:%d\n", e.name, e.holdDay,e.startTime,e.endTime);
}

int member_id(char *name, person person_list[9]) {
    int i;
    for (i = 0; i < 9; i++) {
        if (strcmp(name, person_list[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

int getDate(int month, int day){
    static const int d2d[15] = {0, 0, 6, 7, 8, 9, 10, 11, 0, 12, 13, 14, 15, 16, 17};
    if(month == 4) return day-25;
    else return d2d[day];
}

int create_team(char* str,person personArr[9], team *ans) {
    char *p = strtok(str, " ");
//    printf("name=%s\n", p);
    strcpy(ans->name,p);

    p = strtok(NULL, " ");
    strcpy(ans->project,p);
//    printf("project=%s\n", p);

    p = strtok(NULL, " ");
    ans->member[0] = ans->manager = member_id(p, personArr);
//    printf("manager=%s\n", p);

    ans->memberCount = 1;
    int i;
    for (i = 1; i < 4; i++) {
        p = strtok(NULL, " ");
        if(p == NULL) break;
        ++ans->memberCount;
        ans->member[i] = member_id(p, personArr);
        personArr[ans->member[i]].asMember+=1;
        if(personArr[ans->member[i]].manageTeam!=-1){
            return -1;
        }else if(personArr[ans->member[i]].asMember>=3){
            return -1;
        }
    }
    for(;i<4;i++) ans->member[i]=-1;
    return 0;
}

void print_team(team *t, person person_list[9],person personArr[9]) {

    printf("team: %s\nteam leader:%s\nteam_member:%s, %s, %s\n", t->name, personArr[t->manager].name,
           person_list[t->member[0]].name, person_list[t->member[1]].name, person_list[t->member[2]].name);

}

int menu(int *type) {
    char ans[20];
    printf("\n\n~~ WELCOME TO PolyStar ~~\n\n");
    printf("1. \tCreate Project Team\n\n");
    printf("2. \tProject Meeting Request\n");
    printf("\t2a. \tSingle input\n"
           "\t2b. \tBatch input\n"
           "\t2c. \tMeeting Attendance\n\n");
    printf("3. \tPrint Meeting Schedule\n");
    printf("\t3a. \tFCFS YYY1-M1-D1 YYY2-M2-D2 (First Come First Served)\n"
           "\t3b. \tGARR YYY1-M1-D1 YYY2-M2-D2 (Another algorithm implemented)\n"
           "\t3c. \tREPO YYY1-M1-D1 YYY2-M2-D2 (Attendance Report)\n\n");
    printf("4. \tExit\n\n"
           "Enter an option: ");
    //scanf("%s", ans);
    gets_s(ans, stdin);
    int re = ans[0] - '0';
    if(re == 2 || re == 3){
        if(strlen(ans) != 2)
            return -1;
        *type = ans[1] - 'a' + 1;
    }
    return re;
}

// person personArr[9]={
//		{0,"Alan",-1,0},
//		{1,"Billy",-1,0},
//		{2,"Cathy",-1,0},
//		{3,"David",-1,0},
//		{4,"Eva",-1,0},
//		{5,"Fanny",-1,0},
//		{6,"Gray",-1,0},
//		{7,"Helen",-1,0}
// };

int getPrintCommand(char* user_input, char* pipeDt){
    static char s1[10], s2[20], s3[20];
    static const int d2d[15] = {0, 0, 6, 7, 8, 9, 10, 11, 0, 12, 13, 14, 15, 16, 17};
    sscanf(user_input, "%s %s %s", s1, s2, s3);
    int y1, m1, d1, y2, m2, d2;
    sscanf(s2, "%d-%d-%d", &y1, &m1, &d1);
    int date1 = 0;
    if(m1 == 4)
        date1 = d1-25;
    else if(m1==5)
        date1 = d2d[d1];
    sscanf(s3, "%d-%d-%d", &y2, &m2, &d2);
    int date2 = 0;
    if(m2 == 4)
        date2 = d2-25;
    else if(m2==5)
        date2 = d2d[d2];
    sprintf(pipeDt, "P$%d$%d$", date1, date2);
    return 0; 
}

#define NUM_ALG 2
int main(int argc, char *argv[]) {
    int next_meeting = 0, next_team = 0, pid;
    int fd[2][2];
    int fda[2][2]; //write to F
    int i;
    for (i=0;i<NUM_ALG;i++){
        if(pipe(fd[i])<0 || pipe(fda[i])<0) {
			printf("pipe error\n");
			exit(1);
		}
    }

    int part_num =0;
    char user_input_buf[100] = "1";

    for (i = 0; i < NUM_ALG ;i++) {
        pid = fork();
        if (pid < 0) {
            printf("fork failed");
        } else if (pid == 0) {
            prctl(PR_SET_PDEATHSIG, SIGKILL);
            if(i == 0){
                F1main(fd, fda);
            }else {
				F2main(fd,fda);
			}
            exit(0);
        }
    }

    if(pid!=0) {
        for (i=0;i<NUM_ALG;i++) {
            close(fd[i][0]);
            close(fda[i][1]);
        }

        char buf[BUF] = "";
        char buf2[BUF] = "";
        int input_type = 0;
        part_num=menu(&input_type);
        while (part_num != 4) {
            switch (part_num) {
                case 1 :
                    //                这里应该是一个while循环，直到用户输入“0”之后才结束
                    printf("Please input information of the team in the format :\n\"Team_X Project_X Leader_name Member_name_1 Member_name_2 Member_name_3\"\n");
//                    printf("Enter > ");
//                    getchar();
//                    fgets(user_input_buf,100,stdin);
//                    gets_s(user_input_buf);
//                    printf("Input is: %s\n", user_input_buf);
                    while (1) {
                        printf("Enter > ");
                        gets_s(user_input_buf, stdin);
                        if(user_input_buf[0] == '0')
                            break;
                        team temp = {0};
                        int valid = create_team(user_input_buf, personArr, &temp);
//                        print_team(&temp, personArr, personArr);
                        if (next_team >= 5) {
                            printf("Already 5 teams");
                            continue;
                        } else if (valid == -1) {
                            printf("Invalid format, Please try again\n");
                            continue;
                        }
                        for (i = next_team-1; i >= 0; i--) {
                            if (temp.manager == teamArr[i].manager || strcmp(temp.name, teamArr[i].name) == 0) {
                                printf("Invalid team information, Please try again\n");
                                valid = -1;
                                break;
                            }
                        }
                        if(valid == -1)
                            continue;
                        personArr[temp.manager].manageTeam = next_team;
                        temp.index=next_team;
                        team2str('G', &temp, buf);
                        write(fd[0][1], buf, strlen(buf));
						write(fd[1][1], buf, strlen(buf));

                        int numget = read(fda[0][0],buf2,100);
                        while(numget == 0) numget = read(fda[0][0], buf2, 100);
						numget = read(fda[1][0],buf2,100);
                        while(numget == 0) numget = read(fda[1][0], buf2, 100);
						
						
                        buf2[numget] = 0;
                        strcpy(buf,"");
                        teamArr[next_team++] = temp;
                        printf("Team \"%s\" for project \"%s\" is created\n",temp.name,temp.project);
               //         printf("%s",team2str('A',&temp,buf));
                    }
                    break;
                case 2 :
                    strcpy(user_input_buf, "");
                    event temp;
                    while (1) {
                        printf("Enter > ");
//                        getchar();
//                    fgets(user_input_buf,100,stdin);
                        gets_s(user_input_buf, stdin);
                        if(user_input_buf[0] - '0' == 0) break;
                        printf("Input is: %s\n", user_input_buf);
                        if (input_type == 1) {
                            temp = str2event(user_input_buf, teamArr);
                            if (temp.holdDay != -1) {
                                temp.index=next_meeting;
                                eventArr[next_meeting] = temp;
                                event2str('E', &temp, buf);
                                write(fd[0][1], buf, strlen(buf));
								write(fd[1][1], buf, strlen(buf));
								
                                int numget = read(fda[0][0],buf2,100);
                                while(numget == 0) numget = read(fda[0][0], buf2, 100);
								numget = read(fda[1][0],buf2,100);
                                while(numget == 0) numget = read(fda[1][0], buf2, 100);
								
								
                                buf2[numget] = 0;
                                debug("FF 1: Finish!\n");
                                debug("fda = %d, sum = %s\n", fda[0][0], buf2);
                                next_meeting++;
//                                print_event(temp, personArr);
                            } else {
                                printf("Invalid input, please try again.\n");
                            }
                            continue;
                        }
                        else if(input_type == 2){
                            FILE* f = fopen(user_input_buf, "r");
                            if (f == NULL) {
                                printf("There is no such file. Please try again.\n");
                                continue;
                            }
                            while (1) {
//                    fgets(user_input_buf,100,stdin);
                                int rett = gets_s(user_input_buf, f);
								//printf("%d: ",rett);
                                if(rett == 0 || rett == -1){
                                    break;
                                }
                                if (strncmp(user_input_buf, "Team_", 5) == 0) {
									//puts(user_input_buf);
                                    temp = str2event(user_input_buf, teamArr);
                                    if (temp.holdDay != -1) {
                                        temp.index=next_meeting;
                                        eventArr[next_meeting] = temp;
                                        next_meeting++;
                                        event2str('E', &temp, buf);
										//printf("main: ");puts(buf);
                                        write(fd[0][1], buf, strlen(buf));
										write(fd[1][1], buf, strlen(buf));
										
                                        int numget = read(fda[0][0],buf2,100);
                                        while(numget == 0) numget = read(fda[0][0], buf2, 100);
										numget = read(fda[1][0],buf2,100);
                                        while(numget == 0) numget = read(fda[1][0], buf2, 100);
                                        buf2[numget] = 0;
                                 //       debug("2 FF: Finish!\n");
                                 //       debug("fda = %d, sum = %s\n", fda[0][0], buf2);
//                                      print_event(temp, personArr);
                                    }
                                }
                            }
                            fclose(f);
                            //fflush(stdin);
                        }
                        else if(input_type == 3){
                            gets_s(user_input_buf, stdin);
                            event temp = str2event(user_input_buf, teamArr);
                            sprintf(buf, "B$%d$%d$%d$%d", temp.teamID, temp.holdDay, temp.startTime, temp.endTime);
                            write(fd[0][1], buf, strlen(buf));
                            int numget = read(fda[0][0],buf2,100);
                            while(numget == 0) numget = read(fda[0][0], buf2, 100);
                        }
                    }
                    break;
                case 3 :
//                    getchar();
//                    fgets(user_input_buf,100,stdin);
                    printf("Enter > ");
                    gets_s(user_input_buf, stdin);
                    if(input_type == 1){
                        getPrintCommand(user_input_buf, buf);
                    //    puts("FF: Start Sending...");
                        write(fd[0][1], buf, strlen(buf));
                        strcpy(buf2, "");
                        int numget = read(fda[0][0],buf2,100);
                        while(numget == 0) numget = read(fda[0][0], buf2, 100);
                        buf2[numget] = 0;
                    }else if(input_type == 2){
                        getPrintCommand(user_input_buf, buf);
						write(fd[1][1], buf, strlen(buf));
                        strcpy(buf2, "");
                        int numget = read(fda[1][0],buf2,100);
                        while(numget == 0) numget = read(fda[1][0], buf2, 100);
                        buf2[numget] = 0;
                    }else if(input_type == 3){
                        char date1[15], date2[15];
                        sscanf(user_input_buf, "REPO %s %s", date1, date2);
                        int m1, d1, m2, d2;
                        sscanf(date1, "2022-%d-%d", &m1, &d1);
                        sscanf(date2, "2022-%d-%d", &m2, &d2);
                        sprintf(buf, "R$%d$%d$", getDate(m1, d1), getDate(m2, d2));
                        write(fd[0][1], buf2, strlen(buf));
                        int numget = read(fda[0][0],buf2,100);
                        while(numget == 0) numget = read(fda[0][0], buf2, 100);
                    }
                    //                对用户输入进行转义，之后进行输出
                    //                do part 3
                    // printf("%s", user_input_buf);
                    break;
                case 4:
                    write(fd[0][1],"F",strlen("F"));
                    for(i=0; i<NUM_ALG; ++i)
                        wait(NULL);
                    break;
                default:
                    printf("Invalid input, please try again.\n=========================================\n\n");
            }
            part_num = menu(&input_type);
        }
        printf("\n~~~~~Thank you, wish you have a nice day!~~~~~\n");

    }
    return 0;
}
