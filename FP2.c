#include "child.h"
#include "util.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define SCHEDULE 'P'
#define END 'F'

// extern event eventArr[200];
// extern person personArr[9];
// extern team teamArr[6];

event eventArr[200];
person personArr[9];
team teamArr[6];

// sort by priority
// sort by endtime
// sort by starttime

int eventCnt; // event number
int sortedEventArr[200];  // storing the index?

// below adopting the insertion sort (most efficient)
// starting from smallest to largest
// not in-place

void sortEventArr() {
}

// int scheduled[200]={0};
// int schedule(int eventID) {
	
// }
//int unhandled[2][200],tot[2]=0;
// void scheduleAll() {
// 	int i;
// 	sortEventArr();
// 	for(int i=0;i<eventCnt;++i) unhandled[0][tot[0]++]=i;
// 	int dr=0;
// 	while(1) {
// 		const int L0=10;
// 		int curTeam=-1,curHours=0;
// 		int moreHandled=0;
// 		for(i=0;i<tot[dr];++i) {
// 			int now=unhandled[dr][i];
// 			if(eventArr[now].teamID!=curTeam) {
// 				curTeam=eventArr[now].teamID;
// 				curHours=0;
// 			}
// 			if(curHours+eventArr[now].dutation>L0) continue;
// 			if(schedule(now)) {
// 				curHours+=eventArr[now].dutation;
// 				moreHandled=1;
// 			}
// 			else {
// 				unhandled[dr^1][tot[dr^1]++]=now;
// 			}
// 		}
// 		dr^=1;
// 		if(!moreHandled) break;
// 	}
// 	for(i=0;i<tot[dr];++i) {
// 		int now=unhandled[dt][i];
// 		if(!schedule(now)) {
// 			unhandled[dr^1][tot[dr^1]++]=unhandled[now];
// 		}
// 	}
// 	dr^=1;
// 	for(i=0;i<tot[dr];++i) {
// 		int now=unhandled[dt][i];
// 		reschedule(now);
// 	}
// }
int main() {	
	// for testing
	event eventArr[200]={
		    {0, 0, 1, 9, 10, '0','0'}, // 5
			{1, 0, 5, 12, 13, '0','0'},// 7
			{2, 0, 1, 12, 13, '0','0'},// 6
			{3, 1, 2, 13, 18, '0','0'},// 9
			{4, 1, 2, 10, 12, '0','0'},// 8
			{5, 2, 1, 13, 14, '0','0'}, // 1
			{6, 2, 3, 10, 15,'0','0'}, // 2
			{7, 3, 4, 9, 10, '0','0'},  // 3
			{8, 3, 4, 12, 13, '0','0'}, // 5
			{9, 3, 4, 11, 13, '0','0'}, // 4
			// 5 6 7 9 8 0 2 1 4 3
	};
    team teamArr[6]={
			{0,"team1","project1", 3, 0, {}},
			{1,"team2","project2", 4, 0, {}},
			{2,"team3","project3", 1, 0, {}},
			{3,"team4","project4", 2, 0, {}},
			{4,"team5","project5", 8, 0, {}},
			{5,"team6","project6", 5, 0, {}},
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

	eventCnt=10;
	//sortEventArr();
	// start
	event storage[200];
	int s;
	for(s=0; s<eventCnt; s++){
		storage[s]=eventArr[s];
	}

	int i, j;
	for(i=1; i<eventCnt; i++){
		event new_event = storage[i];
		char key = (personArr[teamArr[new_event.teamID].manager].name)[0];
		for (j=i-1; j>=0; j--) {
			    event cur = storage[j];
			    char cmp = (personArr[teamArr[cur.teamID].manager].name)[0];
                if (cmp < key) {
					break;  
				}else if(cmp==key){
					// same priority
					int base1 = new_event.holdDay*100;
					int base2 = cur.holdDay*100;
					if(base1+new_event.endTime>base2+cur.endTime){
						break;
					}else if(base1+new_event.endTime==base2+cur.endTime){
						// same priority and end time
						if(base1+new_event.startTime>=base2+cur.startTime){
							// same priority and end time and start time
							break;
						}else{
							storage[j+1]=storage[j];
						}
					}else{
						storage[j+1]=storage[j];
					}
				}
				else{
					storage[j+1]=storage[j];
				}
        }
        storage[j+1] = new_event;
	}

	for(s=0; s<eventCnt; s++){
		sortedEventArr[s]=storage[s].index;
	}
	// end


	for(i=0; i<eventCnt; i++){
		printf("%d ", sortedEventArr[i]);
	}
	printf("\n");

}