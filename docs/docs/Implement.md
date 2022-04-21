---
article: false
date: 2022-03-31
index: 3


---

# Implement & Paradigm 

## 前言

当你遇到任何你无法解决的问题，你应当立刻上网查找资料，并和组员进行讨论。你不能不负责任的完成任务以不负责任的依赖项。事实上，我们应当对我们不擅长的地方和组员进行讨论，说不定你会发现新大陆！当然，如果有一些基础问题，比如Github之类的东西不会用，告诉大家，大家都会乐其去帮助你！

就本文档而言，如果你遇到有异议的地方，请立刻联系我 by WeChat or Email. 联系人：韩佳明

## 程序模型

此 Project 需要用到以下模型

### Server-Client 模型

- 父进程作为Server，作为主动方
- 子进程作为Client，作为被动方

### 事务驱动编程

在执行任务时，程序一直处于监听状态，即一直在 `While(1) ` 这样的循环中，除非收到通知道他们该做什么。

### 状态机模型

这个应该在 COMP 2421 中学习过，在Networking中也有涉及。

## 进程视图

- 父父进程作为统筹器。
- 两个父进程做两种不同的算法。也是作为服务端。
- 每个子进程代表一个用户，也是一个客户端。

<img src="https://pic.hanjiaming.com.cn/2022/03/30/a2abbddd96d6a.png" alt="1648652744118.png" style="zoom: 50%;" />

## 模型视图

以 父进程（算法2）为例子

```c
// 程序启动后，父父进程立刻创建 父进程1

// 父父进程 立刻为 5 个客户分别创建子进程。
if(...){
  fork();
}

// 适用于算法1 的父进程进入监听状态
while (1){
  get FFmessgae;
  if (FFmessage length == 0){
    continue;
  }
  
  // add a new event
  if (FFmessage...){
    
    // ask every child if this event can be add
    for(int i=0;i<childnumber;i++){
      something();
      // wait message from child
    	while(1){
        get Cmessage;
        something();
      }  
    }
    
    // if can't
    something();
    
   	// if all can, do add
    for(int i=0;i<childnumber;i++){
      something();
      // wait message from child
    	while(1){
        get Cmessage;
        something();
      }  
    }
  }
  
  // print calendar
  if (FFmessage...){
    for(int i=0;i<childnumber;i++){
      something();
    }
  }
  
  // ...
  ...
}
```

## 流程说明

- 程序进入后只有FF
- FF创建F1和F2
  - do something
  - 创建C1 - C8 
    - do something
  - 结束C1 - C8
- 结束FF1
- 结束FF2

