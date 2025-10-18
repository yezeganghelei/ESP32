/*
 * @Descripttion : 
 * @version      : 
 * @Author       : Kevincoooool
 * @Date         : 2021-03-25 17:15:11
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-01-06 11:20:00
 * @FilePath: \S3_DEMO\16.Test\main\button\button.c
 */

#include "button.h"
#include "bsp_adc.h"
/*******************************************************************
 *                          variable declaration                               
 *******************************************************************/

Button_t Button_UP;
Button_t Button_ENTER;
Button_t Button_DOWN;
Button_t Button_RIGHT;
Button_t Button_LEFT;

uint8_t Button_Value = BT_NONE;
char * Button_Tips[]={
	"No Button!",
	"Button 1 down!",
	"Button 1 Double!",
	"Button 1 Long!",
	"Button 1 Long_Free!",
	
	"Button 2 down!",
	"Button 2 Double!",
	"Button 2 Long!",
	"Button 2 Long_Free!",

	"Button 3 down!",
	"Button 3 Double!",
	"Button 3 Long!",
	"Button 3 Long_Free!",

	"Button 4 down!",
	"Button 4 Double!",
	"Button 4 Long!",
	"Button 4 Long_Free!",

	"Button 5 down!",
	"Button 5 Double!",
	"Button 5 Long!",
	"Button 5 Long_Free!",
};
//One click of the button
void Btn1_Down_CallBack(void *btn)
{

	Button_Value = BT1_DOWN;
	printf("Button 1 down!\n");

}
//Double click of button
void Btn1_Double_CallBack(void *btn)
{
	Button_Value = BT1_DOUBLE;
	printf("Button 1 Double!\n");
}
//Double click of button
void Btn1_Long_CallBack(void *btn)
{
	Button_Value = BT1_LONG;
	printf("Button 1 Long!\n");
}
void Btn1_Long_Free_CallBack(void *btn)
{
	Button_Value = BT1_LONGFREE;
	printf("Button 1 Long_Free!\n");
}
///////////////////////////////////////////////////////
//按键二click
void Btn2_Down_CallBack(void *btn)
{
	Button_Value = BT2_DOWN;
	printf("Button 2 down!\n");
}
//Double click of button
void Btn2_Double_CallBack(void *btn)
{
	Button_Value = BT2_DOUBLE;
	printf("Button 2 Double!\n");
}
//Double click of button
void Btn2_Long_CallBack(void *btn)
{
	Button_Value = BT2_LONG;
	printf("Button 2Long!\n");
}
void Btn2_Long_Free_CallBack(void *btn)
{
	Button_Value = BT2_LONGFREE;
	printf("Button 2 Long_Free!\n");
}
///////////////////////////////////////////////////////
//Three clicks of button
void Btn3_Down_CallBack(void *btn)
{
	Button_Value = BT3_DOWN;
	printf("Button 3 down!\n");
}
//Double click of button
void Btn3_Double_CallBack(void *btn)
{
	Button_Value = BT3_DOUBLE;
	printf("Button 3 Double!\n");
}
//Double click of button
void Btn3_Long_CallBack(void *btn)
{
	Button_Value = BT3_LONG;
	printf("Button 3 Long!\n");
}
void Btn3_Long_Free_CallBack(void *btn)
{
	Button_Value = BT3_LONGFREE;
	printf("Button 3 Long_Free!\n");
}

///////////////////////////////////////////////////////
//Three clicks of button
void Btn4_Down_CallBack(void *btn)
{
	Button_Value = BT4_DOWN;
	printf("Button 4 down!\n");
}
//Double click of button
void Btn4_Double_CallBack(void *btn)
{
	Button_Value = BT4_DOUBLE;
	printf("Button 4 Double!\n");
}
//Double click of button
void Btn4_Long_CallBack(void *btn)
{
	Button_Value = BT4_LONG;
	printf("Button 4 Long!\n");
}
void Btn4_Long_Free_CallBack(void *btn)
{
	Button_Value = BT4_LONGFREE;
	printf("Button 4 Long_Free!\n");
}
///////////////////////////////////////////////////////
//Three clicks of button
void Btn5_Down_CallBack(void *btn)
{
	Button_Value = BT5_DOWN;
	printf("Button 5 down!\n");
}
//Double click of button
void Btn5_Double_CallBack(void *btn)
{
	Button_Value = BT5_DOUBLE;
	printf("Button 5 Double!\n");
}
//Double click of button
void Btn5_Long_CallBack(void *btn)
{
	Button_Value = BT5_LONG;
	printf("Button 5 Long!\n");
}
void Btn5_Long_Free_CallBack(void *btn)
{
	Button_Value = BT5_LONGFREE;
	printf("Button 5 Long_Free!\n");
}

uint8_t Read_KEY1_Level(void)
{
	return (Get_Adc_Button() == 3) ? 1 : 0;
}

uint8_t Read_KEY2_Level(void)
{
	return (Get_Adc_Button() == 1) ? 1 : 0;
}
uint8_t Read_KEY3_Level(void)
{
	return (Get_Adc_Button() == 2) ? 1 : 0;
}
uint8_t Read_KEY4_Level(void)
{
	return (Get_Adc_Button() == 4) ? 1 : 0;
}
uint8_t Read_KEY5_Level(void)
{
	return (Get_Adc_Button() == 5) ? 1 : 0;
}
void Button_Init(void)
{

	Button_Create("Button_UP",
				  &Button_UP,
				  Read_KEY1_Level,
				  KEY_ON);
	Button_Attach(&Button_UP, BUTTON_DOWN, Btn1_Down_CallBack);			//click
	Button_Attach(&Button_UP, BUTTON_DOUBLE, Btn1_Double_CallBack);		//double click
	Button_Attach(&Button_UP, BUTTON_LONG, Btn1_Long_CallBack);			//long press press
	Button_Attach(&Button_UP, BUTTON_LONG_FREE, Btn1_Long_Free_CallBack); //Long press and release

	Button_Create("Button_ENTER",
				  &Button_ENTER,
				  Read_KEY2_Level,
				  KEY_ON);
	Button_Attach(&Button_ENTER, BUTTON_DOWN, Btn2_Down_CallBack);			//click
	Button_Attach(&Button_ENTER, BUTTON_DOUBLE, Btn2_Double_CallBack);		//double click
	Button_Attach(&Button_ENTER, BUTTON_LONG, Btn2_Long_CallBack);			//long press press
	Button_Attach(&Button_ENTER, BUTTON_LONG_FREE, Btn2_Long_Free_CallBack); //Long press and release

	Button_Create("Button_DOWN",
				  &Button_DOWN,
				  Read_KEY3_Level,
				  KEY_ON);
	Button_Attach(&Button_DOWN, BUTTON_DOWN, Btn3_Down_CallBack);			//click
	Button_Attach(&Button_DOWN, BUTTON_DOUBLE, Btn3_Double_CallBack);		//double click
	Button_Attach(&Button_DOWN, BUTTON_LONG, Btn3_Long_CallBack);			//long press press
	Button_Attach(&Button_DOWN, BUTTON_LONG_FREE, Btn3_Long_Free_CallBack); //Long press and release

	Button_Create("Button_RIGHT",
				  &Button_RIGHT,
				  Read_KEY4_Level,
				  KEY_ON);
	Button_Attach(&Button_RIGHT, BUTTON_DOWN, Btn4_Down_CallBack);			//click
	Button_Attach(&Button_RIGHT, BUTTON_DOUBLE, Btn4_Double_CallBack);		//double click
	Button_Attach(&Button_RIGHT, BUTTON_LONG, Btn4_Long_CallBack);			//long press press
	Button_Attach(&Button_RIGHT, BUTTON_LONG_FREE, Btn4_Long_Free_CallBack); //Long press and release

	Button_Create("Button_LEFT",
				  &Button_LEFT,
				  Read_KEY5_Level,
				  KEY_ON);
	Button_Attach(&Button_LEFT, BUTTON_DOWN, Btn5_Down_CallBack);			//click
	Button_Attach(&Button_LEFT, BUTTON_DOUBLE, Btn5_Double_CallBack);		//double click
	Button_Attach(&Button_LEFT, BUTTON_LONG, Btn5_Long_CallBack);			//long press press
	Button_Attach(&Button_LEFT, BUTTON_LONG_FREE, Btn5_Long_Free_CallBack); //Long press and release

	Get_Button_Event(&Button_UP);
	Get_Button_Event(&Button_ENTER);
	Get_Button_Event(&Button_DOWN);
    Get_Button_Event(&Button_RIGHT);
    Get_Button_Event(&Button_LEFT);
}