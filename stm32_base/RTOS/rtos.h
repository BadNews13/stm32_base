#ifndef RTOS_H
#define RTOS_H

/******************************************************************************************
 * За основу взят планировщик задач с сайта ChipEnable.ru                                 *
 * http://chipenable.ru/index.php/programming-avr/item/110-planirovschik.html             *
 *                                                                                        *
 * Доработал Шибанов Владимир aka KontAr                                                  *
 * Дата: 26.03.2014                                                                       *
 *                                                                                        *
 * Изменения:                                                                             *
 * - добавлен однократный вызов задачи                                                    *
 * - добавлено удаление задачи по имени                                                   *
 * - при повторном добавлении задачи обновляются ее переменные                            *
 * - добавлен указатель на "хвост" списка                                                 *
 * - функции РТОС скорректированы с учетом "хвоста"                                       *
 ******************************************************************************************/

//#include "stm32f10x.h"
#include <global_defines.h>
#include <timerRTOS.h>

#define MAX_TASKS	16               // Количество задач

//#define RTOS_ISR	TIMER1_COMPA_vect // Вектор прерывания
/******************************************************************************************
 * Структура задачи
 */

typedef struct task
{
   void (*pFunc) (void);			// указатель на функцию
   uint16_t delay;					// задержка перед первым запуском задачи
   uint16_t period;					// сколько раз запустить задачу (перезапускаться будет бет задержки)
   uint8_t run;						// флаг готовности задачи к запуску
}task;

/******************************************************************************************
 * Прототипы фукнций
 */
void RTOS_Init (void);

void RTOS_ISR_INT(void);
void RTOS_SetTask (void (*taskFunc)(void), u16 taskDelay, u16 taskPeriod);

void RTOS_DeleteTask (void (*taskFunc)(void));
void RTOS_DispatchTask (void);

void RTOS_WDT_ISR_CALL(void);

uint16_t RTOS_calc_timeout_by_prescaler(uint16_t time_delay);

#endif
