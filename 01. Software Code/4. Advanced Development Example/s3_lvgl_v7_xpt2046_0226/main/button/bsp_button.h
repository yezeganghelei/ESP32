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

/** Macro for setting the button pressed level
	*  If the button is pressed high, set KEY_ON=1, KEY_OFF=0
	*  If the button is pressed low, set KEY_ON=0, KEY_OFF=1
	*/
#define KEY_ON 1
#define KEY_OFF 0

#define BTN_NAME_MAX 32 // Name is at most 32 bytes

/* Button debounce time is 40 ms; a call period of 20 ms is recommended
 Only when the state remains unchanged for 40 ms is it considered valid, including both release and press events
*/

/* Whether to support single-click and double-click triggers simultaneously. If this macro is enabled, both single and double clicks invoke callbacks, but the single click responds with a delay,
   because it must determine whether a double click followed; otherwise, the delay is the double-click interval BUTTON_DOUBLE_TIME.
   If this macro is disabled, only one of single click/double click should exist in the project; otherwise, a single click is triggered when the double click responds,
   because a double click is produced only after a press and release */

#define SINGLE_AND_DOUBLE_TRIGGER 1

//#define CONTINUOS_TRIGGER    // whether continuous triggering is supported; if continuous firing is used, do not detect single/double click and long press

/* Whether long-press release is required to trigger. If this macro is enabled, a single long press is triggered only after the long press is released;
   otherwise, the long press is triggered continuously after the specified time, with the trigger period determined by BUTTON_LONG_CYCLE */

#define LONG_FREE_TRIGGER 0
// Whether to trigger a single long press during a long press and trigger long-press release on final release
#define LONG_FREE_ENABLE 1

#ifndef BUTTON_DEBOUNCE_TIME
#define BUTTON_DEBOUNCE_TIME 5 // Debounce time      (n-1)*call period
#endif

#ifndef BUTTON_CONTINUOS_CYCLE
#define BUTTON_CONTINUOS_CYCLE 1 // Continuous-press trigger period  (n-1)*call period
#endif

#ifndef BUTTON_LONG_CYCLE
#define BUTTON_LONG_CYCLE 1 // Long-press trigger period  (n-1)*call period
#endif

#ifndef BUTTON_DOUBLE_TIME
#define BUTTON_DOUBLE_TIME 10 // Double-click interval  (n-1)*call period; 200-600 ms recommended
#endif

#ifndef BUTTON_LONG_TIME
#define BUTTON_LONG_TIME 20 /* Sustain for n seconds ((n-1)*call period ms) to be considered a long-press event */
#endif

#define TRIGGER_CB(event)            \
  if (btn->CallBack_Function[event]) \
  btn->CallBack_Function[event]((Button_t *)btn)

typedef void (*Button_CallBack)(void *); /* Button trigger callback function; the user must implement it */

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
  number_of_event, /* Events that trigger callbacks */
  NONE_TRIGGER
} Button_Event;

/*
	Each button corresponds to one global structure variable.
	Its members are required to implement filtering and the various button states.
*/
typedef struct button
{
  /* Below is a function pointer to the function that determines whether the button is pressed */
  uint8_t (*Read_Button_Level)(void); /* Button level read function; the user must implement it */

  char Name[BTN_NAME_MAX];

  uint8_t Button_State : 4;         /* Current button state (pressed or released) */
  uint8_t Button_Last_State : 4;    /* Previous button state, used to detect double clicks */
  uint8_t Button_Trigger_Level : 2; /* Button trigger level */
  uint8_t Button_Last_Level : 2;    /* Current button level */

  uint8_t Button_Trigger_Event; /* Button trigger event: single click, double click, long press, etc. */

  Button_CallBack CallBack_Function[number_of_event];

  uint8_t Button_Cycle; /* Continuous button press period */

  uint8_t Timer_Count;   /* Timing */
  uint8_t Debounce_Time; /* Debounce time */

  uint8_t Long_Time; /* Button press duration */

  struct button *Next;

} Button_t;

/* Function declarations exposed for external use */

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
