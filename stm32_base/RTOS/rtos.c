

#include <rtos.h>


//*********************************** Переменные модуля ***********************************
volatile static task TaskArray[MAX_TASKS];      // очередь задач
volatile static uint8_t arrayTail;              // "хвост" очереди

extern volatile uint32_t uptime_cnt;
extern volatile uint32_t cfgtime_cnt;
//*****************************************************************************************


//************************* Инициализация РТОС, время тика - 1 мс *************************
//inline void RTOS_Init()
void RTOS_Init(void)
{
	timerRTOS_ini();
//	TIMER_2_START;
	arrayTail = 0;	// "хвост" в 0
}
//*****************************************************************************************


//****************************** Добавление задачи в список *******************************
void RTOS_SetTask (void (*taskFunc)(void), u16 taskDelay, u16 taskPeriod)
{
   uint8_t i;

   if(!taskFunc) return;
   for(i = 0; i < arrayTail; i++)						// поиск задачи в текущем списке
   {
      if(TaskArray[i].pFunc == taskFunc)				// если нашли, то обновляем переменные
      {
         __disable_irq ();								// запретить прерывания

         TaskArray[i].delay  = taskDelay;
         TaskArray[i].period = taskPeriod;
         TaskArray[i].run    = 0;

         __enable_irq ();								// разрешить прерывания

         return;										// обновив, выходим
      }
   }

   if (arrayTail < MAX_TASKS)                         // если такой задачи в списке нет
   {                                                  // и есть место,то добавляем
	   __disable_irq (); // запретить прерывания

      TaskArray[arrayTail].pFunc  = taskFunc;
      TaskArray[arrayTail].delay  = taskDelay;
      TaskArray[arrayTail].period = taskPeriod;
      TaskArray[arrayTail].run    = 0;

      arrayTail++;                                    // увеличиваем "хвост"

      __enable_irq ();  // разрешить прерывания
   }
}
//*****************************************************************************************


//******************************* Удаление задачи из списка *******************************
void RTOS_DeleteTask (void (*taskFunc)(void))
{
	uint8_t i;

   for (i=0; i<arrayTail; i++)                        // проходим по списку задач
   {
      if(TaskArray[i].pFunc == taskFunc)              // если задача в списке найдена
      {
    	 __disable_irq (); // запретить прерывания

         if(i != (arrayTail - 1))                     // переносим последнюю задачу
         {                                            // на место удаляемой
//			TaskArray[i] = TaskArray[arrayTail - 1];

			TaskArray[i].pFunc  = TaskArray[arrayTail-1].pFunc;
			TaskArray[i].delay  = TaskArray[arrayTail-1].delay;
			TaskArray[i].period = TaskArray[arrayTail-1].period;
			TaskArray[i].run    = TaskArray[arrayTail-1].run;

         }
         arrayTail--;                                 // уменьшаем указатель "хвоста"

         __enable_irq ();  // разрешить прерывания
         return;
      }
   }
}
//*****************************************************************************************


//*************************** Диспетчер РТОС, вызывается в main ***************************
void RTOS_DispatchTask()
{
	uint8_t i;

   void (*function) (void);

   //dac_set(DISPATCH);
   for (i=0; i<arrayTail; i++)                        // проходим по списку задач
   {
      if (TaskArray[i].run == 1)                      // если флаг на выполнение взведен,
      {                                               // запоминаем задачу, т.к. во
         function = TaskArray[i].pFunc;               // время выполнения может
                                                      // измениться индекс
         if(TaskArray[i].period == 0)
         {                                            // если период равен 0
            RTOS_DeleteTask(TaskArray[i].pFunc);      // удаляем задачу из списка,
         }
         else
         {
            TaskArray[i].run = 0;                     // иначе снимаем флаг запуска
            if(!TaskArray[i].delay)                   // если задача не изменила задержку
            {                                         // задаем ее
               TaskArray[i].period = TaskArray[i].period-1;
            }                                         // задача для себя может сделать паузу
         }
         (*function)();                               // выполняем задачу
      }
   }
}
//*****************************************************************************************


//**************** Таймерная служба RTOS (прерывание аппаратного таймера) *****************
void RTOS_ISR_INT(void)
{
	static uint16_t subseccnt=1;
	subseccnt++;
//	if(subseccnt==10)
//	{
		subseccnt = 0;
		for (uint8_t i=0; i<arrayTail; i++)			// проходим по списку задач
		{
			if  (TaskArray[i].delay == 0)	{TaskArray[i].run = 1;}		// если время до выполнения истекло	// взводим флаг запуска,
			else							{TaskArray[i].delay--;}		// иначе уменьшаем время
		}
//		RTOS_DispatchTask();
//	}
}




/*
void RTOS_WDT_ISR_CALL()
{
	static uint16_t subseccnt=0;

	u08 i;

	for (i=0; i<arrayTail; i++)			// проходим по списку задач
	{
		if  (TaskArray[i].delay == 0)	// если время до выполнения истекло
		TaskArray[i].run = 1;			// взводим флаг запуска,
		else TaskArray[i].delay--;		// иначе уменьшаем время
	}
	if(subseccnt++==1000)
	{
		uptime_cnt++;
		cfgtime_cnt++;
		subseccnt = 0;
	}
	timeout_handler_task();
}
*/

/*
uint16_t RTOS_calc_timeout_by_prescaler(uint16_t time_delay)
{
	uint16_t val=0;
	val=((time_delay*2)+(time_delay*2)/10);
	val=val+val/2;
//	val=time_delay*3;
//	val=val/17;
//	val=val*10;
//	val=((time_delay*40)/5)*100;
	if (val > 0)	{val=val-1;}
	if (val <= 0)	{val=1;}

	return val;
}
*/

