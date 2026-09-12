/************************************************************
  * @brief   Button driven
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
 * Variable declaration
 *******************************************************************/

static struct button *Head_Button = NULL;
bool long_press = false;
/*******************************************************************
 *                         function declaration     
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n);
static void Print_Btn_Info(Button_t *btn);
static void Add_Button(Button_t *btn);

/************************************************************
  * @brief   Button creation
	* @param   name : Button name
	* @param   btn : Button structure
  * @param   read_btn_level : Key level reading function，Users need to implement return by themselvesuint8_ttype level
  * @param   btn_trigger_level : Key trigger level
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

  // memset(btn, 0, sizeof(struct button)); //Clear the structure information, it is recommended that the user clear it before

  StrnCopy(btn->Name, name, BTN_NAME_MAX); /* Create button name */

  btn->Button_State = NONE_TRIGGER;                  //Button status
  btn->Button_Last_State = NONE_TRIGGER;             //Last button status
  btn->Button_Trigger_Event = NONE_TRIGGER;          //Button trigger event
  btn->Read_Button_Level = read_btn_level;           //Button reading level function
  btn->Button_Trigger_Level = btn_trigger_level;     //Key trigger level
  btn->Button_Last_Level = btn->Read_Button_Level(); //Current level of button
  btn->Debounce_Time = 0;

  printf("Button create success!\r\n");

  Add_Button(btn); //Added to singly linked list when created

  Print_Btn_Info(btn); //Print information
}

/************************************************************
  * @brief The key trigger event is linked to the callback function mapping
	* @param btn: button structure
	* @param btn_event: key trigger event
  * @param btn_callback: The callback processing function after the key is triggered. Requires user implementation
  * @return NULL
  * @author jiejie
  * @github https://github.com/jiejieTop
  * @date 2018-xx-xx
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
      btn->CallBack_Function[i] = btn_callback; //Callback function triggered by key event, used to handle key event
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //Callback function triggered by key event, used to handle key event
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
  * @brief Get the event triggered by key press
	* @param NULL
  * @return NULL
  * @author jiejie
  * @github https://github.com/jiejieTop
  * @date 2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Get_Button_EventInfo(Button_t *btn)
{
  //Callback function triggered by key event, used to handle key event
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
  * @brief Get the event triggered by key press
	* @param NULL
  * @return NULL
  * @author jiejie
  * @github https://github.com/jiejieTop
  * @date 2018-xx-xx
  * @version v1.0
  ***********************************************************/
uint8_t Get_Button_State(Button_t *btn)
{
  return (uint8_t)(btn->Button_State);
}

/************************************************************
  * @brief   Key cycle processing function
  * @param   btn:Processed keys
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    This function must be called at a certain period，The recommended period is20~50ms
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
  uint8_t current_level = (uint8_t)btn->Read_Button_Level(); //Get the current key level
  //Key level changes
  if ((current_level != btn->Button_Last_Level) && (++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME))
  {
    btn->Button_Last_Level = current_level; //Update current key level
    btn->Debounce_Time = 0;                 //Confirmed by pressing

    //If the previous state was not pressed, change the button state to pressed (first press of a double click)
    if (((btn->Button_State == NONE_TRIGGER) || (btn->Button_State == BUTTON_DOUBLE)))
    {
      btn->Button_State = BUTTON_DOWN;
    }
    //Release button
    else if (btn->Button_State == BUTTON_DOWN)
    {
      if (!long_press)
      {

        btn->Button_State = BUTTON_UP;
        TRIGGER_CB(BUTTON_UP); // trigger release
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
    if (btn->Button_Last_Level == btn->Button_Trigger_Level) //button pressed
    {
#if CONTINUOS_TRIGGER //Support continuous triggering

      if (++(btn->Button_Cycle) >= BUTTON_CONTINUOS_CYCLE)
      {
        btn->Button_Cycle = 0;
        btn->Button_Trigger_Event = BUTTON_CONTINUOS;
        TRIGGER_CB(BUTTON_CONTINUOS); //Double click
        //USART3_Put_String("lianxu button !\r\n");
      }

#else

      btn->Button_Trigger_Event = BUTTON_DOWN;

      if (++(btn->Long_Time) >= BUTTON_LONG_TIME) //Update the trigger event to long press before releasing the button
      {
#if LONG_FREE_TRIGGER

        btn->Button_Trigger_Event = BUTTON_LONG;

#elif LONG_FREE_ENABLE
        if (long_press == false)
        {

          TRIGGER_CB(BUTTON_LONG); //Long press
          long_press = true;
          btn->Button_Trigger_Event = BUTTON_LONG_FREE;
        }

#else

        if (++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE) //Period of continuous long-press triggering
        {
          btn->Button_Cycle = 0;
          btn->Button_Trigger_Event = BUTTON_LONG;
          TRIGGER_CB(BUTTON_LONG); //Long press
        }
#endif

        if (btn->Long_Time == 0xFF) //Update time overflow
        {
          btn->Long_Time = BUTTON_LONG_TIME;
        }
        //USART3_Put_String("long down count!\r\n");
      }

#endif
    }

    break;
  }

  case BUTTON_UP: // Pop up state
  {
    if (btn->Button_Trigger_Event == BUTTON_DOWN) //Trigger click
    {
      if ((btn->Timer_Count <= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State == BUTTON_DOUBLE)) // double click
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
        btn->Long_Time = 0; //Failed to detect long press, cleared to 0

#if (SINGLE_AND_DOUBLE_TRIGGER == 0)
        TRIGGER_CB(BUTTON_DOWN); //click
#endif
        btn->Button_State = BUTTON_DOUBLE;
        btn->Button_Last_State = BUTTON_DOUBLE;
      }
    }
#if !LONG_FREE_ENABLE
    else if (btn->Button_Trigger_Event == BUTTON_LONG)
    {
#if LONG_FREE_TRIGGER
      TRIGGER_CB(BUTTON_LONG); //Long press

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

      TRIGGER_CB(BUTTON_LONG_FREE); //Long press to release

#endif
      btn->Long_Time = 0;
      btn->Button_State = NONE_TRIGGER;
      long_press = false;
      btn->Button_Last_State = BUTTON_LONG_FREE;
      //USART3_Put_String("long_free click button !\r\n");
    }
#if CONTINUOS_TRIGGER
    else if (btn->Button_Trigger_Event == BUTTON_CONTINUOS) //Double click
    {
      btn->Long_Time = 0;
      TRIGGER_CB(BUTTON_CONTINUOS_FREE); //Burst release
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = BUTTON_CONTINUOS;
    }
#endif

    break;
  }

  case BUTTON_DOUBLE:
  {
    btn->Timer_Count++; //time record
    if (btn->Timer_Count >= BUTTON_DOUBLE_TIME)
    {
      btn->Button_State = NONE_TRIGGER;
      btn->Button_Last_State = NONE_TRIGGER;
    }
#if SINGLE_AND_DOUBLE_TRIGGER

    if ((btn->Timer_Count >= BUTTON_DOUBLE_TIME) && (btn->Button_Last_State != BUTTON_DOWN))
    {
      btn->Timer_Count = 0;
      TRIGGER_CB(BUTTON_DOWN); //click
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
  * @brief Scan keys in a traversal manner without losing each key
	* @param NULL
  * @return NULL
  * @author jiejie
  * @github https://github.com/jiejieTop
  * @date 2018-xx-xx
  * @version v1.0
  * @note This function needs to be called periodically. It is recommended to call it every 20-50ms.
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
  * @brief   Traverse keys
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

/**************************** The following is the internal calling function ********************/

/************************************************************
  * @brief Copy the specified length string
	* @param NULL
  * @return NULL
  * @author jiejie
  * @github https://github.com/jiejieTop
  * @date 2018-xx-xx
  * @version v1.0
  * @note NULL
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
  * @brief   Print key related information
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
  * @brief   Use a singly linked list to connect keys
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