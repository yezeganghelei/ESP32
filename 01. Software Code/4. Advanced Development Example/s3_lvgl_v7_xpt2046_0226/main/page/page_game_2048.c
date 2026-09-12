/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-15 10:53:21
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-13 17:15:54
 * @FilePath     : \s3_lvgl_v7\main\page\page_game_2048.c
 */
/**
 ******************************************************************************
 * @file    PAGE_game_2048.c
 * @author  100ask
 * @version V1.2
 * @date    2020-12-12
 * @brief	2048 game
 ******************************************************************************
 * Change Logs:
 * Date           Author          Notes
 * 2020-12-12     zhouyuebiao     First version
 * 2021-01-25     zhouyuebiao     V1.2 
 ******************************************************************************
 * @attention
 *
 * Copyright (C) 2008-2021 Shenzhen Baiwen Network Technology Co., Ltd. <https://www.100ask.net/>
 * All rights reserved
 *
 ******************************************************************************
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "button.h"
#include "page_game_2048.h"
#include "app_anim.h"

#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"
LV_FONT_DECLARE(myFont);
/**********************
 *  STATIC VARIABLES
 **********************/
static PT_PAGE_2048_game g_pt_PAGE_2048_game; // Data structure

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void PAGE_game_2048_init(void);												// Screen initialization
static void PAGE_game_2048_init_board(lv_obj_t *parent, uint8_t board[SIZE][SIZE]); // Initialize the board and render the main screen
static void PAGE_game_2048_init_board_number(uint8_t board[SIZE][SIZE]);			// Initialize the tile data
static void PAGE_game_2048_draw_board(lv_obj_t *parent, char *number,
									  uint16_t canvas_w, uint16_t canvas_h,
									  uint16_t rect_dsc_x, uint16_t rect_dsc_y,
									  lv_color_t rgb32);								 // Fill and draw a tile
static void PAGE_game_2048_add_random(uint8_t board[SIZE][SIZE]);						 // Initialize a random value at a random position in the 2D array
static void PAGE_game_2048_rotate_board(uint8_t board[SIZE][SIZE]);						 // Rotate the matrix
static bool PAGE_game_2048_slide_array(uint8_t array[SIZE]);							 // Check whether the array still has room
static bool PAGE_game_2048_move_up(uint8_t board[SIZE][SIZE]);							 // Check whether an upward move is possible (touchscreen control)
static bool PAGE_game_2048_move_left(uint8_t board[SIZE][SIZE]);						 // Check whether a left move is possible (touchscreen control)
static bool PAGE_game_2048_move_down(uint8_t board[SIZE][SIZE]);						 // Check whether a downward move is possible (touchscreen control)
static bool PAGE_game_2048_move_right(uint8_t board[SIZE][SIZE]);						 // Check whether a right move is possible (touchscreen control)
static bool PAGE_game_2048_find_pair_down(uint8_t board[SIZE][SIZE]);					 // Check whether a right move is possible
static bool PAGE_game_2048_game_ended(uint8_t board[SIZE][SIZE]);						 // Determine whether the game should end
static char *PAGE_game_2048_int_to_string(int num, char *str);							 // Convert an integer value to a string
static lv_color_t PAGE_game_2048_num_to_color(uint32_t num);							 // Set the color corresponding to a number
static uint8_t PAGE_game_2048_find_target(uint8_t array[SIZE], uint8_t x, uint8_t stop); // Check whether sliding should stop to avoid a double merge
static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE]);					 // Find whether the 2D array has values that can be merged (equal values can merge)
// Tasks
static void PAGE_game_2048_game_key_up(void);	 // Button-controlled move up (button control)
static void PAGE_game_2048_game_key_down(void);	 // Button-controlled move down (button control)
static void PAGE_game_2048_game_key_left(void);	 // Button-controlled move left (button control)
static void PAGE_game_2048_game_key_right(void); // Button-controlled move right (button control)
// Touchscreen detection event
static void event_handler_play_2048(lv_obj_t *obj, lv_event_t event); // Touchscreen detection event handler (up, down, left, right swipes)
/*
 *  Function:    void PAGE_ _tool_widget_test(void)
 *  Input:      None
 *  Return:     None
 *  Purpose:   Application entry
*/
void PAGE_game_2048(void)
{
	/* Allocate memory */
	g_pt_PAGE_2048_game = (T_PAGE_2048_game *)malloc(sizeof(T_PAGE_2048_game));

	/* Initialize the desktop background */
	g_pt_PAGE_2048_game->bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(g_pt_PAGE_2048_game->bg, LV_HOR_RES, LV_VER_RES);
	lv_obj_set_y(g_pt_PAGE_2048_game->bg, 0);

	/* Initialize the game stage */
	g_pt_PAGE_2048_game->canvas_stage = lv_obj_create(g_pt_PAGE_2048_game->bg, NULL);
	lv_obj_set_style_local_radius(g_pt_PAGE_2048_game->canvas_stage, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);						  // Set corner radius
	lv_obj_set_style_local_bg_color(g_pt_PAGE_2048_game->canvas_stage, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, PAGE_2048_GAME_BOX_COLOR); //Set color
	lv_obj_set_size(g_pt_PAGE_2048_game->canvas_stage, PAGE_2048_GAME_BOX_W, PAGE_2048_GAME_BOX_H);
	lv_obj_align(g_pt_PAGE_2048_game->canvas_stage, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

	/* Initialize the main screen */
	g_pt_PAGE_2048_game->play_game = true;
	g_pt_PAGE_2048_game->current_score_value = 0;
	PAGE_game_2048_init();
	/* Assign the screen touch event handler */
	lv_obj_set_click(lv_layer_top(), true);
	lv_obj_set_event_cb(lv_layer_top(), event_handler_play_2048);
}

/*
 *  Function:    static void PAGE_ _tool_music_player_init(void)
 *  Input:      None
 *  Return:     None
 *  Purpose:   Application screen initialization
*/
static void PAGE_game_2048_init(void)
{

	/* Best score display area */
	lv_obj_t *obj_best_source = lv_obj_create(g_pt_PAGE_2048_game->bg, NULL);
	lv_obj_set_size(obj_best_source, 65, 35);
	lv_obj_align(obj_best_source, g_pt_PAGE_2048_game->canvas_stage, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);

	/* Best score title */
	lv_obj_t *best_source_tip_text = lv_label_create(obj_best_source, NULL); /* Create label */
	lv_label_set_text(best_source_tip_text, "BEST");
	lv_obj_align(best_source_tip_text, NULL, LV_ALIGN_IN_TOP_MID, 0, 0); /* Center */

	/* Best score value */
	g_pt_PAGE_2048_game->label_best_score = lv_label_create(obj_best_source, NULL); /* Create label */
	lv_label_set_text(g_pt_PAGE_2048_game->label_best_score, "1024");
	lv_obj_align(g_pt_PAGE_2048_game->label_best_score, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0); /* Center */

	/* Current score display area */
	lv_obj_t *obj_current_source = lv_obj_create(g_pt_PAGE_2048_game->bg, NULL);
	lv_obj_set_size(obj_current_source, 65, 35);
	lv_obj_align(obj_current_source, g_pt_PAGE_2048_game->canvas_stage, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

	/* Current score label */
	lv_obj_t *current_source_tip_text = lv_label_create(obj_current_source, NULL); /* Create label */
	lv_label_set_text(current_source_tip_text, "SORCE");
	lv_obj_align(current_source_tip_text, NULL, LV_ALIGN_IN_TOP_MID, 0, 0); /* Center */

	/* Current score */
	g_pt_PAGE_2048_game->label_current_score = lv_label_create(obj_current_source, NULL); /* Create label */
	lv_label_set_text(g_pt_PAGE_2048_game->label_current_score, "0");
	lv_obj_align(g_pt_PAGE_2048_game->label_current_score, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0); /* Center on the current canvas */

	/* Initialize the game numbers */
	PAGE_game_2048_init_board_number(g_pt_PAGE_2048_game->game_board);

	/* Initialize each tile in the game stage */
	PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
}

/*
 *  Function:    static void PAGE_game_2048_init_board(lv_obj_t * parent, uint8_t board[SIZE][SIZE])
 *  Input:     parent object
 *  Return:    array to operate on
 *  Purpose:   Initialize the board and render the main screen
*/
static void PAGE_game_2048_init_board(lv_obj_t *parent, uint8_t board[SIZE][SIZE])
{
	/* Clear all previous child objects */
	lv_obj_clean(parent);

	/* Current score */
	char str_current_score[8] = {0};
	lv_label_set_text(g_pt_PAGE_2048_game->label_current_score, PAGE_game_2048_int_to_string(g_pt_PAGE_2048_game->current_score_value, str_current_score));

	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			if (board[x][y] != 0)
			{
				//sprintf(tmp_char, "%d", board[x][y]);
				char tmp_char[8];
				snprintf(tmp_char, 8, "%u", (uint32_t)1 << board[x][y]);
				PAGE_game_2048_draw_board(parent, tmp_char,
										  PAGE_2048_NUMBER_BOX_W, PAGE_2048_NUMBER_BOX_H,
										  (8 * (y + 1)) + (PAGE_2048_NUMBER_BOX_W * (y + 1 - 1)), (8 * (x + 1)) + (PAGE_2048_NUMBER_BOX_W * (x + 1 - 1)), PAGE_game_2048_num_to_color((uint32_t)1 << board[x][y]));
			}
			else
			{
				PAGE_game_2048_draw_board(parent, "",
										  PAGE_2048_NUMBER_BOX_W, PAGE_2048_NUMBER_BOX_H,
										  (8 * (y + 1)) + (PAGE_2048_NUMBER_BOX_W * (y + 1 - 1)), (8 * (x + 1)) + (PAGE_2048_NUMBER_BOX_W * (x + 1 - 1)), PAGE_2048_NUMBER_BOX_COLOR);
			}
		}
	}
}

/*
 *  Function:    static void PAGE_game_2048_draw_board(lv_obj_t * parent, char * number, \
														 uint16_t canvas_w, uint16_t canvas_h, \
														 uint16_t rect_dsc_x, uint16_t rect_dsc_y, \
														 lv_color_t rgb32)
 *  Input:     parent object to draw on
 *  Input:     value to display
 *  Input:     width
 *  Input:     height
 *  Input:     x-axis offset
 *  Input:     y-axis offset
 *  Input:     tile background color
 *  Return:     None
 *  Purpose:   Fill and draw a tile
*/
static void PAGE_game_2048_draw_board(lv_obj_t *parent, char *number,
									  uint16_t canvas_w, uint16_t canvas_h,
									  uint16_t rect_dsc_x, uint16_t rect_dsc_y,
									  lv_color_t rgb32)
{
	/* Create tile */
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_size(obj, canvas_w, canvas_h);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, rgb32);
	//lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);			  // Set corner radius
	lv_obj_set_style_local_border_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0); // Border transparency
	lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &myFont);	  // Font size

	lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_LEFT, rect_dsc_x, rect_dsc_y);

	/* Number */
	lv_obj_t *icon = lv_label_create(obj, NULL); /* Create label */

	lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_obj_set_style_local_text_font(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_text(icon, number);

	lv_obj_align(icon, NULL, LV_ALIGN_CENTER, 0, 0); /* Center on the current canvas */
}

/*
 *  Function:    static char* PAGE_game_2048_int_to_string(int num, char *str)
 *  Input:     integer value to convert
 *  Return:    converted string
 *  Purpose:   Convert an integer value to a string
*/
static char *PAGE_game_2048_int_to_string(int num, char *str)
{
	int i = 0;	 //Index for filling str
	if (num < 0) //If num is negative, make it positive
	{
		num = -num;
		str[i++] = '-';
	}
	//Convert
	do
	{
		str[i++] = num % 10 + 48; //Take the least significant digit of num; ASCII codes for characters '0'-'9' are 48-57; simply put, digit 0 + 48 = 48, whose ASCII code corresponds to the character '0'
		num /= 10;				  //Remove the least significant digit
	} while (num);				  //Continue looping while num is not 0

	str[i] = '\0';

	//Determine the starting position for adjustment
	int j = 0;
	if (str[0] == '-') //If there is a minus sign, it does not need to be adjusted
	{
		j = 1; //Start adjusting from the second character
		++i;   //Because of the minus sign, the symmetry axis for swapping also shifts back by one
	}
	//Symmetric swap
	for (; j < i / 2; j++)
	{
		//Symmetric swap of the two ends; this swaps a and b without a temporary variable: a=a+b; b=a-b; a=a-b;
		str[j] = str[j] + str[i - 1 - j];
		str[i - 1 - j] = str[j] - str[i - 1 - j];
		str[j] = str[j] - str[i - 1 - j];
	}

	return str; //Return the converted value
}

/*
 *  Function:    static void PAGE_game_2048_init_board_number(uint8_t board[SIZE][SIZE])
 *  Input:     array to initialize
 *  Return:     None
 *  Purpose:   Initialize the 2D array values (all set to 0)
*/
static void PAGE_game_2048_init_board_number(uint8_t board[SIZE][SIZE])
{
	uint8_t x, y;
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			board[x][y] = 0;
		}
	}

	/* Initialize random values at two random positions */
	PAGE_game_2048_add_random(board);
	PAGE_game_2048_add_random(board);

	g_pt_PAGE_2048_game->current_score_value = 0;
}

//__use_no_semihosting was requested, but _ttywrch was
// void time(int ch)
// {
// 	ch = ch;
// }

/*
 *  Function:    static void PAGE_game_2048_add_random(uint8_t board[SIZE][SIZE])
 *  Input:     array in which to initialize a random value at a random position
 *  Return:     None
 *  Purpose:   Initialize a random value at a random position in the 2D array
*/
static void PAGE_game_2048_add_random(uint8_t board[SIZE][SIZE])
{
	static bool initialized = false;
	uint8_t x, y;
	uint8_t r, len = 0;
	uint8_t n, list[SIZE * SIZE][2];

	if (!initialized)
	{
		srand(20);
		initialized = true;
	}

	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			if (board[x][y] == 0)
			{
				list[len][0] = x;
				list[len][1] = y;
				len++;
			}
		}
	}

	if (len > 0)
	{
		r = rand() % len;
		x = list[r][0];
		y = list[r][1];
		n = (rand() % 10) / 9 + 1;
		board[x][y] = n;
	}
}

/*
 *  Function:    static lv_color_t PAGE_game_2048_num_to_color(uint32_t num)
 *  Input:     number whose color is to be set
 *  Return:    color value
 *  Purpose:   Set the color corresponding to a number
*/
static lv_color_t PAGE_game_2048_num_to_color(uint32_t num)
{
	switch (num)
	{
	case 2:
		return PAGE_2048_NUMBER_2_COLOR;
	case 4:
		return PAGE_2048_NUMBER_4_COLOR;
	case 8:
		return PAGE_2048_NUMBER_8_COLOR;
	case 16:
		return PAGE_2048_NUMBER_16_COLOR;
	case 32:
		return PAGE_2048_NUMBER_32_COLOR;
	case 64:
		return PAGE_2048_NUMBER_64_COLOR;
	case 128:
		return PAGE_2048_NUMBER_128_COLOR;
	case 256:
		return PAGE_2048_NUMBER_256_COLOR;
	case 512:
		return PAGE_2048_NUMBER_512_COLOR;
	case 1024:
		return PAGE_2048_NUMBER_1024_COLOR;
	case 2048:
		return PAGE_2048_NUMBER_2048_COLOR;
	default:
		return PAGE_2048_NUMBER_2048_COLOR;
	}
}

/*
 *  Function:    static uint8_t PAGE_game_2048_find_target(uint8_t array[SIZE],uint8_t x,uint8_t stop) 
 *  Input:     2D array to check
 *  Return:    target position
 *  Purpose:   Check whether sliding should stop to avoid a double merge
*/
static uint8_t PAGE_game_2048_find_target(uint8_t array[SIZE], uint8_t x, uint8_t stop)
{
	uint8_t t;
	// if the position is already on the first, don't evaluate
	if (x == 0)
	{
		return x;
	}
	for (t = (x - 1);; t--)
	{
		if (array[t] != 0)
		{
			if (array[t] != array[x])
			{
				// merge is not possible, take next position
				return (t + 1);
			}
			return t;
		}
		else
		{
			// we should not slide further, return this one
			if (t == stop)
			{
				return t;
			}
		}
	}
	// we did not find a
	return x;
}

/*
 *  Function:    static bool PAGE_game_2048_slide_array(uint8_t array[SIZE]) 
 *  Input:     array to check
 *  Return:    true - space available; false - no free space
 *  Purpose:   Check whether the array still has room
*/
static bool PAGE_game_2048_slide_array(uint8_t array[SIZE])
{
	bool success = false;
	uint8_t x, t, stop = 0;

	for (x = 0; x < SIZE; x++)
	{
		if (array[x] != 0)
		{
			t = PAGE_game_2048_find_target(array, x, stop);
			// if target is not original position, then move or merge
			if (t != x)
			{
				// if target is zero, this is a move
				if (array[t] == 0)
				{
					array[t] = array[x];
				}
				else if (array[t] == array[x])
				{
					// merge (increase power of two)
					array[t]++;
					// increase score
					g_pt_PAGE_2048_game->current_score_value += (uint32_t)1 << array[t];
					// set stop to avoid double merge
					stop = t + 1;
				}
				array[x] = 0;
				success = true;
			}
		}
	}
	return success;
}

/*
 *  Function:    static bool PAGE_game_2048_rotate_board(uint8_t board[SIZE][SIZE]) 
 *  Input:     matrix to rotate
 *  Return:     None
 *  Purpose:   Rotate the matrix
*/
static void PAGE_game_2048_rotate_board(uint8_t board[SIZE][SIZE])
{
	uint8_t i, j, n = SIZE;
	uint8_t tmp;
	for (i = 0; i < n / 2; i++)
	{
		for (j = i; j < n - i - 1; j++)
		{
			tmp = board[i][j];
			board[i][j] = board[j][n - i - 1];
			board[j][n - i - 1] = board[n - i - 1][n - j - 1];
			board[n - i - 1][n - j - 1] = board[n - j - 1][i];
			board[n - j - 1][i] = tmp;
		}
	}
}

/*
 *  Function:    static bool PAGE_game_2048_move_up(uint8_t board[SIZE][SIZE]) 
 *  Input:     2D array to check
 *  Return:    true - can move; false - cannot move
 *  Purpose:   Check whether an upward move is possible
*/
static bool PAGE_game_2048_move_up(uint8_t board[SIZE][SIZE])
{
	bool success = false;
	uint8_t x;
	for (x = 0; x < SIZE; x++)
	{
		success |= PAGE_game_2048_slide_array(board[x]);
	}
	return success;
}

/*
 *  Function:    static bool PAGE_game_2048_move_left(uint8_t board[SIZE][SIZE]) 
 *  Input:     2D array to check
 *  Return:    true - can move; false - cannot move
 *  Purpose:   Check whether an upward move is possible
*/
static bool PAGE_game_2048_move_left(uint8_t board[SIZE][SIZE])
{
	bool success;
	PAGE_game_2048_rotate_board(board);
	success = PAGE_game_2048_move_up(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	return success;
}

/*
 *  Function:    static bool PAGE_game_2048_move_down(uint8_t board[SIZE][SIZE]) 
 *  Input:     2D array to check
 *  Return:    true - can move; false - cannot move
 *  Purpose:   Check whether a downward move is possible
*/
static bool PAGE_game_2048_move_down(uint8_t board[SIZE][SIZE])
{
	bool success;
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	success = PAGE_game_2048_move_up(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	return success;
}

/*
 *  Function:    static bool PAGE_game_2048_move_right(uint8_t board[SIZE][SIZE]) 
 *  Input:     2D array to check
 *  Return:    true - can move; false - cannot move
 *  Purpose:   Check whether a right move is possible
*/
static bool PAGE_game_2048_move_right(uint8_t board[SIZE][SIZE])
{
	bool success = false;
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	success = PAGE_game_2048_move_up(board);
	PAGE_game_2048_rotate_board(board);
	return success;
}

/*
 *  Function:    static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE]) 
 *  Input:     2D array to search
 *  Return:    true - has mergeable values; false - has no mergeable values
 *  Purpose:   Find whether the 2D array has values that can be merged (equal values can merge)
*/
static bool PAGE_game_2048_find_pair_down(uint8_t board[SIZE][SIZE])
{
	bool success = false;
	uint8_t x, y;
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE - 1; y++)
		{
			if (board[x][y] == board[x][y + 1])
				return true;
		}
	}
	return success;
}

/*
 *  Function:    static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE]) 
 *  Input:     2D array to count
 *  Return:    number of empty tiles
 *  Purpose:   Count the number of empty tiles
*/
static uint8_t PAGE_game_2048_count_empty(uint8_t board[SIZE][SIZE])
{
	uint8_t x, y;
	uint8_t count = 0;
	for (x = 0; x < SIZE; x++)
	{
		for (y = 0; y < SIZE; y++)
		{
			if (board[x][y] == 0)
			{
				count++;
			}
		}
	}
	return count;
}
static void game_message(const char *str, uint16_t t) //System notification box
{

	lv_obj_t *mbox1 = lv_msgbox_create(g_pt_PAGE_2048_game->bg, NULL); //Global notification box
	lv_obj_set_style_local_text_font(mbox1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_msgbox_set_text(mbox1, str);
	lv_obj_set_size(mbox1, 240, 50);
	lv_msgbox_start_auto_close(mbox1, t);
	lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}
// Game over

/*
 *  Function:    static bool PAGE_game_2048_game_ended(uint8_t board[SIZE][SIZE])
 *  Input:      None
 *  Return:    true - game over, false - game can continue
 *  Purpose:   Determine whether the game should end
*/
static bool PAGE_game_2048_game_ended(uint8_t board[SIZE][SIZE])
{
	bool ended = true;
	if (PAGE_game_2048_count_empty(board) > 0)
		return false;
	if (PAGE_game_2048_find_pair_down(board))
		return false;
	PAGE_game_2048_rotate_board(board);
	if (PAGE_game_2048_find_pair_down(board))
		ended = false;
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	PAGE_game_2048_rotate_board(board);
	game_message("GAME OVER", 10000);
	return ended;
}
/*
 *  Function:    static void event_handler_play_2048(lv_obj_t * obj, lv_event_t event)
 *  Input:     object that triggered the event
 *  Input:     type of the triggered event
 *  Return:     None
 *  Purpose:   Touchscreen detection event handler (up, down, left, right swipes)
*/
static void event_handler_play_2048(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_GESTURE)
	{

		switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
		{
		case LV_GESTURE_DIR_TOP:
			//printf("LV_GESTURE_DIR_TOP.\n\r");
			PAGE_game_2048_game_key_left();
			break;
		case LV_GESTURE_DIR_BOTTOM:
			//printf("LV_GESTURE_DIR_BOTTOM.\n\r");
			PAGE_game_2048_game_key_right();
			break;
		case LV_GESTURE_DIR_RIGHT:
			//printf("LV_GESTURE_DIR_RIGHT.\n\r");
			PAGE_game_2048_game_key_down();
			break;
		case LV_GESTURE_DIR_LEFT:
			//printf("LV_GESTURE_DIR_LEFT.\n\r");
			PAGE_game_2048_game_key_up();
			break;
		default:
			break;
		}
	}
	switch (event)
	{
	case LV_EVENT_LONG_PRESSED: /* Long press */
		page.PagePop();
		printf("Long press\n");
		break;
	}
}
/*
 *  Function:    static void PAGE_game_2048_game_key_right(void)
 *  Input:      None
 *  Input:      None
 *  Return:     None
 *  Purpose:   Button-controlled move up
*/
static void PAGE_game_2048_game_key_up(void)
{
	if (PAGE_game_2048_move_up(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  Function:    static void PAGE_game_2048_game_key_right(void)
 *  Input:      None
 *  Input:      None
 *  Return:     None
 *  Purpose:   Button-controlled move down
*/
static void PAGE_game_2048_game_key_down(void)
{
	if (PAGE_game_2048_move_down(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  Function:    static void PAGE_game_2048_game_key_right(void)
 *  Input:      None
 *  Input:      None
 *  Return:     None
 *  Purpose:   Button-controlled move left
*/
static void PAGE_game_2048_game_key_left(void)
{
	if (PAGE_game_2048_move_left(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  Function:    static void PAGE_game_2048_game_key_right(void)
 *  Input:      None
 *  Input:      None
 *  Return:     None
 *  Purpose:   Button-controlled move right
*/
static void PAGE_game_2048_game_key_right(void)
{
	if (PAGE_game_2048_move_right(g_pt_PAGE_2048_game->game_board))
	{
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_add_random(g_pt_PAGE_2048_game->game_board);
		PAGE_game_2048_init_board(g_pt_PAGE_2048_game->canvas_stage, g_pt_PAGE_2048_game->game_board);
		if (PAGE_game_2048_game_ended(g_pt_PAGE_2048_game->game_board))
		{
			printf("         GAME OVER          \n");
			game_message("GAME OVER", 10000);
		}
	}
}

/*
 *  Function:    static void event_handler_back_to_home(lv_obj_t * obj, lv_event_t event)
 *  Input:     object that triggered the event
 *  Input:     type of the triggered event
 *  Return:     None
 *  Purpose:   Return-to-home event handler
*/
static void event_handler_back_to_home(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		if (g_pt_PAGE_2048_game->task_handle != NULL)
			lv_task_del(g_pt_PAGE_2048_game->task_handle); /* Delete task */
		if (g_pt_PAGE_2048_game->canvas_stage != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->canvas_stage); /* Delete the game stage */
		if (g_pt_PAGE_2048_game->label_best_score != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->label_best_score); /* Delete the best score object */
		if (g_pt_PAGE_2048_game->label_current_score != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->label_current_score); /* Delete the current score object */
		if (g_pt_PAGE_2048_game->bg != NULL)
			lv_obj_del(g_pt_PAGE_2048_game->bg); /* Delete the background */

		/* Free memory */
		free(g_pt_PAGE_2048_game);

		/* Clear lv_layer_top */
		lv_obj_set_click(lv_layer_top(), false);
		lv_obj_clean(lv_layer_top());
		lv_obj_set_event_cb(lv_layer_top(), NULL); /* Assign the event handler */

		/* Clear the screen and return to the home screen */
		// PAGE_ _anim_out_all(lv_scr_act(), 0);
		// PAGE_ _demo_home(0);
	}
}

void page_game_2048_load(void)
{
	PAGE_game_2048();

	obj_add_anim(
		g_pt_PAGE_2048_game->bg,		  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
		300,							  //Animation speed
		-APP_WIN_WIDTH,							  //Start value
		0,								  //End value
		lv_anim_path_linear				  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
}

static void Exit(void)
{
	obj_add_anim(
		g_pt_PAGE_2048_game->bg,		  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
		300,							  //Animation speed
		0,								  //Start value
		-APP_WIN_WIDTH,							  //End value
		lv_anim_path_linear				  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	// lv_obj_del(game_2048);
	// if (g_pt_PAGE_2048_game->task_handle != NULL)
	// 	lv_task_del(g_pt_PAGE_2048_game->task_handle); /* Delete task */
	if (g_pt_PAGE_2048_game->canvas_stage != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->canvas_stage); /* Delete the game stage */
	if (g_pt_PAGE_2048_game->label_best_score != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->label_best_score); /* Delete the best score object */
	if (g_pt_PAGE_2048_game->label_current_score != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->label_current_score); /* Delete the current score object */
	if (g_pt_PAGE_2048_game->bg != NULL)
		lv_obj_del(g_pt_PAGE_2048_game->bg); /* Delete the background */
	lv_obj_set_click(lv_layer_top(), false);
	lv_obj_clean(lv_layer_top());
	lv_obj_set_event_cb(lv_layer_top(), NULL); /* Assign the event handler */
	/* Free memory */
	free(g_pt_PAGE_2048_game);
}
static void Setup(void)
{
	//Get the available heap size
	printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_game_2048_load();
}
void move_task_game_2048(uint8_t move)
{

	switch (move)
	{

	case BT1_LONG: //Move up
		PAGE_game_2048_game_key_down();
		break;
	case BT1_LONGFREE: //Move up

		break;
	case BT3_LONG: //Move down
		PAGE_game_2048_game_key_up();
		break;
	case BT3_LONGFREE: //Move up

		break;
	case BT1_DOWN: //Move up
		PAGE_game_2048_game_key_up();
		break;
	case BT3_DOWN: //Move down
		PAGE_game_2048_game_key_down();
		break;
	case BT1_DOUBLE: //Move up
		PAGE_game_2048_game_key_left();
		break;
	case BT3_DOUBLE: //Move down
		PAGE_game_2048_game_key_right();
		break;
	default:
		break;
	}
}
/**
  * @brief  Page event
  * @param  btn:button that raised the event
  * @param  event:event ID
  * @retval None
  */
static void Event(void *btn, int event)
{

}

/**
  * @brief  Page registration
  * @param  pageID:ID assigned to this page
  * @retval None
  */
void PageRegister_Game_2048(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register Game_2048 with the page scheduler */\r\n");
}
