/************************************************************
  * @brief   Button driver
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    button.c
  ***********************************************************/
#include "bsp_button.h"

/*******************************************************************
 *                          Variable declarations                               
 *******************************************************************/

static struct button *Head_Button = NULL;
bool long_press = false;
/*******************************************************************
 *                         Function declarations     
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n);
static void Print_Btn_Info(Button_t *btn);
static void Add_Button(Button_t *btn);

/************************************************************
  * @brief   Button creation
	* @param   name : button name
	* @param   btn : button structure
  * @param   read_btn_level : button level read function; the user must implement it to return a uint8_t level
  * @param   btn_trigger_level : button trigger level
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Create(const char *name,
                   Button_t *btn,
                   uint8_t (*read_btn_level)(void),
                   uint8_t btn_trigger_level)
{
  if (btn == NULL)
  {
  }

  // memset(btn, 0, sizeof(struct button)); // clear the struct information; it is recommended to clear it beforehand

  StrnCopy(btn->Name, name, BTN_NAME_MAX); /* Create the button name */

  btn->Button_State = NONE_TRIGGER;                  // Button state
  btn->Button_Last_State = NONE_TRIGGER;             // Previous button state
  btn->Button_Trigger_Event = NONE_TRIGGER;          // Button trigger event
  btn->Read_Button_Level = read_btn_level;           // Button level read function
  btn->Button_Trigger_Level = btn_trigger_level;     // Button trigger level
  btn->Button_Last_Level = btn->Read_Button_Level(); // Current button level
  btn->Debounce_Time = 0;

  //USART3_Put_String("Button create success!\r\n");

  Add_Button(btn); // Add to the linked list on creation

  Print_Btn_Info(btn); // Print information
}

/************************************************************
  * @brief   Map button trigger events to callback functions
	* @param   btn : button structure
	* @param   btn_event : button trigger event
  * @param   btn_callback : callback handler invoked after the button is triggered; the user must implement it
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Button_Attach(Button_t *btn, Button_Event btn_event, Button_CallBack btn_callback)
{
  if (btn == NULL)
  {
  }

  if (BUTTON_ALL_RIGGER == btn_event)
  {
    for (uint8_t i = 0; i < number_of_event - 1; i++)
      btn->CallBack_Function[i] = btn_callback; // Callback invoked by the button event, used to handle button events
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; // Callback invoked by the button event, used to handle button events
  }
}

/************************************************************
  * @brief   Delete an already created button
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Delete(Button_t *btn)
{
  struct button **curr;
  for (curr = &Head_Button; *curr;)
  {
    struct button *entry = *curr;
    if (entry == btn)
    {
      *curr = entry->Next;
    }
    else
    {
      curr = &entry->Next;
    }
  }
}

/************************************************************
  * @brief   Get the button trigger event
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Get_Button_EventInfo(Button_t *btn)
{
  // Callback invoked by the button event, used to handle button events
  for (uint8_t i = 0; i < number_of_event - 1; i++)
  {
    if (btn->CallBack_Function[i] != 0)
    {

      printf("Button_Event:%d", i);
    }
  }
}

uint8_t Get_Button_Event(Button_t *btn)
{
  return (uint8_t)(btn->Button_Trigger_Event);
}

/************************************************************
  * @brief   Get the button trigger event
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
uint8_t Get_Button_State(Button_t *btn)
{
  return (uint8_t)(btn->Button_State);
}

/************************************************************
  * @brief   Button periodic processing function
  * @param   btn: the button to process
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    This function must be called periodically; a period of 20~50 ms is recommended
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
  uint8_t current_level = (uint8_t)btn->Read_Button_Level(); // Get the current button level
  // Button level changed
  if ((current_level != btn->Button_Last_Level) && (++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME))
  {
    btn->Button_Last_Level = current_level; // Update the current button level
    btn->Debounce_Time = 0;                 // Confirmed as pressed

    // If the previous state was not pressed, change the button state to pressed (first press/double-click press)
    if (((btn->Button_State == NONE_TRIGGER) || (btn->Button_State == BUTTON_DOUBLE)))
    {
      btn->Button_State = BUTTON_DOWN;
    }
    // Release the button
    else if (btn->Button_State == BUTTON_DOWN)
    {
      if (!long_press)
      {

        btn->Button_State = BUTTON_UP;
        TRIGGER_CB(BUTTON_UP); // Trigger release
      }
      else
      {
        long_press = false;
        btn->Button_State = BUTTON_UP;
        btn->Button_Trigger_Event = BUTTON_LONG_FREE;
      }

      //USART3_Put_String("release button !\r\n");
    }
  }

  switch (btn->Button_State)
  {
  case BUTTON_DOWN: // Pressed state
  {
    if (btn->Button_Last_Level == btn->Button_Trigger_Level) // Button pressed
    {
#if CONTINUOS_TRIGGER // Support continuous triggering

      if (++(btn->Button_Cycle) >= BUTTON_CONTINUOS_CYCLE)
      {
        btn->Button_Cycle = 0;
        btn->Button_Trigger_Event = BUTTON_CONTINUOS;
        TRIGGER_CB(BUTTON_CONTINUOS); // Continuous press
        //USART3_Put_String("lianxu button !\r\n");
      }

#else

      btn->Button_Trigger_Event = BUTTON_DOWN;

      if (++(btn->Long_Time) >= BUTTON_LONG_TIME) // Before releasing, update the trigger event to long press
      {
#if LONG_FREE_TRIGGER

        btn->Button_Trigger_Event = BUTTON_LONG;

#elif LONG_FREE_ENABLE
        if (long_press == false)
        {

          TRIGGER_CB(BUTTON_LONG); // Long press
          long_press = true;
          btn->Button_Trigger_Event = BUTTON_LONG_FREE;
        }

#else

        if (++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE) // Period for continuously triggering long press
        {
          btn->Button_Cycle = 0;
          btn->Button_Trigger_Event = BUTTON_LONG;
          TRIGGER_CB(BUTTON_LONG); // Long press
        }
#endif

        if (btn->Long_Time == 0xFF) // Update time overflow
        {
          btn->Long_Time = BUTTON_LONG_TIME;
        }
        //USART3_Put_String("long down count!\r\n");
      }

#endif
    }

    break;
  }

  case BUTTON_UP: // Released state
  {
    if (btn->Button_Trigger_Event == BUTTON_DOWN) // Trigger single click
    {
      if ((btn->Timer_Count <= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State == BUTTON_DOUBLE)) // Double click
      {
        btn->Button_Trigger_Event = BUTTON_DOUBLE;
        TRIGGER_CB(BUTTON_DOUBLE);
        //USART3_Put_String("double click button !\r\n");
        btn->Button_State = NONE_TRIGGER;
        btn->Button_Last_State = NONE_TRIGGER;
      }
      else
      {
        btn->Timer_Count = 0;
        btn->Long_Time = 0; // Long press detection failed; reset to 0

#if (SINGLE_AND_DOUBLE_TRIGGER == 0)
        TRIGGER_CB(BUTTON_DOWN); // Single click
#endif
        btn->Button_State = BUTTON_DOUBLE;
        btn->Button_Last_State = BUTTON_DOUBLE;
      }
    }
#if !LONG_FREE_ENABLE
    else if (btn->Button_Trigger_Event == BUTTON_LONG)
    {
#if LONG_FREE_TRIGGER
      TRIGGER_CB(BUTTON_LONG); // Long press

#endif
      btn->Long_Time = 0;
      btn->Button_State = NONE_TRIGGER;
      long_press = false;
      btn->Button_Last_State = BUTTON_LONG;
      //USART3_Put_String("long click button !\r\n");
    }
#endif
    else if (btn->Button_Trigger_Event == BUTTON_LONG_FREE)
    {
#if LONG_FREE_ENABLE

      TRIGGER_CB(BUTTON_LONG_FREE); // Long-press release

#endif
      btn->Long_Time = 0;
      btn->Button_State = NONE_TRIGGER;
      long_press = false;
      btn->Button_Last_State = BUTTON_LONG_FREE;
      //USART3_Put_String("long_free click button !\r\n");
    }
#if CONTINUOS_TRIGGER
    else if (btn->Button_Trigger_Event == BUTTON_CONTINUOS) // Continuous press
    {
      btn->Long_Time = 0;
      TRIGGER_CB(BUTTON_CONTINUOS_FREE); // Continuous-release
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = BUTTON_CONTINUOS;
    }
#endif

    break;
  }

  case BUTTON_DOUBLE:
  {
    btn->Timer_Count++; // Time tracking
    if (btn->Timer_Count >= BUTTON_DOUBLE_TIME)
    {
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = NONE_TRIGGER;
    }
#if SINGLE_AND_DOUBLE_TRIGGER

    if ((btn->Timer_Count >= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State != BUTTON_DOWN))
    {
      btn->Timer_Count = 0;
      TRIGGER_CB(BUTTON_DOWN); // Single click
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = BUTTON_DOWN;
    }

#endif

    break;
  }

  default:
    break;
  }
}

/************************************************************
  * @brief   Scan buttons by traversal without missing any button
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    This function must be called periodically; once every 20-50 ms is recommended
  ***********************************************************/
void Button_Process(void)
{
  struct button *pass_btn;
  for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
    Button_Cycle_Process(pass_btn);
  }
}

/************************************************************
  * @brief   Traverse buttons
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Search_Button(void)
{
  struct button *pass_btn;
  for (pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
    printf("button node have %s \r\n", pass_btn->Name);
  }
}

/**************************** Internal functions below ********************/

/************************************************************
  * @brief   Copy a string of the specified length
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n)
{
  if (n != 0)
  {
    char *d = dst;
    const char *s = src;
    do
    {
      if ((*d++ = *s++) == 0)
      {
        while (--n != 0)
          *d++ = 0;
        break;
      }
    } while (--n != 0);
  }
  return (dst);
}

/************************************************************
  * @brief   Print button-related information
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static void Print_Btn_Info(Button_t *btn)
{

  printf("\nbutton struct information:\n\
              btn->Name:%s \n\
              btn->Button_State:%d \n\
              btn->Button_Trigger_Event:%d \n\
              btn->Button_Trigger_Level:%d \n\
              btn->Button_Last_Level:%d \n\n",
         btn->Name,
         btn->Button_State,
         btn->Button_Trigger_Event,
         btn->Button_Trigger_Level,
         btn->Button_Last_Level);
  Search_Button();
}
/************************************************************
  * @brief   Link buttons together using a singly linked list
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static void Add_Button(Button_t *btn)
{
  btn->Next = Head_Button;
  Head_Button = btn;
}
