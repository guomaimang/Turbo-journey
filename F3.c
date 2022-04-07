#include "child.h"
#include "util.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

// 待合并
// listen the the C1-C8
// receive :
// 1. received requests
// 2. accepted requests (done)
// 3. accepted time slots

// calculate:
// 1. total received requests
// 2. total accepted/rejected requests
// 3. team time ultilization
// 4. staff time ultilization
// 5. staff attendance rate

// event eventArr[200];
// person personArr[9];
// team teamArr[6];

//event finalEventArr[200]; // this is the final event arrangment
int finalEventCnt = 9; // the final event number

    // for testing
    event finalEventArr[200]={
        	{5, 2, 1, 13, 14, '0','0'}, // 1
            {6, 2, 3, 10, 15,'0','0'}, // 5
            {7, 3, 4, 9, 10, '0','0'},  // 1
            {9, 3, 4, 11, 13, '0','0'}, // 2
		    {0, 0, 1, 9, 10, '0','0'}, // 1
            {2, 0, 1, 12, 13, '0','0'},// 1
			{1, 0, 5, 12, 13, '0','0'},// 1
            {4, 1, 2, 10, 12, '0','0'},// 2
			{3, 1, 2, 13, 18, '0','0'},// 5
	};
    team teamArr[6]={
			{0,"team1","project1", 0, 3, {0,1,2}}, // 3
			{1,"team2","project2", 1, 3, {0,1,2}}, // 7
			{2,"team3","project3", 2, 3, {0,1,2}}, // 6
			{3,"team4","project4", 3, 2, {3,4}}, // 3
			{4,"team5","project5", 4, 2, {3,4}},
			{5,"team6","project6", 6, 3, {1,6,7}}, // 5 teams?
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

int receivedRequests[8] = {10, 10, 10, 10, 10, 10, 10, 10}; // input 80
// int acceptedRequests[8] = {3, 3, 2, 2, 2, 0, 0, 0}; // input 12
int acceptedRequests[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // input 12
int rejectedRequests[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
int totalReceivedRequests = 0;
int totalAcceptedRequests = 0;
int totalRejectedRequests = 0;

int teamTime[5] = {0, 0, 0, 0, 0}; 
// int staffTime[8] = {16, 16, 16, 3, 3, 0, 0, 0}; // input 95
int staffTime[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // input 95
float teamTimeUtil[5] = {0, 0, 0, 0, 0};
float staffTimeUtil[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float staffAttendanceRate[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int startDay = 0;
int endDay = 17;
// need to write to the child
// generating report
// *****THIS IS BONUS 3C*****

void printAttendanceReport(){
    printf("Performance: \n\n");

    // 1. total received requests
    // 2. total accepted/rejected requests
    int i;
    for(i=0; i<8; i++){
        totalReceivedRequests=totalReceivedRequests+receivedRequests[i];
        totalAcceptedRequests=totalAcceptedRequests+acceptedRequests[i];
        rejectedRequests[i] = receivedRequests[i] - acceptedRequests[i];
        totalRejectedRequests=totalRejectedRequests+rejectedRequests[i];
    }

    printf("Total Number of Requests Received: %d (99.9%%)\n", totalReceivedRequests);
    printf("      Number of Requests Accepted: %d (%.1f%%)\n", totalAcceptedRequests, (float)totalAcceptedRequests/(float)totalReceivedRequests*100);
    printf("      Number of Requests Rejected: %d (%.1f%%)\n", totalRejectedRequests, (float)totalRejectedRequests/(float)totalReceivedRequests*100);

    printf("\nUtilization of Time Slot:\n\n");
    printf("\tAccepted request\t - %.1f%%\n", (float)totalAcceptedRequests/(float)totalReceivedRequests*100);
    // 3. team time ultilization
    for(i=0; i<5; i++){
        // get the team time
        int j;
        for(j=0; j<finalEventCnt; j++){
            if(finalEventArr[j].teamID==i){
                teamTime[i] = teamTime[i]+ finalEventArr[j].endTime - finalEventArr[j].startTime;
            }
        }
        teamTimeUtil[i] = (float)teamTime[i]/(float)((endDay-startDay+1)*9)*100;
        printf("\t%s\t\t - %.1f%%\n", teamArr[i].name, teamTimeUtil[i]);  // 名字的设计！
    }

    // 4. staff time ultilization
    for(i=0; i<8; i++){
        staffTimeUtil[i] = (float)staffTime[i]/(float)((endDay-startDay+1)*9)*100;
        printf("\t%s\t\t - %.1f%%\n", personArr[i].name, staffTimeUtil[i]);
    }

    printf("\nAttendance:\n\n");
    // 5. staff attendance rate
    for(i=0; i<8; i++){
        staffAttendanceRate[i] = (float)acceptedRequests[i]/(float)receivedRequests[i];
        printf("\t%s\t\t - %.1f%%\n", personArr[i].name, staffAttendanceRate[i]*100);
    }
}


// *****THIS IS BONUS 2C*****
// purpose: used to analyze a specified period for a team is available
// accepted: 
// rejected:
// providing the required time meeting information

int checkTeamID = 2;
int checkDay = 3;
int checkStartTime = 9;
int checkEndTime = 15;

// duplicated team member 
// 0 represents no conflict
// 1 represents conflict
int checkTeam(team t1, team t2){
    int i;
    for(i=0; i<t1.memberCount; i++){
        int j;
        for(j=0; j<t2.memberCount; j++){
            if(t1.member[i]==t2.member[j]){
                return 1;
            }
        }
    }
    return 0;
}
void ifAvailable(){
    // in the final event array
    // 1. accept/reject the request
    int available = 1;
    int i;
    for(i=0; i<finalEventCnt; i++){
        if(finalEventArr[i].holdDay==checkDay){
            // the time is overlapping
            if((finalEventArr[i].startTime <= checkStartTime && checkStartTime <= finalEventArr[i].endTime)||(finalEventArr[i].startTime <= checkEndTime && checkEndTime <= finalEventArr[i].endTime)){
                if(checkTeam(teamArr[checkTeamID], teamArr[finalEventArr[i].teamID])==1){
                    available = 0;
                }
            }
        }
    }

    if(available){
        printf("*** Meeting Request – ACCEPTED ***\n\n");
    }else{
        printf("*** Meeting Request – REJECTED ***\n\n");
    }

    // print the information
    printf("The scheduled requests for the required period:\n");
    printf("\n===========================================================================\n");
    int satisfitedEventNum = 0;
    for(i=0; i<finalEventCnt; i++){
        if(finalEventArr[i].holdDay==checkDay){
            // the time is overlapping
            if((finalEventArr[i].startTime <= checkStartTime && checkStartTime <= finalEventArr[i].endTime)||(finalEventArr[i].startTime <= checkEndTime && checkEndTime <= finalEventArr[i].endTime)){
                satisfitedEventNum++;
                if(finalEventArr[i].holdDay>=0&&finalEventArr[i].holdDay<=5){
                    printf("%d. %s 2022-04-%2d %2d:00 %d\n", satisfitedEventNum, teamArr[finalEventArr[i].teamID].name, 25+finalEventArr[i].holdDay, finalEventArr[i].startTime, finalEventArr[i].endTime-finalEventArr[i].startTime);
                }else if(finalEventArr[i].holdDay>=6&&finalEventArr[i].holdDay<=11){
                    printf("%d. %s 2022-05-%2d %2d:00 %d\n", satisfitedEventNum, teamArr[finalEventArr[i].teamID].name, finalEventArr[i].holdDay-4, finalEventArr[i].startTime, finalEventArr[i].endTime-finalEventArr[i].startTime);
                }else{
                    printf("%d. %s 2022-05-%2d %2d:00 %d\n", satisfitedEventNum, teamArr[finalEventArr[i].teamID].name, finalEventArr[i].holdDay-3, finalEventArr[i].startTime, finalEventArr[i].endTime-finalEventArr[i].startTime);
                }
            }
        }
    }
    printf("===========================================================================\n");
}

int main(){
    // this is to initialize the array accepted requests
    // and this is to initialize the array staff time

    int i;
    for(i=0; i<8; i++){
        int j;
        for(j=0; j<finalEventCnt; j++){
            int z;
            int status = 0;
            for(z=0; z<teamArr[finalEventArr[j].teamID].memberCount; z++){
                if(((teamArr[finalEventArr[j].teamID].member)[z])==i){
                    if(status==0){
                        acceptedRequests[i]++;
                        status++;
                    }
                    staffTime[i] = staffTime[i] + finalEventArr[j].endTime - finalEventArr[j].startTime;
                }
            }
        }
    }

    // for(i=0; i<8; i++){
    //     printf("accepted requests: %d\n", acceptedRequests[i]);
    // }
    // for(i=0; i<8; i++){
    //     printf("time: %d\n", staffTime[i]);
    // }
    //printAttendanceReport();
    ifAvailable();
    return 0;
}