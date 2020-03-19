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
#define          N_TASKS            4





/*

*********************************************************************************************************

*                                              VARIABLES

*********************************************************************************************************

*/



//OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
//OS_STK        TaskStartStk[TASK_STK_SIZE];
//char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */


char TaskVotingData[N_TASKS - 1]; //voting task에서 전달되는 파라미터, 여기서 TaskStartCreateTask에서 각 태스크의 priority를 넣어준다.

OS_STK TaskDecisionStk[TASK_STK_SIZE]; //decision task의 생성시 쓰이는 스택 
OS_STK TaskVotingStk[N_TASKS - 1][TASK_STK_SIZE]; //voting task의 생성시 쓰이는 스택
OS_STK TaskStartStk[TASK_STK_SIZE];

OS_EVENT* RandomSem; // 세마포어 변수 선언
OS_FLAG_GRP* v_grp;
OS_FLAG_GRP* d_grp; // 이벤트 플래그
OS_EVENT* queue_to_master;
OS_EVENT* mbox_to_voting[N_TASKS - 1];

INT8U majority;	//0이 나온 횟수
void* master_queue[N_TASKS - 1]; //queue 사용 배열

								 //char receive_array[N_TASKS - 1]; // 각 랜덤 태스크가 만든 랜덤 숫자 저장
								 //int send_array[N_TASKS - 1]; // decision task가 결정한 W나  L을 저장





								 /*

								 *********************************************************************************************************

								 *                                         FUNCTION PROTOTYPES

								 *********************************************************************************************************

								 */



								 //void Task(void* data);
								 //Task를 2개의 함수로 나누기
void Voting_Task(void* pdata);
void Decision_Task(void* pdata);

static void  TaskStart(void* data);                  /* Function prototypes of tasks                  */
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
	INT8U err;
	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);                        /* Clear the screen                         */
	OSInit();                                              /* Initialize uC/OS-II                      */

	RandomSem = OSSemCreate(1);
	v_grp = OSFlagCreate(0x00, &err);
	d_grp = OSFlagCreate(0x00, &err);
	queue_to_master = OSQCreate(master_queue, 3);
	mbox_to_voting[0] = OSMboxCreate(0);
	mbox_to_voting[1] = OSMboxCreate(0);
	mbox_to_voting[2] = OSMboxCreate(0);

	PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
	PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */
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



	OSStatInit();                                          /* Initialize uC/OS-II's statistics         */



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



//static  void  TaskStartDisp(void)

//{

//	char   s[80];

//

//

//	sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */

//	PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

//

//	sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */

//	PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

//

//	sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */

//	PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

//

//	sprintf(s, "V%4.2f", (float)OSVersion() * 0.01);               /* Display uC/OS-II's version number    */

//	PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

//

//	switch (_8087) {                                               /* Display whether FPU present          */

//	case 0:

//		PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);

//		break;

//

//	case 1:

//		PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);

//		break;

//

//	case 2:

//		PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);

//		break;

//

//	case 3:

//		PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);

//		break;

//	}

//}



/*$PAGE*/

/*

*********************************************************************************************************

*                                             CREATE TASKS

*********************************************************************************************************

*/



static  void  TaskStartCreateTasks(void)
{
	INT8U i;
	INT8U err; // 에러

	for (i = 0; i < N_TASKS - 1; i++)  //voting task 3개와 decision task 1개 총 4개의 task 생성
	{
		TaskVotingData[i] = i;
		OSTaskCreate(Voting_Task, (void*)&TaskVotingData[i], &TaskVotingStk[i][TASK_STK_SIZE - 1], i + 1);
	}
	OSTaskCreate(Decision_Task, (void*)0, &TaskDecisionStk[TASK_STK_SIZE - 1], i + 1);

}
/*

*********************************************************************************************************
*                                            TASKS
*********************************************************************************************************

*/

void Voting_Task(void* pdata) {
	INT8U err;
	INT8U random_number;
	INT8U task_number;
	INT8U result;
	INT8U i;
	char ch;

	for (;;) {
		task_number = *(int*)pdata; // task번호 생성
		random_number = rand() % 2; //임의의 수를 이용해서 O나 X를 전송
		if (random_number == 0) {
			ch = 'O';
		}
		else {
			ch = 'X';
		}
		OSQPost(queue_to_master, &ch); //queue에 Master task에게 (voting task 중 하나)결과를 전송
		OSFlagPend(v_grp, 0x07, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err); // decision task에서 master를 결정할 때까지 대기

		result = *(int*)OSMboxPend(mbox_to_voting[task_number], 0, &err); // master가 누구인지 decision  task에서 받아옴

		if (result == task_number) { //master task가 할 일
			OSSemPend(RandomSem, 0, &err);
			majority = 0;
			for (i = 0; i < N_TASKS - 1; i++) {
				if ((*(char*)(OSQPend(queue_to_master, 0, &err))) == 'O') {
					majority++; //O이 나온 횟수 카운트
				}
				PC_DispChar(30, 4, '0' + majority, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
			}
			OSFlagPost(d_grp, 0x01 << result, OS_FLAG_SET, &err); //decision task로 작업이 끝났음을 알린다.
			OSSemPost(RandomSem);
		}
		OSTimeDlyHMSM(0, 0, 2, 0);
	}
}

void Decision_Task(void* pdata) {
	INT8U master;
	INT8U err;
	INT8U bgnd_color;
	INT8U i, j;

	for (;;) {
		// --- master task 결정 후 voting task로 결과 전달 ------
		master = rand() % 3;
		OSMboxPost(mbox_to_voting[0], &master);
		OSMboxPost(mbox_to_voting[1], &master);
		OSMboxPost(mbox_to_voting[2], &master);
		OSFlagPost(v_grp, 0x01 << master, OS_FLAG_SET, &err);
		// --------------------------------------------------------
		OSFlagPend(d_grp, 0x07, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err); //master task 작업 끝날 때까지 대기

		OSSemPend(RandomSem, 0, &err);
		if (majority > 1) { // O이 많으면 파랑(O가 2개,3개)
			bgnd_color = DISP_BGND_BLUE;
		}
		else { //X가 많으면 빨강(O가 0,1개)
			bgnd_color = DISP_BGND_RED;
		}

		for (i = 0; i < N_TASKS - 1; i++) {
			//화면에 각 voting task가 O인지 X인지 결과 출력(master task 초록)
			if (master == i) {
				PC_DispChar(9 * i, 4, *(char*)master_queue[i], DISP_FGND_WHITE + DISP_BGND_GREEN);
			}
			else {
				PC_DispChar(9 * i, 4, *(char*)master_queue[i], DISP_FGND_WHITE + DISP_BGND_BLACK);
			}
		}
		for (j = 5; j < 20; j++) {
			for (i = 0; i < 80; i++) {
				PC_DispChar(i, j, ' ', DISP_FGND_WHITE + bgnd_color);
			}
		}
		OSSemPost(RandomSem);

		OSTimeDlyHMSM(0, 0, 2, 0);
	}
}
