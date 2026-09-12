#ifndef PAGE_2048_SNAKE_H
#define PAGE_2048_SNAKE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
* INCLUDES
*********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#include "../../lv_ex_conf.h"
#endif


/*********************
 *      DEFINES
 *********************/
#define KSDIY_GAME_SNAKE_TITLE_SPACE   (50)
#define KSDIY_GAME_SNAKE_INIT_LINE     (3)
#define KSDIY_GAME_SNAKE_MAX_LINE      (240)
#define KSDIY_GAME_SNAKE_SIZE          (20)
//#define KSDIY_GAME_SNAKE_EYE_SIZE    (5)
#define KSDIY_GAME_SNAKE_SPEED         (10)
#define KSDIY_GAME_SNAKE_SPACE         (KSDIY_GAME_SNAKE_SIZE - KSDIY_GAME_SNAKE_SPEED)

#define KSDIY_GAME_SNAKE_HEAD_COLOR    (LV_COLOR_YELLOW)  		// Snake head color
#define KSDIY_GAME_SNAKE_BODY_COLOR    (LV_COLOR_BLACK)  		// Snake body color
#define KSDIY_GAME_SNAKE_RADIUS        (15)   					// Snake body corner radius setting
#define KSDIY_GAME_FOOD_RADIUS         (50)   					// Food corner radius setting
#define KSDIY_GAME_FOOD_COLOR          (LV_COLOR_PURPLE) 	    // Food color
#define KSDIY_GAME_FOOD_SIZE           (KSDIY_GAME_SNAKE_SIZE)
#define KSDIY_GAME_FOOD_CHECK_SPACE    (10)
#define KSDIY_GAME_FOOD_MAX_HOR        (LV_HOR_RES - KSDIY_GAME_SNAKE_SIZE)
#define KSDIY_GAME_FOOD_MAX_VER        (LV_VER_RES - KSDIY_GAME_SNAKE_TITLE_SPACE)
#define KSDIY_GAME_SNAKE_USE_KEY       (1)  					// Use button control


/**********************
 *      TYPEDEFS
 **********************/
// Game data
typedef struct _KSDIY_snake {
	lv_obj_t  * bg;				// Background
	lv_obj_t  * obj_food;		// Food handle
	lv_obj_t  * label_info;		// Game info
	lv_task_t * task_handle; 	// Task handle
	uint8_t	sroce;				// Score
	uint8_t len;				// Length
	uint8_t gesture;			// Snake head direction flag
} T_KSDIY_snake, *PT_KSDIY_snake;

// Snake body data
typedef struct Link{
	lv_obj_t *obj;            	// Snake body
	lv_coord_t x;             	// x coordinate
	lv_coord_t y;             	// y coordinate
	struct Link *prior;       	// Points to the direct predecessor
	struct Link *next;        	// Points to the direct successor
}link_snake, p_link_snake;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void page_game_snake(void);

void move_task_game_snake(uint8_t move);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PAGE_2048_GAME_H */





