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

#define           N_TASKS            5





/*

*********************************************************************************************************

*                                              VARIABLES

*********************************************************************************************************

*/



OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */

OS_STK        TaskStartStk[TASK_STK_SIZE];

char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */



OS_EVENT* Sem; // 세마포어

OS_FLAG_GRP* s_grp;

OS_FLAG_GRP* r_grp; // 이벤트 플래그



char receive_array[N_TASKS - 1]; // 각 랜덤 태스크가 만든 랜덤 숫자 저장

int send_array[N_TASKS - 1]; // decision task가 결정한 W나  L을 저장





/*

*********************************************************************************************************

*                                         FUNCTION PROTOTYPES

*********************************************************************************************************

*/



void Task(void* data);

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





	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);                        /* Clear the screen                         */



	OSInit();                                              /* Initialize uC/OS-II                      */



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



	/* Semaphore 초기화 */

	Sem = OSSemCreate(1);



	/* Event Flag 초기화 */

	s_grp = OSFlagCreate(0x00, &err);

	r_grp = OSFlagCreate(0x00, &err);



	for (i = 0; i < N_TASKS; i++) // random task 4개와 decision task 1개 총 5개의 task 생성

	{

		TaskData[i] = '0' + i;

		OSTaskCreate(Task, (void*)& TaskData[i], &TaskStk[i][TASK_STK_SIZE - 1], i + 1);

	}

}



void Task(void* pdata) {

	INT8U err;



	INT8U push_number; // 0~63 사이의 random number를 random task 1~4가 생성한다.

	// INT8U get_number[4]; // random task 1~4가 만든 random number를 decision task가 받아온다.



	INT8U i, j; // for문 용



	INT8U min; // get_number[4]에서 가장 작은 숫자

	INT8U min_task; // 가장 작은 숫자를 보내온 random task의 index

	INT8U task_number = (int)(*(char*)pdata - 48); // 각 task의 index

	// pdata는 char인데, '0'은 아스키코드 48이므로 - 48을 하면 숫자 변환이 됨



	char push_letter; // W 혹은 L 을 저장함. Decision task에 생성.

	// char get_letter; // W 혹은 L 을 저장함. Decision task가 만든 문자를 Random task 1-4 에서 받아옴



	int fgnd_color, bgnd_color; // Random task 1-4 가 만약 W이면 칠해줄 색



	char s[10]; // 중간중간에 숫자를 두자리-> %2d로 변환할 때 씀



	// task_number = pdata 가 0-3이면 random task, 4이면 decision task

	if (*(char*)pdata == '4') { // decision task일 경우

		for (;;) {

			// 모든 랜덤 태스크에서 랜덤 숫자를 생성해서, s_grp의 조건값이 0x0F(이진수로 1111)이 될때까지 wait
			OSFlagPend(s_grp, 0x0F, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);

			min = send_array[0];
			min_task = 0;

			for (i = 1; i < N_TASKS - 1; i++) {
				// random task가 보낸 4개의 랜덤 숫자중 가장 작은 수 찾기

				if (send_array[i] < min) {
					min = send_array[i];
					min_task = i;
				}
			}



			for (i = 0; i < N_TASKS - 1; i++) {

				// 가장 작은 랜덤 넘버를 보낸 random task에 W를 전송

				if (i == min_task) {
					push_letter = 'W';

				}

				else {

					push_letter = 'L';

				}

				receive_array[i] = push_letter;
				OSFlagPost(r_grp, (1 << i), OS_FLAG_SET, &err);

			}

			OSTimeDlyHMSM(0, 0, 2, 0);

		}

	}

	else { // random task 1-4 일 경우

		for (;;) {

			push_number = random(64);

			sprintf(s, "%2d", push_number);



			// task 0:5	 task 1:10	 task 2 : 3		task3 : 64

			PC_DispStr(0 + 18 * task_number, 4, "task", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

			PC_DispChar(4 + 18 * task_number, 4, *(char*)pdata, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);

			PC_DispStr(6 + 18 * task_number, 4, s, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);



			// Send_array에 접근하는 태스크가 동시에 여러개면 안되니까 critical section을 만든다.

			OSSemPend(Sem, 0, &err);



			// Pend ~ Post 사이 전부 critical section

			send_array[task_number] = push_number; // send_array의 태스크의 index 번째에 랜덤 숫자 저장

			OSFlagPost(s_grp, (1 << task_number), OS_FLAG_SET, &err);

			// 0~3 이므로 1~4를 얻기위해 shifting 한다.
			// 해당 태스크에서 랜덤 숫자를 만들어 send_array에 저장했음을 이벤트 Flag를 통해 알림


			OSSemPost(Sem);
			//Decision task에서 모든 task의 W과 L을 결정해 저장하여
			//rgp의 조건값이 0x0F가 될 때까지 wait
			OSFlagPend(r_grp, (1 << task_number), OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

			OSSemPend(Sem, 0, &err);
			// task의 우선순위에 따른 색 설정

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



			// task 0:5[L]	 task 1:10[L]	 task 2 : 3[W]		task3 : 64[L]
			PC_DispChar(8 + 18 * task_number, 4, '[', DISP_FGND_WHITE + bgnd_color);
			PC_DispChar(10 + 18 * task_number, 4, ']', DISP_FGND_WHITE + bgnd_color);
			PC_DispChar(9 + 18 * task_number, 4, receive_array[task_number], DISP_FGND_WHITE + bgnd_color);



			// W 문자를 받은 task의 색으로 배경색칠
			if (receive_array[task_number] == 'W') {
				for (j = 5; j < 24; j++) {
					for (i = 0; i < 80; i++) {
						PC_DispChar(i, j, ' ', fgnd_color + bgnd_color);
					}
				}
			}

			OSSemPost(Sem);
			OSTimeDlyHMSM(0, 0, 2, 0);

		}

	}

}