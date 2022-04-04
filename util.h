// 类型编号 1
typedef struct typePerson{
  int index;
  char name[11];
 
  int manageTeam; // 管理的团队的团队的id 无则 = -1
  int asMember;   // 作为几个团队的member？ 限制最多是3个
  // 一个人最多参与 3 个团队，即 1 <= asManager + asMember <= 3
}person;

// 类型编号 2
typedef struct typeTeam{
  int index;
  char name[11];

  int manager; 	// 从职员库中索引定位
  int memberCount; // how many member
  int member[3];

}team;

// 类型编号 3
typedef struct typeEvent{
  int index;

  int teamID;
  int manager; 	// 从职员库中索引定位
  int memberCount; // how many member
  int member[3];

  int holdDay; 		// 0-17
  int startTime;  // 9-17
  int endTime;		// 10-18

  int priority;   // FCFS 中可无视
  char whichTeam[11];

}event;

extern event eventArr[200];
extern person personArr[10];
extern team teamArr[256];

enum {BUF = 255};

