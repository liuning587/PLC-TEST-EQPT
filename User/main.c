/** 
 * @file     main.c
 * @brief    载波测试工装
 * @details  
 * @author   华兄
 * @email    591881218@qq.com
 * @date     2015
 * @version  vX.XX
 * @par Copyright (c):  
 *           深圳市合尔凯科技有限公司
 * @par History:          
 *   version: author, date, desc\n 
 */

#include "includes.h"

                                                          
OS_STK                 App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
OS_STK                 App_TaskKeyStk[APP_CFG_TASK_KEY_STK_SIZE];
OS_STK                 App_TaskEndTickStk[APP_CFG_TASK_END_TICK_STK_SIZE];
OS_STK                 App_TaskEndProcStk[APP_CFG_TASK_END_PROC_STK_SIZE];
OS_STK                 App_TaskPlcStk[APP_CFG_TASK_PLC_STK_SIZE];
OS_STK                 App_TaskDispStk[APP_CFG_TASK_DISP_STK_SIZE];

static  void           App_TaskStart                (void *p_arg);
static  void           App_TaskKey                  (void *p_arg);
static  void           App_TaskEndTick              (void *p_arg);
static  void           App_TaskEndProc              (void *p_arg);
static  void           App_TaskPlc                  (void *p_arg);
static  void           App_TaskDisp                 (void *p_arg);

static  void           App_MemAlloc                 (void);
static  void           App_EventCreate              (void);
static  void           App_TaskCreate               (void);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/
int  main(void)
{   
    INT8U  err;
    
    
    CPU_IntDis();                                               /* Disable all interrupts until we are ready to accept them */

    App_MemAlloc();
    
    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreateExt((void (*)(void *)) App_TaskStart,           /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&App_TaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreateExt()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
static  void  App_TaskStart (void *p_arg)
{
    INT32U  count = 0;

    
    (void)p_arg;                                                /* See Note #1                                              */

    BSP_Init();                                                 /* Initialize BSP functions                                 */
    
    OS_CPU_SysTickInit();                                       /* Initialize the SysTick.                                  */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif   

    MEM_Init();

    End_Init();

    App_EventCreate();                                          /* Create application events                                */

    App_TaskCreate();                                           /* Create application tasks                                 */
        
    while (DEF_TRUE) {
        beep();

        if(!(count % LED_RUN_TOGGLE_PERIOD))
        {
            LED_RUN_TOGGLE();      
        }

        count++;
        
        OSTimeDlyHMSM(0, 0, 0, 50);
    }
}

/*
*********************************************************************************************************
*                                             App_TaskKey()
*
* Description : This task monitors the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskKey()' by 'OSTaskCreateExt()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskKey (void *p_arg)
{
    INT32U  key_msg;

    
    (void)p_arg; 
    
    while (DEF_TRUE) {  
        key_msg = GUI_X_WaitKey();

        GUI_X_KeyProc(key_msg);
    }
}

/*
*********************************************************************************************************
*                                             App_TaskEndTick()
*
* Description : This task monitors the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskEndTick()' by 'OSTaskCreateExt()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskEndTick (void *p_arg)
{
    (void)p_arg; 
    
    while (DEF_TRUE) {    
        End_tick_check();
        
        LED_UART_OFF();

        OSTimeDlyHMSM(0, 0, 0, 20);
    }
}

/*
*********************************************************************************************************
*                                             App_TaskEndProc()
*
* Description : This task monitors the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskEndProc()' by 'OSTaskCreateExt()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskEndProc (void *p_arg)
{
    INT8U  i, err;
    P_END_OBJ  pEndObj = NULL;
    P_MSG_INFO  pMsg = NULL;
    

    (void)p_arg;
    
    while (DEF_TRUE) { 
        OSSemPend(g_sem_end, 0, &err);

        for(i = START_COM_PORT; i < MAX_COM_PORT; i++)
        {
            pEndObj = g_EndObjectPool + i;

            while((pMsg = dequeue(g_EndRxQueue[pEndObj->end_id])) != NULL)
            {
                if(pMsg->msg_header.end_id == pEndObj->end_id)
                {
                    End_postProcess(pEndObj->end_id, pMsg);
                }
                else
                {
                    free_send_buffer(pMsg);
                }
            }
        }
    }
}

/*
*********************************************************************************************************
*                                             App_TaskPlc()
*
* Description : This task monitors the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskPlc()' by 'OSTaskCreateExt()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskPlc (void *p_arg)
{
    INT8U  i, j, err;
    OS_CPU_SR_ALLOC();

    
    (void)p_arg;  

    OSTimeDlyHMSM(0, 0, 0, 500);

    for(i = 0; i < mPLC_NUM; i++)
    {
        mPLC_SELECT(i);
        g_cur_mplc = i;   
        
        for(j = 0; j < mPLC_RESET_NUM; j++)
        {
            while(OSSemAccept(g_sem_plc));
            
            mPLC_RESET_LOW();
            OSTimeDlyHMSM(0, 0, 0, 300);
            mPLC_RESET_HIGH();
                        
            OSSemPend(g_sem_plc, OS_TICKS_PER_SEC, &err);
            
            if(OS_ERR_NONE == err)
            {
                OSTimeDlyHMSM(0, 0, 0, 300);
            }
            else
            {
                
            }          
        }
    }

    LED_mPLC_ON();

    OSTimeDlyHMSM(0, 0, 2, 0);

    LED_mPLC_OFF();

    while (DEF_TRUE) {
        FREQ_SELECT(PLC_FREQ_270KHz);

        for(i = 0; i < PLC_FREQ_NUM; i++)
        {
            g_cur_freq = i;
            
            for(j = 0; j < mPLC_NUM; j++)
            {
                mPLC_SELECT(j);
                g_cur_mplc = j;   

                OS_ENTER_CRITICAL();
                g_sta_level[g_cur_mplc][g_cur_freq] = FALSE;
                OS_EXIT_CRITICAL();

                g_sta_level_flag = TRUE;

                while(OSSemAccept(g_sem_plc));
            
                cplc_read_energy();

                if(PLC_FREQ_270KHz == i)
                {
                    OSSemPend(g_sem_plc, 2750, &err);
                }
                else
                {
                    OSSemPend(g_sem_plc, 1500, &err);
                }

                if(OS_ERR_NONE != err)
                {
                    OS_ENTER_CRITICAL();
                    g_mplc_state[g_cur_mplc][g_cur_freq] = FALSE;
                    OS_EXIT_CRITICAL();
                }   

                g_sta_level_flag = FALSE;
                
                OSTimeDlyHMSM(0, 0, 0, 100);
            }
        }

        beep_on();
    }
}

/*
*********************************************************************************************************
*                                             App_TaskDisp()
*
* Description : This task monitors the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskDisp()' by 'OSTaskCreateExt()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskDisp (void *p_arg)
{  
    INT8U  i, err;
    bool  *ptr;
    INT16U  color;
    OS_CPU_SR_ALLOC(); 

    
    (void)p_arg;  

    while (DEF_TRUE) {     
        while(OSSemAccept(g_sem_disp));
        
        lcd_read_id();       

        OSSemPend(g_sem_disp, OS_TICKS_PER_SEC, &err);

        if(OS_ERR_NONE == err)
        {
            LED_DISP_ON();
        }
        else
        {
            LED_DISP_OFF();
        }

        for(i = 0; i < (MAX_PLC_GROUP_EDIT_NUM - 1); i++)
        {
        	memcpy(g_color_buf, g_color_code, sizeof(g_color_code));            
            memcpy(g_font_buf, g_font_code, sizeof(g_font_code));
                    	
        	g_color_buf[EDIT_DP_HIGH_INDEX] = (g_plc_group_edit_dp[i] >> 8) & 0xff;
        	g_color_buf[EDIT_DP_LOW_INDEX]	= ((g_plc_group_edit_dp[i] >> 0) & 0xff) | 0x03;	

        	g_font_buf[EDIT_ADDR_HIGH_INDEX] = (g_plc_group_edit_addr[i] >> 8) & 0xff;
        	g_font_buf[EDIT_ADDR_LOW_INDEX]	= (g_plc_group_edit_addr[i] >> 0) & 0xff;	

            color = LCD_COLOR_WHITE;

            //sprintf((char *)(&g_color_buf[EDIT_DATA_INDEX]), "%c%c", (color >> 8), (color >> 0)); 

            g_color_buf[EDIT_DATA_INDEX] = color >> 8;
            g_color_buf[EDIT_DATA_INDEX + 1] = color >> 0;

            switch(g_mem_para.plc_group)
            {
            case PLC_GROUP_1:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "一");
                break;

            case PLC_GROUP_2:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "二");
                break;

            case PLC_GROUP_3:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "三");
                break;

            case PLC_GROUP_4:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "四");
                break;

            case PLC_GROUP_5:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "五");
                break;

            case PLC_GROUP_6:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "六");
                break;

            case PLC_GROUP_7:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "七");
                break;

            case PLC_GROUP_8:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "八");
                break;             

            case PLC_GROUP_9:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "九");
                break;

            default:
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "空");
                break;
            }
            
            lcd_disp(g_color_buf, sizeof(g_color_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);  

            lcd_disp(g_font_buf, sizeof(g_font_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);             
        }

        ptr = (bool *)g_mplc_state;
        
        for(i = 0; i < MAX_mPLC_STATE_EDIT_NUM; i++)
        {
            memcpy(g_color_buf, g_color_code, sizeof(g_color_code));
            memcpy(g_font_buf, g_font_code, sizeof(g_font_code));
        	
        	g_color_buf[EDIT_DP_HIGH_INDEX] = (g_mplc_state_edit_dp[i] >> 8) & 0xff;
        	g_color_buf[EDIT_DP_LOW_INDEX]	= ((g_mplc_state_edit_dp[i] >> 0) & 0xff) | 0x03;	

        	g_font_buf[EDIT_ADDR_HIGH_INDEX] = (g_mplc_state_edit_addr[i] >> 8) & 0xff;
        	g_font_buf[EDIT_ADDR_LOW_INDEX]	= (g_mplc_state_edit_addr[i] >> 0) & 0xff;	

            OS_ENTER_CRITICAL();
        	if(TRUE == *ptr)	
        	{		
                color = LCD_COLOR_WHITE;
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "成功");      
        	}
        	else
        	{
                color = LCD_COLOR_RED;
                sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "失败");      
        	}
            OS_EXIT_CRITICAL();
            
            //sprintf((char *)(&g_color_buf[EDIT_DATA_INDEX]), "%c%c", (color >> 8), (color >> 0)); 

            g_color_buf[EDIT_DATA_INDEX] = color >> 8;
            g_color_buf[EDIT_DATA_INDEX + 1] = color >> 0;

            ptr++;            

            lcd_disp(g_color_buf, sizeof(g_color_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);            

            lcd_disp(g_font_buf, sizeof(g_font_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);      
        }        

        ptr = (bool *)g_sta_level;
        
        for(i = 0; i < MAX_ERR_EDIT_NUM; i++)
        {
            memcpy(g_color_buf, g_color_code, sizeof(g_color_code));
            memcpy(g_font_buf, g_font_code, sizeof(g_font_code));
        	
        	g_color_buf[EDIT_DP_HIGH_INDEX] = (g_err_edit_dp[i] >> 8) & 0xff;
        	g_color_buf[EDIT_DP_LOW_INDEX]	= ((g_err_edit_dp[i] >> 0) & 0xff) | 0x03;	

        	g_font_buf[EDIT_ADDR_HIGH_INDEX] = (g_err_edit_addr[i] >> 8) & 0xff;
        	g_font_buf[EDIT_ADDR_LOW_INDEX]	= (g_err_edit_addr[i] >> 0) & 0xff;

            color = LCD_COLOR_WHITE;

            //sprintf((char *)(&g_color_buf[EDIT_DATA_INDEX]), "%c%c", (color >> 8), (color >> 0)); 

            g_color_buf[EDIT_DATA_INDEX] = color >> 8;
            g_color_buf[EDIT_DATA_INDEX + 1] = color >> 0;

            OS_ENTER_CRITICAL();
        	sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "%d", *ptr);      
            OS_EXIT_CRITICAL();         

            ptr++;

            lcd_disp(g_color_buf, sizeof(g_color_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);            

            lcd_disp(g_font_buf, sizeof(g_font_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);      
        }        

        for(i = 0; i < MAX_OS_INFO_EDIT_NUM; i++)
        {
        	memcpy(g_color_buf, g_color_code, sizeof(g_color_code));            
            memcpy(g_font_buf, g_font_code, sizeof(g_font_code));
                    	
        	g_color_buf[EDIT_DP_HIGH_INDEX] = (g_os_info_edit_dp[i] >> 8) & 0xff;
        	g_color_buf[EDIT_DP_LOW_INDEX]	= ((g_os_info_edit_dp[i] >> 0) & 0xff) | 0x03;	

        	g_font_buf[EDIT_ADDR_HIGH_INDEX] = (g_os_info_edit_addr[i] >> 8) & 0xff;
        	g_font_buf[EDIT_ADDR_LOW_INDEX]	= (g_os_info_edit_addr[i] >> 0) & 0xff;	

            color = LCD_COLOR_WHITE;

            //sprintf((char *)(&g_color_buf[EDIT_DATA_INDEX]), "%c%c", (color >> 8), (color >> 0));

            g_color_buf[EDIT_DATA_INDEX] = color >> 8;
            g_color_buf[EDIT_DATA_INDEX + 1] = color >> 0;
        	
        	if(0 == i)
        	{
        		sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "%3d", OSCPUUsage);                
        	}
        	else
        	{
        		sprintf((char *)(&g_font_buf[EDIT_DATA_INDEX]), "%1d.%02d", OSVersion() / 100, OSVersion() % 100);
        	}

            lcd_disp(g_color_buf, sizeof(g_color_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);  

            lcd_disp(g_font_buf, sizeof(g_font_buf));
            OSTimeDlyHMSM(0, 0, 0, LCD_DISP_TIME);             
        }                  
    }
}

/*
*********************************************************************************************************
*                                      App_MemAlloc
*
* Description :  Allocs the application memories.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  Main().
*
* Note(s)     :  none.
*********************************************************************************************************
*/
static void App_MemAlloc(void)
{

}

/*
*********************************************************************************************************
*                                      App_EventCreate
*
* Description :  Creates the application events.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/
static  void  App_EventCreate (void)
{
    g_sem_end = OSSemCreate(0);
    g_sem_disp = OSSemCreate(0);
    g_sem_plc = OSSemCreate(0);    
    g_sem_key = OSSemCreate(0);
}

/*
*********************************************************************************************************
*                                      App_TaskCreate()
*
* Description :  This function creates the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/
static  void  App_TaskCreate (void)
{       
    INT8U  err;
    

    OSTaskCreateExt((void (*)(void *)) App_TaskKey,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskKeyStk[APP_CFG_TASK_KEY_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_KEY_PRIO,
                    (INT16U          ) APP_CFG_TASK_KEY_PRIO,
                    (OS_STK         *)&App_TaskKeyStk[0],
                    (INT32U          ) APP_CFG_TASK_KEY_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskNameSet(APP_CFG_TASK_KEY_PRIO, "Key", &err);

    OSTaskCreateExt((void (*)(void *)) App_TaskEndTick,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskEndTickStk[APP_CFG_TASK_END_TICK_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_END_TICK_PRIO,
                    (INT16U          ) APP_CFG_TASK_END_TICK_PRIO,
                    (OS_STK         *)&App_TaskEndTickStk[0],
                    (INT32U          ) APP_CFG_TASK_END_TICK_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskNameSet(APP_CFG_TASK_END_TICK_PRIO, "EndTick", &err);

    OSTaskCreateExt((void (*)(void *)) App_TaskEndProc,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskEndProcStk[APP_CFG_TASK_END_PROC_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_END_PROC_PRIO,
                    (INT16U          ) APP_CFG_TASK_END_PROC_PRIO,
                    (OS_STK         *)&App_TaskEndProcStk[0],
                    (INT32U          ) APP_CFG_TASK_END_PROC_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskNameSet(APP_CFG_TASK_END_PROC_PRIO, "EndProc", &err);    

    OSTaskCreateExt((void (*)(void *)) App_TaskPlc,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskPlcStk[APP_CFG_TASK_PLC_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_PLC_PRIO,
                    (INT16U          ) APP_CFG_TASK_PLC_PRIO,
                    (OS_STK         *)&App_TaskPlcStk[0],
                    (INT32U          ) APP_CFG_TASK_PLC_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskNameSet(APP_CFG_TASK_PLC_PRIO, "PLC", &err);    
    
    OSTaskCreateExt((void (*)(void *)) App_TaskDisp,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskDispStk[APP_CFG_TASK_DISP_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_DISP_PRIO,
                    (INT16U          ) APP_CFG_TASK_DISP_PRIO,
                    (OS_STK         *)&App_TaskDispStk[0],
                    (INT32U          ) APP_CFG_TASK_DISP_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskNameSet(APP_CFG_TASK_DISP_PRIO, "Display", &err);    
}

