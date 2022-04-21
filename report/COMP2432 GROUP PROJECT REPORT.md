# Turbo-Journey

## 1. Introduction

Organizing meetings for multiple people in an organization like a business or a school is often difficult. Perhaps others are busy with other events. This Project contains two algorithms that try to solve the problem of organizing corporate meeting schedules.

This project PolyStar Interior Design Company provides a good training of implementation of a real system used in medium-size or small-size entrepreneurs. Our design of PMS (referred to Turbo-Journey in the following), is a project meeting scheduler, to schedule meetings for different teams and clients, mostly under fixed teams’ & clients’ scenarios. 

In this course, we’re required to understand the concepts and theories of operating system, and its different services and implementations. As Silberschatz et. Al (2018) state, operating system acts as intermediary between the user of a computer and the computer hardware, providing the environment in which a user can execute programs in a convenient and efficient manner. Since this project involved many concepts in OS, such as system calls, process communication, CPU scheduling, we, group06, have gain skills and experience in many aspects as required in the course:

Professional knowledge such as:

- Identifying services of different OS;
- Identifying internal structures of OS and write programs using system calls;
- Understanding concepts of process control, communication, memory management, deadlock and synchronization;
- All-roundness:
  - Developing skills in problem solving;
  - Solving problems in groups and gain collaboration & communication skills;

Finally, using two scheduling algorithms, referring to the First Come First Server and self-design best-performance algorithm, our PMS can satisfy the following requirements:

- All the six parts and bonus have been implemented
- Modular design
- Enhanced flexibility and maintainability
- The self-design algorithm approaches to reach the overall best performance under research and testing


## 2. Scope about OS

### 2.1. I/O for Unix systems. 

In this project, we need to read in/out the files from different files. In addition to using the I/O functions that come with the C library, we also called the I/O interface of the Unix system to input and output by changing the file descriptor. 

### 2.2. The use of multiple processes.

As can be seen from part (4), our program structure is divided into a total of 34 subroutines to carry out. Among them, we make extensive use of functions such as wait/fork in order to synchronize. This allows several children to carry out their work synchronously, increasing the efficiency of the system and the isolation of the data.

### 2.3. The use of pipes

To enable the exchange of information between multiple processes, we use the pipe system call in our project. Since our project has a total of 19 processes running simultaneously, many of them need to communicate with each other. We have imposed a series of restrictions on what we can communicate. For example, we created two pipes between each of the two processes that needed to communicate. This makes the flow of the pipeline always unidirectional and avoids multiple writes. Also, we specify that each command requires back and forth authentication, ensuring atomicity of each operation. To make the instructions easy to identify, we provide a specific format for each instruction. The author details the specific syntax of the intra-pipeline communication statements in Section 5.

### 2.4 Process synchronization

In order to make the processes synchronizable, we have taken a number of countermeasures. Our processes are divided into three layers, and each layer consists of First, our processes (except the main process) are written in a task-driven way. That is, the process operations are controlled with external instructions (i.e., what is passed in within the pipe). This ensures that processes can behave the way we want them to. Also, we design acknowledge commands for each instruction, and thanks to the blocking mechanism of the read() function, we can guarantee the atomicity of each command. It is also guaranteed that the necessary information can be returned to the call.

## 3. Program structure

### 3.1. Process

The program has a total of 19 processes, including one FF-Process (Parent of F-Process) and two F-Process (Parent of C-Process) for meeting arrangements, each of which has 8 C-Process (Child-Process) representing 8 staffs.

- FF Process: Parent Parent process
  - The FF process acts as an orchestrator.
- Two parent processes do two different algorithms. Also as server side.
  - F1 Process: Parent process for FCFS, child of FF
  - F2 Process: Parent Process for Customized algorithm, child of FF
- Cx Process: Child Process C1 to C8, child of F1 and F2

<img src="https://pic.hanjiaming.com.cn/2022/04/18/093727c7a6245.png" alt="1650297321479.png" style="zoom: 25%;" />

The program consists of three-level processes (FF-Process, F-process, C-Process), each consisting of two files, a C header file, which stores the declarations of functions and global variables, and a C source file, which stores the definitions and implementation of functions and global variables. The structure of such a program follows the C programming specification. Such a specification reduces code duplication, prevents duplicate variable definitions, enables programmers to locate code quickly and increases the efficiency of the team when writing programs.

### 3.2. Programming Model

-  Server-Client model
  - The parent process acts as Server, as the active party
  - The child process acts as the Client, as the passive party
- Event-driven programming
- State Machine Model

### 3.3. Pipeline

There are two major inter-process communication mechanisms: Shared memory and Message passing. Since sharing memory within the same computer with several CPU’s is not always straightforward. We use Message passing to enable inter-process communication.

Turbo-Journey uses pipelines for communication among process. Each pipeline is unidirectional, so there are two pipes between two processes for sending messages in different directions. FF-Process and each F-process has two pipe for inter-process communication, F-Process and each C-Process has two pipe for inter-process communication as well. Therefor total 36 pipes are used.

<img src="https://pic.hanjiaming.com.cn/2022/04/19/fa758c08586b2.png" alt="1650297870137.png" style="zoom:50%;" />

### 3.4. How to work

<img src="https://pic.hanjiaming.com.cn/2022/04/19/1a751c07110ba.png" alt="1650349547182.png" style="zoom:50%;" />

## 4. Communication Standards

To guarantee efficient inter-process communication amongst this number of pipes, and this number of processes, a messaging specification has been defined.

- Transfer method: char [101] via pipe.
- Separate blocks with `$ `in the buf of the string.

For example, the sheet below is communication standard of F2 -> Cx

| signal | arg1       | Meaning                                                      |
| ------ | ---------- | ------------------------------------------------------------ |
| T      | an event   | try to add event                                             |
| A      | /          | confirm add event A                                          |
| C      | /          | cancel event A                                               |
| P      | fd, d1, d2 | Tell child to print in file description fd, from date 1 to date 2 |
| I      | a person   | Tell child the information itself                            |
| R      | d1, d2     | Ask child how many meetings there are between A and B        |
| F      | /          | end process                                                  |

## FCFS algorithm

Turbo-Journey's first algorithm is very similar to the Process scheduling algorithm FCFS. It is shown in the figure below.

<img src="https://pic.hanjiaming.com.cn/2022/04/19/8c00fdbeb6b7a.png" alt="1650348459447.png" style="zoom:50%;" />

## Customized algorithm

<img src="https://pic.hanjiaming.com.cn/2022/04/19/7c9f0f1386c78.png" alt="1650367298808.png" style="zoom: 33%;" />
