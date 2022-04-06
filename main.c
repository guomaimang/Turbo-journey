#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "util.h"
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
    for(int i =0;i<6;i++){
        if(strcmp(team_list[i].name,ans.name)==0){
            ans.teamID=i;
        }
    }
    if(ans.teamID==-1){
        ans.holdDay=-1;
        return ans;
    }
    p = strtok(NULL, " -");
    if (strcmp(p, "2022") != 0) {
        ans.holdDay = -1;
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
        return ans;
    }
    p = strtok(NULL, " -:");
    int start_time=atoi(p);
    if(start_time<9 || start_time>17){
        ans.holdDay = -1;
        return ans;
    }else{
        ans.startTime = start_time;
    }
    p = strtok(NULL, " -:");
    p = strtok(NULL, " -:");
    ans.endTime = ans.startTime+atoi(p);

    for (int i =0;i<6;i++){
        if (strcmp(team_list[i].name,ans.name)==0){
            strcpy(ans.project ,team_list[i].project);
        }
    }
    return ans;
}
void print_event(event e, person person_list[9]) {
    printf("team: %s\nhold date: %d\n start_time: %d\n Endtime:%d\n", e.name, e.holdDay,e.startTime,e.endTime);
}

char* team2str(char sig,team t,char str[100]){
    sprintf(str,"%c$2$%d$%s$%s$%d$%d$%d$%d$%d",sig,t.index,t.name,t.project,t.manager,t.memberCount,t.member[0],t.member[1],t.member[2]);
    return str;
}
char* event2str(char sig,event e,char str[100]){
    sprintf(str,"%c$3$%d$%d$%d$%d$%d$%s$%s",sig,e.index,e.teamID,e.holdDay,e.startTime,e.endTime,e.name,e.project);
    return str;
}

int member_id(char *name, person person_list[9]) {
    for (int i = 0; i < 9; i++) {
        if (strcmp(name, person_list[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

team create_team(char* str,person personArr[9]) {
    team ans = {};
    char *p = strtok(str, " ");
    strcpy(ans.name,p);

    p = strtok(NULL, " ");
    strcpy(ans.project,p);

    p = strtok(NULL, " ");
    ans.manager = member_id(p, personArr);

    for (int i = 0; i < 3; i++) {
        p = strtok(NULL, " ");
        ans.member[i] = member_id(p, personArr);
        personArr[ans.member[i]].asMember+=1;
        if(personArr[ans.member[i]].manageTeam!=-1){
            ans.member[0]=-1;
            return ans;
        }else if(personArr[ans.member[i]].asMember>=3){
            ans.member[0]=-1;
            return ans;
        }
    }
    for(int i=0;i<3;i++){
        if(ans.member[i]>=0){
            ans.memberCount=(i+1);
        }
    }
    return ans;
}

void print_team(team t, person person_list[9],person personArr[9]) {

    printf("team: %s\nteam leader:%s\nteam_member:%s, %s, %s\n", t.name, personArr[t.manager].name,
           person_list[t.member[0]].name, person_list[t.member[1]].name, person_list[t.member[2]].name);

}

int menu() {
    char ans[5];
    printf("\n\n~~ WELCOME TO PolyStar ~~\n\n");
    printf("1. \tCreate Project Team\n\n");
    printf("2. \tProject Meeting Request\n");
    printf("\t2a. \tSingle input\n"
           "\t2b. \tBatch input\n"
           "\t2c. \tMeeting Attendance (if implemented)\n\n");
    printf("3. \tPrint Meeting Schedule\n");
    printf("\t3a. \tFCFS (First Come First Served)\n"
           "\t3b. \tXXXX (Another algorithm implemented)\n"
           "\t3c. \tYYYY (Attendance Report) (if implemented)\n\n");
    printf("4. \tExit\n\n"
           "\tEnter an option:");
    scanf("%s", ans);
    int re = ans[0] - '0';
    return re;
}

int main(int argc, char *argv[]) {
    event eventArr[200]={};
    team teamArr[6]={};
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
    int next_meeting = 0, next_team = 0, pid, f_number = 0;
    int fd[2][2];
    int fda[2][2];
    for (int i=0;i<2;i++){
        if(pipe(fd[i])<0)printf("pipe error\n");
        if(pipe(fda[i])<0)printf("pipe error\n");
    }

    int part_num = menu();
    char user_input_buf[100] = "1";

    for (int i = 0; i < 2 ;i++) {
        pid = fork();
        if (pid < 0) {
            printf("fork failed");
        } else if (pid == 0) {
            break;
        }
        f_number++;
    }

    if(pid==0) {
        for (int i=0;i<2;i++){
            close(fd[i][1]);
            close(fda[i][0]);
        }

        char* buf[100];

        while (part_num != 4) {
            part_num=menu();
            switch (part_num) {
                case 1 :
                    //                这里应该是一个while循环，直到用户输入“0”之后才结束
                    printf("Please input information of the team in the format :\n\"Team_X Project_X Leader_name Member_name_1 Member_name_2 Member_name_3\"\n");
                    printf("Enter > ");
                    gets(user_input_buf);
                    printf("Input is: %s\n", user_input_buf);
                    while ((user_input_buf[0] - '0') != 0) {

                        team temp = create_team(user_input_buf, personArr);

                        printf("Enter > ");
                        gets(user_input_buf);

                        print_team(temp, personArr, personArr);
                        if (next_team >= 5) {
                            printf("Already 5 teams");
                            continue;
                        } else if (temp.member[0] == -1) {
                            printf("Invalid format, Please try again\n");
                            continue;
                        }
                        for (int i = next_team; i >= 0; i--) {
                            if (temp.manager == teamArr[i].manager || strcmp(temp.name, teamArr[i].name) == 0) {
                                printf("Invalid team information, Please try again\n");
                                continue;
                            }
                        }
                        personArr[temp.manager].manageTeam = next_team;
                        temp.index=next_team;
                        write(fd[0][1], team2str('G',temp,buf), strlen(buf));

                        while((buf[0]-0)==0){
                            read(fda[0][0],buf,100);
                        }
                        strcpy(buf,"");
                        teamArr[next_team] = temp;
                        printf("Team \"%s\" for project \"%s\" is created\n",temp.name,temp.project);
                    }
                    break;
                case 2 :
                    strcpy(user_input_buf, "");
                    event temp;
                    while ((user_input_buf[0] - '0') != 0) {
                        printf("please input information of the meeting in the format :\n\"Team_X yyyy-mm-dd hh:mm x\"\n");
                        printf("Enter > ");
                        gets(user_input_buf);
                        printf("Input is: %s\n", user_input_buf);
                        if (strncmp(user_input_buf, "Team_", 5) == 0) {
                            temp = str2event(user_input_buf, teamArr);
                            if (temp.holdDay != -1) {
                                temp.index=next_meeting;
                                eventArr[next_meeting] = temp;
                                write(fd[0], event2str('E',temp,buf), strlen(buf));
                                next_meeting++;
                                print_event(temp, personArr);
                            } else {
                                printf("Invalid input, please try again.\n");
                            }
                            continue;
                        }
                        FILE *f = fopen(user_input_buf, "r");
                        if (f == NULL) {
                            printf("There is no such file. Please try again.\n");
                            continue;
                        }
                        while (!feof(f)) {
                            fgets(user_input_buf, 100, f);
                            if (strncmp(user_input_buf, "Team_", 5) == 0) {
                                temp = str2event(user_input_buf, teamArr);
                                if (temp.holdDay != -1) {
                                    eventArr[next_meeting] = temp;
                                    next_meeting++;
                                    print_event(temp, personArr);
                                }
                            }
                        }
                    }
                case 3 :
                    gets(user_input_buf);
                    //                对用户输入进行转义，之后进行输出
                    //                do part 3
                    if (strncmp(user_input_buf, "FCFS", 4) == 0) {

                    } else if (strncmp(user_input_buf, "XXXX", 4) == 0) {

                    } else if (strncmp(user_input_buf, "YYYY", 4) == 0) {

                    } else {

                    }
                    printf("%s", user_input_buf);
                    break;
                case 4:
                    write(fd[0][1],"F",strlen("F"));
                    wait(NULL);
                    break;
                default:
                    printf("Invalid input, please try again.\n=========================================\n\n");
            }
            part_num = menu();
        }
        printf("\n~~~~~Thank you, wish you have a nice day!~~~~~\n");

    }
    return 0;
}