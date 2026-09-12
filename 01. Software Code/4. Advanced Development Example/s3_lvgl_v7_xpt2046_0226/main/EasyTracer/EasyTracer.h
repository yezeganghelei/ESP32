/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-19 12:47:45
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-06-25 14:00:14
 * @FilePath     : \esp-idf\KSDIY_ESPCAM\main\EasyTracer.h
 */
#ifndef EASY_TRACER_H
#define EASY_TRACER_H

#define IMG_X 0	  // Image x coordinate
#define IMG_Y 0	  // Image y coordinate
#define IMG_W 240 // Image width
#define IMG_H 240 // Image height

#define ALLOW_FAIL_PER 3 // Fault tolerance: one bad point is allowed every 1<<ALLOW_FAIL_PER points; a larger tolerance makes recognition easier but increases the error rate
#define ITERATE_NUM    8 // Iteration count; more iterations make recognition more accurate but increase computation

typedef struct{
    unsigned char  H_MIN;// Target minimum hue
    unsigned char  H_MAX;// Target maximum hue	
    
	unsigned char  S_MIN;// Target minimum saturation  
    unsigned char  S_MAX;// Target maximum saturation
	
	unsigned char  L_MIN;// Target minimum luminance  
    unsigned char  L_MAX;// Target maximum luminance
	
	unsigned int  WIDTH_MIN;// Target minimum width
	unsigned int  HIGHT_MIN;// Target minimum height

	unsigned int  WIDTH_MAX;// Target maximum width
	unsigned int  HIGHT_MAX;// Target maximum height

}TARGET_CONDI;// Target condition for judgment

typedef struct{
	unsigned int x;// Target x coordinate
	unsigned int y;// Target y coordinate
	unsigned int w;// Target width
	unsigned int h;// Target height
}RESULT;// Recognition result

// The only API; the user writes the recognition condition into the struct pointed to by Condition, and the function returns the target x/y coordinates, width and height
// Returns 1 on successful recognition, 1 on failure
int Trace(const TARGET_CONDI *Condition,RESULT *Resu);

#endif
