#ifndef _BSP_BUTTON_H
#define _BSP_BUTTON_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_system.h"
#include <esp_err.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include "freertos/task.h"

/** Key press macro
	*  When the button is pressed, the level is high，set up KEY_ON=1， KEY_OFF=0
	*  If the button is pressed, it is low level，把宏set up成KEY_ON=0 ，KEY_OFF=1 That’s it
	*/
#define KEY_ON 1
#define KEY_OFF 0

#define BTN_NAME_MAX 32 //The maximum name is32byte

/* 按键Debounce time40ms, The recommended calling period is20ms
 Only detected continuously40msIt is considered valid only if the status remains unchanged，Including two events: pop-up and press
*/

/* 是否支持click&double click同时存在触发，If you choose to enable macro definition，Callback for both single and double clicks，But clicking will delay the response，
   因为必须判断click之后是否触发了double clickotherwise，延迟时间是double click间隔时间 BUTTON_DOUBLE_TIME。
   And if you do not enable this macro definition，建议工程中只存在click/double click中的一个，otherwise，A click will be triggered when double-clicking in response，
   因为double click必须是有一次按下并且释放之后才产生的 */

#define SINGLE_AND_DOUBLE_TRIGGER 1

//#define CONTINUOS_TRIGGER //Whether continuous triggering is supported. If it is triggered continuously, do not detect single, double clicks and long presses.

/* Whether to support long press and release before triggering，If you open this macro definition，Then a single long press is triggered after the long press is released.，
   otherwise在长按指定时间就一直触发长按，The trigger period is given by BUTTON_LONG_CYCLE Decide */

#define LONG_FREE_TRIGGER 0
//Whether to support triggering a long press during a long press  Finally release and start again, press and hold to release
#define LONG_FREE_ENABLE 1

#ifndef BUTTON_DEBOUNCE_TIME
#define BUTTON_DEBOUNCE_TIME 5 //Debounce time      (n-1)*call cycle
#endif

#ifndef BUTTON_CONTINUOS_CYCLE
#define BUTTON_CONTINUOS_CYCLE 1 //Double click trigger cycle time  (n-1)*call cycle
#endif

#ifndef BUTTON_LONG_CYCLE
#define BUTTON_LONG_CYCLE 1 //Long press trigger cycle time  (n-1)*call cycle
#endif

#ifndef BUTTON_DOUBLE_TIME
#define BUTTON_DOUBLE_TIME 10 //double click间隔时间  (n-1)*call cycle  It is recommended to200-600ms
#endif

#ifndef BUTTON_LONG_TIME
#define BUTTON_LONG_TIME 20 /* continuednSecond((n-1)*call cycle ms)，Think long press event */
#endif

#define TRIGGER_CB(event)            \
  if (btn->CallBack_Function[event]) \
  btn->CallBack_Function[event]((Button_t *)btn)

typedef void (*Button_CallBack)(void *); /* The key triggers the callback function, which needs to be implemented by the user. */

typedef enum
{
  BUTTON_DOWN = 0,
  BUTTON_UP,
  BUTTON_DOUBLE,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_CONTINUOS,
  BUTTON_CONTINUOS_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event, /* The event that triggered the callback */
  NONE_TRIGGER
} Button_Event;

/*
	Each button corresponds to a global structure variable.
	Its member variables are necessary to implement filtering and various key states.
*/
typedef struct button
{
  /* Below is a function pointer，Points to the function that determines whether the button is pressed. */
  uint8_t (*Read_Button_Level)(void); /* Reading the key level function requires the user to implement */

  char Name[BTN_NAME_MAX];

  uint8_t Button_State : 4;         /* Current status of button（Press or pop up） */
  uint8_t Button_Last_State : 4;    /* Last key press status，用于判断double click */
  uint8_t Button_Trigger_Level : 2; /* Key trigger level */
  uint8_t Button_Last_Level : 2;    /* Current level of button */

  uint8_t Button_Trigger_Event; /* Button trigger event，click，double click，Long press and wait */

  Button_CallBack CallBack_Function[number_of_event];

  uint8_t Button_Cycle; /* Continuous key cycle */

  uint8_t Timer_Count;   /* Timing */
  uint8_t Debounce_Time; /* Debounce time */

  uint8_t Long_Time; /* Key press duration */

  struct button *Next;

} Button_t;

/* Function declaration for external calls */

void Button_Create(const char *name,
                   Button_t *btn,
                   uint8_t (*read_btn_level)(void),
                   uint8_t btn_trigger_level);

void Button_Attach(Button_t *btn, Button_Event btn_event, Button_CallBack btn_callback);

void Button_Cycle_Process(Button_t *btn);

void Button_Process(void);

void Button_Delete(Button_t *btn);

void Search_Button(void);

void Get_Button_EventInfo(Button_t *btn);
uint8_t Get_Button_Event(Button_t *btn);
uint8_t Get_Button_State(Button_t *btn);
void Button_Process_CallBack(void *btn);

#endif