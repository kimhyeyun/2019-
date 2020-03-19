/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #2
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */
#define			 N_TASKS           5                  /* Number of identical tasks      */

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStartStk[TASK_STK_SIZE];            /* Startup    task stack                         */
OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */

OS_EVENT* RandomSem;

OS_EVENT* mbox_to_random[4];						// decision task에서 random task로 보내는 mailbox
OS_EVENT* mbox_to_decision[4];						// random task에서 decision task로 보내는 mailbox

OS_EVENT* queue_to_random;							// decision task에서 random task로 보내는 queue
OS_EVENT* queue_to_decision;						// random task에서 decision task로 보내는 queue
void* decision_queue[4];								// decision task에서 random task로 보내는 queue의 배열
void* random_queue[4];									// random task에서 decision task로 보내는 queue의 배열

INT8U select;											// 사용자가 커맨드에 1이나 2를 입력
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  Task(void* data);
static  void  TaskStart(void* data);                  /* Function prototypes of tasks                  */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);


/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main(void)
{

	for (;;) {
		scanf("%d", &select);
		if (select == 1) {
			break;
		}
		else if (select == 2) {
			break;
		}
		else {
			continue;
		}
		//1을 입력하면 mailbox, 2를 입력하면 queue를 이용한다.
		//그 외의 숫자를 입력하면 재입력 받는다.
	}

	//여기랑
	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

	OSInit();                                              /* Initialize uC/OS-II                      */

	PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
	PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

	RandomSem = OSSemCreate(N_TASKS);
	OSTaskCreate(TaskStart, (void*)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);

	OSStart();                                             /* Start multitasking                       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

static void  TaskStart(void* pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
	OS_CPU_SR  cpu_sr;
#endif
	INT16S     key;
	INT8U i;

	pdata = pdata;                                         /* Prevent compiler warning                 */

	TaskStartDispInit();                                   /* Setup the display                        */

	OS_ENTER_CRITICAL();                                   /* Install uC/OS-II's clock tick ISR        */
	PC_VectSet(0x08, OSTickISR);
	PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
	OS_EXIT_CRITICAL();

	OSStatInit();

	if (select == 1)
		// 1을 입력하면 decision task로 보내는 mailbox 4개, random task로 보내는 mailbox 4개 총 8개를 생성
	{
		for (i = 0; i < 4; i++)
		{
			mbox_to_decision[i] = OSMboxCreate(0);
		}

		for (i = 0; i < 4; i++)
		{
			mbox_to_random[i] = OSMboxCreate(0);
		}
	}

	else if (select == 2)
		// 2를 입력하면 decision task로 보내는 queue 1개, random task로 보내는 queue 1개 총 2개를 생성
	{
		queue_to_decision = OSQCreate(decision_queue, 4);
		queue_to_random = OSQCreate(random_queue, 4);
	}

	/* Initialize uC/OS-II's statistics         */

	TaskStartCreateTasks();                                /* Create all other tasks                   */

	for (;;) {
		//TaskStartDisp();                                   /* Update the display                       */

		if (PC_GetKey(&key)) {                             /* See if key has been pressed              */
			if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
				PC_DOSReturn();                            /* Yes, return to DOS                       */
			}
		}

		OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
		OSTimeDly(OS_TICKS_PER_SEC);                       /* Wait one second                          */
	}
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDispInit(void)
{
	/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
	/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
	PC_DispStr(0, 0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
	PC_DispStr(0, 1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 3, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 20, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 21, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 22, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 23, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
	PC_DispStr(0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
	/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
	/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDisp(void)
{
	char   s[80];


	sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */
	PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

	sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */
	PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

	sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */
	PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

	sprintf(s, "V%4.2f", (float)OSVersion() * 0.01);               /* Display uC/OS-II's version number    */
	PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

	switch (_8087) {                                               /* Display whether FPU present          */
	case 0:
		PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;

	case 1:
		PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;

	case 2:
		PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;

	case 3:
		PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
		break;
	}
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks(void)
{
	INT8U  i;

	for (i = 0; i < N_TASKS; i++)	// random task 4개와 decision task 1개 총 5개의 task 생성
	{
		TaskData[i] = '0' + i;
		OSTaskCreate(Task, (void*)& TaskData[i], &TaskStk[i][TASK_STK_SIZE - 1], i + 1);
	}
}
void Task(void* pdata) {
	INT8U err;

	INT8U push_number; //0-63 사이의 랜덤 숫자를 random task 1-4가 생성한다.
	INT8U get_number[4]; //random task 1-4가 만든 랜덤 숫자를 decision task가 받아온다.

	INT8U i, j; //for문 용

	INT8U min; //get_number[4] 에서 가장 작은 숫자
	INT8U min_task; //가장 작은 숫자를 보내온 random task의 index
	INT8U task_number = (int)(*(char*)pdata - 48); //각 task의 index. 
	//pdata는 char 인데, '0'은 아스카코드 48이므로 -48을 하면 숫자 변환이 됨 

	char push_letter; //W 혹은 L을 저장함. desicion task 에서 생성
	char get_letter; //W 혹은 L을 저장함. decision task가 만든 문자를 random task 1-4에서 받아옴
	
	int fgnd_color, bgnd_color; //random task가 1-4가 만약 W이면 칠해줄 색

	char s[10]; //중간중간에 숫자를 두자리-> %2d로 변환할때 씀

	//task_number,pdata가 0-3이면 random task 1-4, 4이면 decision task
	if (*(char*)pdata == '4') { //decision task 일 경우
		for (;;) {
			for (i = 0; i < N_TASKS - 1; i++) {
				if (select == 1) {
					get_number[i] = *(int*)(OSMboxPend(mbox_to_decision[i], 0, & err));
					//decision  task는 메일 박스에 랜덤 숫자가 전송 될때 까지 기다린다.
				}
				else if (select == 2) {
					get_number[i] = *(int*)(OSQPend(queue_to_decision, 0, &err));
					//decsision task는 메세지 큐에 랜덤 숫자가 전송 될 때까지 기다린다.
				}
			}

			min = get_number[0];
			min_task = 0;

			for (i = 1; i < N_TASKS - 1; i++) {//random task가 보낸 4개의 랜덤 숫자중 가장 작은 수 찾기
				if (get_number[i] < min) {
					min = get_number[i];
					min_task = i;
				}
			}

			for (i = 0; i < N_TASKS-1; i++) {
				if (i == min_task) {//가장 작은 랜덤 넘버를 보낸 random task에 W를 전송
					push_letter = 'W';
				}
				else {
					push_letter = 'L';
				}

				if (select == 1) //메일 박스로 random task에게 결과를 보낸다
					OSMboxPost(mbox_to_random[i], &push_letter);

				else if (select == 2) //메세지 큐로 random task에게 결과를 보낸다
					OSQPost(queue_to_random, &push_letter);

				
			}
		}
			
	}
	else {  //random task 1-4 일 경우
		for (;;) {
			push_number = random(64);
			sprintf(s, "%2d", push_number);

			PC_DispStr(0 + 18 * task_number, 4, "task", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispChar(4 + 18 * task_number, 4, *(char*)pdata, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			PC_DispStr(6 + 18 * task_number, 4, s, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

			if (select == 1) {
				OSMboxPost(mbox_to_decision[task_number], &push_number);
				//메일 박스로 decision task에 랜덤 숫자 전송

				get_letter = *(char*)(OSMboxPend(mbox_to_random[task_number], 0, &err));
			}
			else if (select == 2) {
				OSQPost(queue_to_decision, &push_number);
				//메세지 큐로 decision task에 랜덤숫자 전송

				get_letter = *(char*)(OSQPend(queue_to_random, 0, &err));
				//메세지 큐로  decision task에서 결과 W or L 을 받아옴
			}

			if (*(char*)pdata == '0') {
				bgnd_color = DISP_BGND_RED;
				fgnd_color = DISP_FGND_RED;
			}
			else if (*(char*)pdata == '1') {
				bgnd_color = DISP_BGND_CYAN;
				fgnd_color = DISP_FGND_CYAN;
			}
			else if (*(char*)pdata == '2') {
				bgnd_color = DISP_BGND_BLUE;
				fgnd_color = DISP_FGND_BLUE;
			}
			else if (*(char*)pdata == '3') {
				bgnd_color = DISP_BGND_GREEN;
				fgnd_color = DISP_FGND_GREEN;
			}

			PC_DispChar(8 + 18 * task_number, 4, '[', DISP_FGND_WHITE + bgnd_color);
			PC_DispChar(10 + 18 * task_number, 4, ']', DISP_FGND_WHITE + bgnd_color);
			PC_DispChar(9 + 18 * task_number, 4, get_letter, DISP_FGND_WHITE + bgnd_color);

			if (get_letter == 'W') {//W를 받은 random task는 해당 task의 색을 화면에 표시
				for (i = 5; i < 25; i++) {
					for (j = 0; j < 80; j++) {
						PC_DispChar(j, i, ' ', fgnd_color + bgnd_color);
					}
				}
			}
			OSTimeDlyHMSM(0, 0, 2, 0);
		}

	}

}