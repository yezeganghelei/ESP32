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
	*  When the button is pressed the level is high: set KEY_ON=1 and KEY_OFF=0.
	*  If the button is pressed at a low level: set KEY_ON=0 and KEY_OFF=1.
	*/
#define KEY_ON 1
#define KEY_OFF 0

#define BTN_NAME_MAX 32 //Maximum name length is 32 bytes

/* Button debounce time is 40 ms; the recommended call cycle is 20 ms.
  A state is considered valid only if it remains unchanged for 40 ms continuously,
  including both pop-up and press events.
*/

/* Whether single click and double click can both trigger. If this macro is enabled,
   callbacks are registered for both single and double clicks, but the single-click
   response is delayed because it must first determine whether a double click follows;
   the delay is the double-click interval BUTTON_DOUBLE_TIME.
   If this macro is disabled, it is recommended to use only one of single/double click in
   the project; otherwise a single click is also triggered on the first click of a
   double click, because a double click is only produced after one press and release. */

#define SINGLE_AND_DOUBLE_TRIGGER 1

//#define CONTINUOS_TRIGGER //Whether continuous triggering is supported. If it is triggered continuously, do not detect single, double clicks and long presses.

/* Whether to trigger on long-press release. If this macro is enabled, a single long press
   is triggered after the long press is released; otherwise the long press is triggered
   continuously once the long-press time is reached, with the trigger period given by
   BUTTON_LONG_CYCLE. */

#define LONG_FREE_TRIGGER 0
//Whether to trigger a long press during the hold and again on release
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
#define BUTTON_DOUBLE_TIME 10 //Double-click interval (n-1)*call cycle; 200-600 ms recommended
#endif

#ifndef BUTTON_LONG_TIME
#define BUTTON_LONG_TIME 20 /* Continued for n seconds ((n-1)*call cycle ms); considered a long-press event */
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
  /* Below is a function pointer to the function that determines whether the button is pressed. */
  uint8_t (*Read_Button_Level)(void); /* Reading the key level function requires the user to implement */

  char Name[BTN_NAME_MAX];

  uint8_t Button_State : 4;         /* Current button state (press or pop up) */
  uint8_t Button_Last_State : 4;    /* Previous button state, used to detect a double click */
  uint8_t Button_Trigger_Level : 2; /* Key trigger level */
  uint8_t Button_Last_Level : 2;    /* Current level of button */

  uint8_t Button_Trigger_Event; /* Button trigger event: click, double click, long press, etc. */

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