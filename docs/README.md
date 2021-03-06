---
article: false
date: 2022-03-31
index: 0

---

# Dev Document

**COMP 2421 Group Project Development Document**

## 怎样查看

<img src="https://pic.hanjiaming.com.cn/2022/03/31/a01af642388f3.png" alt="1648703839436.png" style="zoom:25%;" />

可通过目录快速翻阅。
临时存放于此站点，完成Project之后将放到Projects所在仓库。

## 怎样更新

- wechat发起讨论
- 直接在页面下方留言处留言

------

## 情景模拟

**PolyStar室内设计公司**在市场上扮演着商业办公室室内设计和承包专家的角色。

有设计师/工作人员在不同的项目组工作，监督/协调一些室内设计和装修项目。

由于不同项目同时进行的发展需要，每个成员在整个期间只从事一个项目的工作是不现实的。换句话说，每个设计师/工作人员应该同时从事多个项目的工作。

每个项目有两个相关的角色：经理和成员。

每个项目都有一个且只有一个经理和几个成员。

- 经理负责项目，扮演领导者的角色。
- 每个设计师/工作人员可以是几个不同项目的经理或项目成员。

为了确保项目的质量，PolyStar不想让设计师或工作人员的工作量过大，并为他们的最大工作量提供了一个指导原则。

- 设计师或工作人员最多可以参与三个项目，但可以只做一个项目的经理。
  - 例如，工作人员_A可以是项目_X的项目经理。他/她不能成为另一个项目Project_Y的经理，但可以成为其他两个项目的成员。
  - 此外，每个项目组最多可以有四个工作人员（即一个项目经理+三个项目成员）。

项目经理通常会与他们的团队成员和/或客户开会。然而，他们中的大多数人都很忙，不能适当地安排/管理这些会议，也不能及时解决问题。这在项目的进展中造成了很多问题。其中一些问题无法及时解决，导致了公司的损失。

知道你在操作系统课程中学习了不同的调度算法，PolyStar的经理想为你提供一份自由职业的工作。**你是否介意帮助PolyStar创建一个 "项目会议调度器"（PMS），以便更好地利用人力资源，改进会议调度/重新安排的方法？我们的目标是实施一个更好的会议调度器，以减少项目进展中的问题，确保会议的及时安排和进行。**

##  Project Requirements

在这个项目中，你将有机会把你从COMP2432中学到的过程调度理论应用到一个日常生活场景中，并为PolyStar开发项目会议调度程序（PMS）来进行部署。

在这个自由职业者的工作中应该采用模块化设计，以提高灵活性和可维护性。一般来说，这个PMS系统应该设计成具有不同功能的模块。

**此外，这个项目应该以较小的步骤进行，由四部分组成，也许最多6部分。**

### Part1

**开发一个程序PMS**，

- 允许用户创建项目组（一个项目经理+最多三个成员）
  - 并将会议的细节（日期、时间、持续时间和/或召集人）添加到现有项目组的时间表中。
- 除了标准的逐行输入方法，PMS应该能够读入一个包含会议请求信息的批处理文件，即一个或多个会议请求存储在这样一个批处理文件中。

请注意，发起预订的人被称为 "用户 "或 "呼叫者"（通常是项目经理），参与会议的团队成员被称为 "被呼叫者"。这个程序被称为输入模块。

- 记住，该程序应允许批量输入，即读入文件进行输入。
- 建议将所有的会议请求存储在一个日志文件中，以备将来审计之用。

顺便说一下，该应用程序的开发是为了暂时在内部使用。如果结果令人满意，PolyStar将考虑向他们的客户推广使用该应用程序，以便与他们的客户有更好的沟通。

### Part2

**增强程序PMS，以检查与会者（团队成员）是否有空。**

- 如果是，它就在每个成员的时间段上分配一个预订。
- 如果不是，该请求将被拒绝。

例如，如果一个工作人员为一个会议安排了一个时间段，他/她将拒绝与预定时间段相冲突的其他会议请求。

但是，如果在你的算法中考虑了项目的优先级，那么优先级较高的项目可能会覆盖优先级较低的项目，并以重新安排的形式对优先级较低的会议安排进行修改，或者以取消的形式对优先级较低的会议安排进行拒绝。这后一种安排就像拒绝现有的，但接受新的。

当然，对于冲突的会议如何安排或处理，你可能有其他假设。

例如，你可以考虑将以前的/新的请求（以优先级较低者为准）推迟到所有成员都有空时的新时段。

你可以考虑在日程安排中加入一些更 "人性化 "的标准，例如，确保在午餐时间的会议之前或之后会有一些午餐时间，以及每天的会议总时数不超过某个限制，例如5小时。

### Part3

用一个调度器扩展第二部分开发的程序，为每个工作人员或项目组的所有会议生成一个会议时间表（例如，工作人员_A的会议时间表或团队_A的会议时间表）。

调度器可以实现几个像讲座中所涉及的调度算法。(在这个项目中，你被要求至少实现两个。其中一个应该是相对简单的 "先到先得 "算法）。这被称为调度内核，在调度模块中。

### Part4

在第二部分中为每个工作人员或项目组增加打印/导出会议时间表的功能。不仅包括已接受的请求，而且那些被拒绝或重新安排的请求也应包括在报告中。这就构成了**输出模块**。

### Part5

(*Bonus up to 2%*) 用你测试过的不同调度算法来分析这个程序。列出那些可以从你的程序中实现的优点和缺点。

### Part6

(*Bonus up to 8%*) 实施每个团队成员的考勤，并讨论这个报告的用途。例如，是否可以用这份报告来评估工作人员的表现，如何评估？

You may form a group of 3 - 5 persons for this project. The project must be implemented using programming **language C** and it must be successfully executed on **ANY ONE of Linux Servers** (*apollo or apollo2*) in the Department of Computing. 

We accept only the C-source code file(s). We simply compile your source code file(s) and then execute the compiled program. If you use any additional library files in your application, you should have ***sought the approval\*** from us first. Those additional library files should be included in your submission and declare clearly how they are applied in your application.
