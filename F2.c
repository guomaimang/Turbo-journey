#include "child.h"
#include "util.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<unistd.h>
#include<sys/wait.h>

#define SCHEDULE 'P'
#define END 'F'
#define ADDEVENT 'T'
#define CONFIRM 'A'
#define CANCEL 'C'

/* event eventArr[200]={
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
}; */

event eventArr[200];
team teamArr[6];
person personArr[9];

int fd[10][2][2];

int eventCnt; // event number
int sortedEventArr[200];  // storing the index

// sort by priority
// sort by endtime
// sort by starttime
// below adopting the insertion sort (most efficient)
// starting from smallest to largest
// not in-place

void sortEventArr() {
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
				}else{
					storage[j+1]=storage[j];
				}
        }
        storage[j+1] = new_event;
	}

	for(s=0; s<eventCnt; s++){
		sortedEventArr[s]=storage[s].index;
	}
}

// int scheduled[200]={0};
#define WRITE(i) write(fd[i][0][1],send,strlen(send))
#define READ(i) read(fd[i][1][0],rcv,80)
int schedule(int eventID) {
	int teamID=eventArr[eventID].teamID;
	int i;
	char send[80]={0},rcv[10];
	event2str(ADDEVENT,eventArr[eventID],send);
	int flag=1;
	//manager as a member
	for(i=0;i<teamArr[teamID].memberCount;++i) {
		WRITE(i);
		READ(i);
		if(rcv[0]=='N') {
			flag=0;
			break;
		} 
	}
	if(flag) send[0]=CONFIRM;
	else send[0]=CANCEL;
	send[1]='$';
	send[2]=0;
	for(i=0;i<teamArr[teamID].memberCount;++i) {
		WRITE(i);
		READ(i);
	}
	return flag;
}
int unhandled[2][200],tot[2]={0};
void scheduleAll() {
	int i;
	sortEventArr();
	for(int i=0;i<eventCnt;++i) unhandled[0][tot[0]++]=i;
	int dr=0;
	while(1) {
		const int L0=10;
		int curTeam=-1,curHours=0;
		int moreHandled=0;
		for(i=0;i<tot[dr];++i) {
			int now=unhandled[dr][i];
			if(eventArr[now].teamID!=curTeam) {
				curTeam=eventArr[now].teamID;
				curHours=0;
			}
			if(curHours+eventArr[now].endTime-eventArr[now].startTime>L0) continue;
			if(schedule(now)) {
				curHours+=eventArr[now].endTime-eventArr[now].startTime;
				moreHandled=1;
			}
			else {
				unhandled[dr^1][tot[dr^1]++]=now;
			}
		}
		dr^=1;
		if(!moreHandled) break;
	}
	for(i=0;i<tot[dr];++i) {
		int now=unhandled[dr][i];
		if(!schedule(now)) {
			unhandled[dr^1][tot[dr^1]++]=unhandled[now][dr];
		}
	}
	dr^=1;
	for(i=0;i<tot[dr];++i) {
		int now=unhandled[dr][i];
		reschedule(now);
	}
}

void print(int beginDate,int endDate) {
	FILE *out;
	out=fopen("Schedule_MINE.txt","w");
	fputs("*** Project Meeting ***",out);
	fputs("",out);
	fputs("Algorithm used: MINE",out);
	fprintf(out,"PeriodL %s to %s\n",toDate[beginDate],toDate[endDate]);
	fputs("",out);
	fputs("Date          Start   End     Team     Project\n",out);
	fputs("===========================================================================",out);
	int i;
	for(i=0;i<eventCnt;++i) {
		event* now=&eventArr[i];
		fprintf(out,"%13s %7s %7s %8s %s",toDate[now->holdDay],toTime[now->startTime-9],toTime[now->endTime-9],teamArr[now->teamID].name,now->project);
	}
	char send[80],rcv[10];
	sprintf(send,"P$%d$%d$%d",out->_fileno,beginDate,endDate);
	for(i=1;i<8;++i) {
		WRITE(i);
		READ(i);
	}
}
#undef WRITE
#undef READ
event ins2event(char ins[]) {
	event ret;
	char* token=strtok(ins,"$");
	ret.index=atoi(token);
	token=strtok(NULL,"$");
	ret.teamID=atoi(token);
	token=strtok(NULL,"$");
	ret.holdDay=atoi(token);
	token=strtok(NULL,"$");
	ret.startTime=atoi(token);
	token=strtok(NULL,"$");
	ret.endTime=atoi(token);
	token=strtok(NULL,"$");
	strcpy(ret.name,token);
	token=strtok(NULL,"$");
	strcpy(ret.project,token);
	while(token!=NULL) token=strtok(NULL,"$");
	return ret;
}
team ins2team(char ins[]) {
	team ret;
	char* token=strtok(ins,"$");
	token=strtok(NULL,"$");
	ret.index=atoi(token);
	token=strtok(NULL,"$");
	strcpy(ret.name,token);
	token=strtok(NULL,"$");
	strcpy(ret.project,token);
	token=strtok(NULL,"$");
	ret.manager=atoi(token);
	token=strtok(NULL,"$");
	ret.memberCount=atoi(token);
	token=strtok(NULL,"$");
	int i;
	for(i=0;token!=NULL;++i) {
		ret.member[i]=atoi(token);
		token=strtok(NULL,"$");
	}
	return ret;
}


int F2main(const int ff2f[],const int f2ff[]) {
	//open pipe & fork
	int i,retpid;
	for(i=0;i<8;++i) {
		if(pipe(fd[i][0])<0 || pipe(fd[i][1])<0) {
			exit(1);
		}
	}
	for(i=0;i<8;++i) {
		retpid=fork();
		if(retpid==0) {
			int childID=i;
            childInput input = (childInput){fd[i][0], fd[i][1]};
            FCFSChild(&input);
			break;
		}
	}
#define WRITE write(f2ff[1],send1,strlen(send1))
#define READ read(ff2f[0],rcv1,80)
	//for parent
	char rcv1[80],send1[]="O";
	while(1) {
		READ;
		switch(rcv1[0]) {
			case 'F': //end
				WRITE;
				return 0;
			case 'E': {//event
				event tmp=ins2event(rcv1);
				eventArr[tmp.index]=tmp;
				WRITE;
				break;
			}
			case 'G': {//group
				team tmp=ins2team(rcv1);
				teamArr[tmp.index]=tmp;
				WRITE;
				break;
			}
			case 'P': {//print
				char* token=strtok(rcv1,"$");
				token=strtok(NULL,"$");
				int beginDate=atoi(token);
				token=strtok(NULL,"$");
				int endDate=atoi(token);
				while(token!=NULL) token=strtok(NULL,"$");
				scheduleAll();
				print(beginDate,endDate);
				WRITE;
				break;
			}
		}
	}
#undef WRITE
#undef READ
	return 0;
}
