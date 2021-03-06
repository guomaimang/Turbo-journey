#include "child.h"
#include "util.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<unistd.h>
#include<sys/wait.h>
#include "F3.h"
#include <fcntl.h>
#include <sys/stat.h>

/* #define SCHEDULE 'P'
#define END 'F'
#define ADDEVENT 'T'
#define CONFIRM 'A'
#define CANCEL 'C' */

int f2c[8][2],c2f[8][2];

int head,tail; // event number
int sortedEventArr[eventSize],eventCnt;

// sort by priority
// sort by endtime
// sort by starttime
// below adopting the insertion sort (most efficient)
// starting from smallest to largest
// not in-place

event storage[eventSize];
void sortEventArr() {
	int i, j;
	eventCnt=0;
	for(i=head; i<tail; ++i){
		storage[eventCnt++]=eventArr[i];
		printf("%s\n",eventArr[i].name);
	}

	//printf("%d %d\n",eventCnt,eventArr[0].index);
	//for(i=0;i<eventCnt;++i) printEvent(&eventArr[i]);
	//puts("");
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

	for(i=0; i<eventCnt; i++){
		sortedEventArr[i]=storage[i].index;
	}
}
event finalEvents[eventSize];
int finalCnt;
// int scheduled[200]={0};
#define WRITEC(i) write(f2c[i][1],send,strlen(send))
#define READC(i) read(c2f[i][0],rcv,80)
int schedule(int eventID) {
	int flag=trySchedule(&eventArr[eventID],f2c,c2f);
	if(flag) {
		event* now=&eventArr[eventID];
		myCalendar[now->holdDay][now->startTime-9]=eventID;
		finalEvents[finalCnt++]=*now;
	}
	return flag;
	
	/* int teamID=eventArr[eventID].teamID;
	int i;
	char send[80]={0},rcv[10];
	event2str(ADDEVENT,&eventArr[eventID],send);
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
	return flag; */
}
int unhandled[2][eventSize],tot[2];
int rejectCnt,rejectedArr[eventSize];
void scheduleAll() {
	tot[0]=tot[1]=0;
	int i;
	sortEventArr();
	//printf("eventCnt=%d\n",eventCnt);
	//for(i=0;i<eventCnt;++i) printf("%d ",sortedEventArr[i]);
	//puts("");
	for(i=0;i<eventCnt;++i) unhandled[0][tot[0]++]=sortedEventArr[i];
	int dr=0;
	while(1) {
		for(i=0;i<tot[dr];++i) printf("%d ",unhandled[dr][i]);puts("");
		const int L0=10;
		int curTeam=-1,curHours=0;
		int moreHandled=0;
		tot[dr^1]=0;
		for(i=0;i<tot[dr];++i) {
			int now=unhandled[dr][i];
			if(eventArr[now].teamID!=curTeam) {
				curTeam=eventArr[now].teamID;
				curHours=0;
			}
			if(curHours+eventArr[now].endTime-eventArr[now].startTime>L0) {
				unhandled[dr^1][tot[dr^1]++]=now;
				continue;
			}
			if(schedule(now)) {
				curHours+=eventArr[now].endTime-eventArr[now].startTime;
				moreHandled=1;
			}
			else {
				//printf("unhandle: %d\n",now);
				unhandled[dr^1][tot[dr^1]++]=now;
			}
		}
		dr^=1;
		//printf("more?%d\n",moreHandled);
		if(!moreHandled) break;
	}
	tot[dr^1]=0;
	for(i=0;i<tot[dr];++i) {
		int now=unhandled[dr][i];
		if(!schedule(now)) {
			unhandled[dr^1][tot[dr^1]++]=now;
		}
	}
	dr^=1;
	tot[dr^1]=0;
	for(i=0;i<tot[dr];++i) {
		int now=unhandled[dr][i];
		if(reschedule(&eventArr[now],f2c,c2f)) {
			myCalendar[eventArr[now].holdDay][eventArr[now].startTime-9]=now;
			finalEvents[finalCnt++]=eventArr[now];
		}
		else {
			rejectedArr[rejectCnt++]=now;
		}
	}
	puts("F2: schedule done!");
}
int printCnt;
void print(int beginDate,int endDate) {
	++printCnt;
	char fileName[50];
	sprintf(fileName,"Schedule_GARR_%02d.txt",printCnt);
	FILE *out;
	out=fopen(fileName,"w");
	fputs("*** Project Meeting ***\n\n",out);
	fputs("Algorithm used: GARR\n",out);
	fprintf(out,"PeriodL %s to %s\n\n",toDate[beginDate],toDate[endDate]);
	fputs("Date          Start   End     Team     Project\n",out);
	fputs("===========================================================================\n",out);
	int i;
	for(i=0;i<finalCnt;++i) {
		event* now=&finalEvents[i];
		if(now->holdDay<beginDate || now->holdDay>endDate) continue;
		fprintf(out,"%13s %7s %7s %8s %s\n",toDate[now->holdDay],toTime[now->startTime-9],toTime[now->endTime-9],teamArr[now->teamID].name,now->project);
		
	}
	// for(i=beginDate;i<=endDate;++i) {
	// for(j=0;j<9;++j) {
			// int id=myCalendar[i][j];
			// if(id==-1) continue;
			// event* now=&eventArr[id];
		// }
	// }
 	char send[80],rcv[100];
	sprintf(send,"P$%s$%d$%d",fileName,beginDate,endDate);
	fclose(out);
	for(i=0;i<8;++i) {
		WRITEC(i);
/* 		int le=READC(i);
		printf("C%d: %s\n",i,rcv); */
		
		while (1) {
			// wait ack from C
			//debug("watr ack from C...\n");
			int np = READC(i);
			//debug("get %s from %d\n", rcv, i);
			if (np <= 0) {
				continue;
			}
			if (rcv[0] == 'D') {
				break;
			}
		}
	}
	out=fopen(fileName,"a");
	fputs("===========================================================================\n",out);
	fputs("*** Meeting Request???REJECTED ***\n\n",out);
	fprintf(out,"There are %d requests rejected for the required period.\n\n",rejectCnt);
	for(i=0;i<rejectCnt;++i) {
		event* now=&eventArr[rejectedArr[i]];
		fprintf(out, "%d.\t%s %s %s %d\n", i+1,
                           now->name,
                           toDate[now->holdDay],
                           toTime[now->startTime-9],
                           now->endTime - now->startTime);
	}
	fclose(out);
	puts("F2: print done!");
}

event ins2event(char ins[]) {
	event ret;
	char* token=strtok(ins,"$");
	token=strtok(NULL,"$");
	token=strtok(NULL,"$");
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


int F2main(int ff2f[2][2],int f2ff[2][2]) {
	//puts("HHH");
	close(ff2f[1][1]);
	close(f2ff[1][0]);
	//open pipe
	int i,retpid;
	for(i=0;i<8;++i) {
		if(pipe(f2c[i])<0 || pipe(c2f[i])<0) {
			puts("Pipe Error!");
			exit(1);
		}
	}
	memset(myCalendar,0xff,sizeof myCalendar);
	//fork
	for(i=0;i<8;++i) {
		retpid=fork();
		if(retpid<0) {
			puts("F2: Fork fail!");
			exit(1);
		}
		else if(retpid==0) {
			//for children
			int childID=i;
            childInput input = (childInput){f2c[i], c2f[i], childID};
            Child(&input);
			exit(0);
		}
	}
#define WRITEFF write(f2ff[1][1],send1,strlen(send1))
#define READFF read(ff2f[1][0],rcv1,80)
	//for parent
	for(i=0;i<8;++i) {
		close(f2c[i][0]);
		close(c2f[i][1]);		
	}
	head=tail=0;
	rejectCnt=0;
	printCnt=0;
	finalCnt=0;
	char rcv1[100],send1[]="D";
	char rcv[100],send[80];
	while(1) {
		int np=READFF;
		if(np<=0) {
			puts("Error!");
			continue;
		}
		rcv1[np]=0;
		int sig=rcv1[0];
		//puts(rcv1);
		switch(sig) {
			case 'F': {//end
				send[0]='F';
				send[1]=0;
				for(i=0;i<8;++i) {
					WRITEC(i);
				}
				return 0;
			}
			case 'E': {//event
				event tmp=ins2event(rcv1);
				eventArr[tmp.index]=tmp;
				tail++;
				WRITEFF;
				break;
			}
			case 'G': {//group
				team tmp=ins2team(rcv1);
				teamArr[tmp.index]=tmp;
				WRITEFF;
				break;
			}
			case 'P': {//print
				char* token=strtok(rcv1,"$");
				token=strtok(NULL,"$");
				int beginDate=atoi(token);
				token=strtok(NULL,"$");
				int endDate=atoi(token);
				while(token!=NULL) token=strtok(NULL,"$");
				printf("tail: %d\n",tail);
				scheduleAll();
				head=tail;
				print(beginDate,endDate);
				WRITEFF;
				break;
			}
			case 'B': {
				int t, d, s, e;
				sscanf(rcv1, "B$%d$%d$%d$%d", &t, &d, &s, &e);
				int stdfd = dup(1);
				int wfd = open("G06_FCFS_Analysis_Report.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
				dup2(wfd, 1);
				ifAvailable(finalEvents, finalCnt, t, d, s, e);
				dup2(stdfd, 1);
				WRITEFF;
				break;
			}
			case 'R': {
				int d1, d2;
				sscanf(rcv1, "R$%d$%d", &d1, &d2);
				int receivedRequests[8] = {0};
				for(i=0; i<tail; ++i){
					int j;
					for(j = 0; j < teamArr[eventArr[i].teamID].memberCount; ++j){
						++receivedRequests[teamArr[eventArr[i].teamID].member[j]];
					}
				}
				int stdfd = dup(1);
				int wfd = open("G06_FCFS_Attendance_Report.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
				dup2(wfd, 1);
				printAttendanceReport(finalEvents, finalCnt, d1, d2, receivedRequests);
				dup2(stdfd, 1);
				WRITEFF;
				break;
			}
		}
	}
	return 0;
}
