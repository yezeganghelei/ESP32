
/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "button.h"
#include "page_game_snake.h"
#include "app_anim.h"

#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"
LV_FONT_DECLARE(myFont);

/**********************
 *  STATIC VARIABLES
 **********************/
static PT_KSDIY_snake g_pt_page_game_snake; // Data structure
static link_snake *spriteSnake;					// Linked-list head (snake head)

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_game_snake_init(void);									 // Initialize the screen
static link_snake *page_game_snake_initLink(void);						 // Initialize the linked list
static void page_game_snake_clear_list(link_snake *ppHeadNode);			 // Clear the linked list, free node memory, and reset it to an empty list
static void page_game_snake_linkAddNode(void);							 // Insert a new node at the tail of the linked list (grow the snake body)
static void lv_task_100ask_game_snake(lv_task_t *task);					 // Game task
static void page_game_snake_update_snake_data(void);					 // Update the snake data structure
static void event_handler_snake_gesture_cb(lv_obj_t *obj, lv_event_t e); // Touchscreen detection event handler (touchscreen controls the game)
static void event_handler_back_to_home(lv_obj_t *obj, lv_event_t event); // Return-to-home event handler

/*
 *  Function:    void page_game_snake(void)
 *  Input:      None
 *  Return:     None
 *  Purpose:   Application initialization entry
*/
void page_game_snake(void)
{
	g_pt_page_game_snake = (T_KSDIY_snake *)malloc(sizeof(T_KSDIY_snake)); // Allocate memory
	g_pt_page_game_snake->gesture = 0;

	g_pt_page_game_snake->bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(g_pt_page_game_snake->bg, LV_HOR_RES, LV_VER_RES);
	lv_obj_set_y(g_pt_page_game_snake->bg, 0);

	page_game_snake_init();

	// /* Create a task to detect key input and game animation */
	g_pt_page_game_snake->task_handle = lv_task_create(lv_task_100ask_game_snake, 150, LV_TASK_PRIO_MID, NULL);

	// lv_obj_set_click(lv_layer_top(), true);
	// lv_obj_set_event_cb(lv_layer_top(), event_handler_snake_gesture_cb);   	// assign event handler

	// add_title(g_pt_page_game_snake->bg, "SNAKE");							// title
	// add_back(lv_layer_top(), event_handler_back_to_home);    				// back-to-home button
		/* Assign the screen touch event handler */
	lv_obj_set_click(lv_layer_top(), true);
	lv_obj_set_event_cb(lv_layer_top(), event_handler_snake_gesture_cb);
}

/*
 *  Function:    static void page_game_snake_init(void)
 *  Input:      None
 *  Return:     None
 *  Purpose:   Application screen initialization
*/
static void page_game_snake_init(void)
{
	spriteSnake = page_game_snake_initLink();

	link_snake *head = NULL;
	head = spriteSnake; //Point the temp pointer back to the head node
	int i = 0;
	lv_coord_t init_x = (rand() % (LV_HOR_RES));
	lv_coord_t init_y = (rand() % (LV_VER_RES));
	while (head->next)
	{
		head = head->next;
		head->obj = lv_obj_create(g_pt_page_game_snake->bg, NULL);
		if (i == 0)
		{
			lv_obj_set_style_local_bg_color(head->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KSDIY_GAME_SNAKE_HEAD_COLOR); //Set color
		}
		else
			lv_obj_set_style_local_bg_color(head->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KSDIY_GAME_SNAKE_BODY_COLOR); //Set color
		lv_obj_set_style_local_radius(head->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KSDIY_GAME_SNAKE_RADIUS);			 // Set corner radius
		lv_obj_set_size(head->obj, KSDIY_GAME_SNAKE_SIZE, KSDIY_GAME_SNAKE_SIZE);
		lv_obj_set_pos(head->obj, init_x + (i * KSDIY_GAME_SNAKE_SIZE), init_y);
		head->x = lv_obj_get_x(head->obj);
		head->y = lv_obj_get_y(head->obj);
		i++;
	}

	// Initialize the food
	g_pt_page_game_snake->obj_food = lv_obj_create(g_pt_page_game_snake->bg, NULL);
	lv_obj_set_size(g_pt_page_game_snake->obj_food, KSDIY_GAME_FOOD_SIZE, KSDIY_GAME_FOOD_SIZE);
	lv_obj_set_style_local_bg_color(g_pt_page_game_snake->obj_food, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KSDIY_GAME_FOOD_COLOR); //Set color
	lv_obj_set_style_local_radius(g_pt_page_game_snake->obj_food, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KSDIY_GAME_FOOD_RADIUS);	// Set corner radius
	lv_obj_set_pos(g_pt_page_game_snake->obj_food, (rand() % (KSDIY_GAME_FOOD_MAX_HOR)), (rand() % (KSDIY_GAME_FOOD_MAX_VER)));

	g_pt_page_game_snake->sroce = 3;
	g_pt_page_game_snake->len = 3;
	g_pt_page_game_snake->label_info = lv_label_create(g_pt_page_game_snake->bg, NULL);
	lv_obj_set_style_local_text_font(g_pt_page_game_snake->label_info, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);	   // Set the font
	lv_label_set_text_fmt(g_pt_page_game_snake->label_info, "LEN: %d\nSROCE: %d", g_pt_page_game_snake->len, g_pt_page_game_snake->sroce); // Display game information
	lv_obj_align(g_pt_page_game_snake->label_info, NULL, LV_ALIGN_IN_TOP_LEFT, 0, KSDIY_GAME_SNAKE_TITLE_SPACE);
}

//
/*
 *  Function:    static link_snake* page_game_snake_initLink(void)
 *  Input:      None
 *  Return:    initialized linked list
 *  Purpose:   Initialize the linked list (snake)
*/
static link_snake *page_game_snake_initLink(void)
{
	link_snake *head = (link_snake *)malloc(sizeof(link_snake)); // Create the first node of the linked list (head element node)
	head->prior = NULL;
	head->next = NULL;
	head->obj = NULL;
	head->x = 0;
	head->y = 0;
	link_snake *list = head; //Declare a pointer to the head node so new nodes can be appended later

	// Build the linked list (initialize the snake body)
	for (int i = 0; i < KSDIY_GAME_SNAKE_INIT_LINE; i++)
	{
		link_snake *body = (link_snake *)malloc(sizeof(link_snake));
		body->prior = NULL;
		body->next = NULL;
		body->obj = NULL;
		body->x = i;
		body->y = i;

		//Link the new node with the last node of the list
		list->next = body;
		body->prior = list;
		//list always points to the last node in the list
		list = list->next;
	}
	return head;
}

/*
 *  Function:    static void page_game_snake_linkAddNode(void)
 *  Input:      None
 *  Return:     None
 *  Purpose:   Insert a new node at the tail of the linked list (grow the snake body)
*/
static void page_game_snake_linkAddNode(void)
{
	link_snake *temp_list;

	// Initialize the new node
	link_snake *list_new = (link_snake *)malloc(sizeof(link_snake));
	list_new->prior = NULL;
	list_new->next = NULL;
	list_new->x = 0;
	list_new->y = 0;
	list_new->obj = lv_obj_create(g_pt_page_game_snake->bg, NULL);
	lv_obj_set_style_local_bg_color(list_new->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KSDIY_GAME_SNAKE_BODY_COLOR); //Set color
	lv_obj_set_style_local_radius(list_new->obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, KSDIY_GAME_SNAKE_RADIUS);		 // Set corner radius
	lv_obj_set_size(list_new->obj, KSDIY_GAME_SNAKE_SIZE, KSDIY_GAME_SNAKE_SIZE);

	// Check the head node
	if (spriteSnake == NULL)
	{
		spriteSnake = list_new;
	}
	else
	{
		temp_list = spriteSnake;
		while (temp_list->next) // Move to the tail of the list
		{
			temp_list = temp_list->next;
		}
		temp_list->next = list_new;
		list_new->prior = temp_list;
	}
}

/*
 *  Function:    static void page_game_snake_clear_list(link_snake* ppHeadNode)
 *  Input:      None
 *  Return:     None
 *  Purpose:   Clear the linked list, free node memory, and reset it to an empty list
*/
static void page_game_snake_clear_list(link_snake *ppHeadNode)
{
	link_snake *pListNodeTmp = NULL;
	if ((ppHeadNode) == NULL)
	{
		printf("The list is empty, no need to clear.\n");
		return;
	}

	//bool is_head = true;
	// Loop to free the memory used by the list nodes,
	while ((ppHeadNode)->next != NULL)
	{
		pListNodeTmp = (ppHeadNode)->next;
		if (ppHeadNode->obj != NULL)
			lv_obj_clean(ppHeadNode->obj); // Delete the snake body node
		free((ppHeadNode));
		(ppHeadNode) = pListNodeTmp;
	}

	// Clear the last node
	if ((ppHeadNode) != NULL)
	{
		if (ppHeadNode->obj != NULL)
			lv_obj_clean(ppHeadNode->obj); // Delete the snake body node
		free((ppHeadNode));
		(ppHeadNode) = NULL;
	}

	printf("The list is cleared.\n");
}

/*
 *  Function:    static void lv_task_100ask_game_snake(lv_task_t * task)
 *  Input:     task descriptor
 *  Return:     None
 *  Purpose:   Detect physical key input to control the game and keep moving the snake
*/
static void lv_task_100ask_game_snake(lv_task_t *task)
{
#if KSDIY_GAME_SNAKE_USE_KEY
	// uint8_t *key = (uint8_t *)&key_value.up_value;
	// if (key[0] == 1)
	// {
	// 	printf("UP\n\r");
	// 	g_pt_page_game_snake->gesture |= 1;
	// 	key[0] = 0;
	// }
	// else if (key[1] == 1)
	// {
	// 	printf("DOWN\n\r");
	// 	g_pt_page_game_snake->gesture |= 2;
	// 	key[1] = 0;
	// }
	// else if (key[2] == 1)
	// {
	// 	printf("LEFT\n\r");
	// 	g_pt_page_game_snake->gesture |= 8;
	// 	key[2] = 0;
	// }
	// else if (key[3] == 1)
	// {
	// 	printf("RIGHT\n\r");
	// 	g_pt_page_game_snake->gesture |= 4;
	// 	key[3] = 0;
	// }
#endif // KSDIY_GAME_SNAKE_USE_KEY

	lv_coord_t x = spriteSnake->next->x - lv_obj_get_x(g_pt_page_game_snake->obj_food); // Get the x-axis difference
	lv_coord_t y = spriteSnake->next->y - lv_obj_get_y(g_pt_page_game_snake->obj_food); // Get the y-axis difference
	if (((x >= -KSDIY_GAME_FOOD_CHECK_SPACE) && (x <= KSDIY_GAME_FOOD_CHECK_SPACE)) && ((y >= -KSDIY_GAME_FOOD_CHECK_SPACE) && (y <= KSDIY_GAME_FOOD_CHECK_SPACE)))
	{
		page_game_snake_linkAddNode();
		g_pt_page_game_snake->len += 1;
		g_pt_page_game_snake->sroce += 1;

		lv_obj_set_pos(g_pt_page_game_snake->obj_food, rand() % (KSDIY_GAME_FOOD_MAX_HOR), rand() % (KSDIY_GAME_FOOD_MAX_VER));
		lv_label_set_text_fmt(g_pt_page_game_snake->label_info, "LEN: %u\nSROCE: %u", g_pt_page_game_snake->len, g_pt_page_game_snake->sroce); // Display game information
	}
	page_game_snake_update_snake_data();
}

/*
 *  Function:    static void event_handler_snake_gesture_cb(lv_obj_t * obj, lv_event_t e)
 *  Input:     object that triggered the event
 *  Input:     type of the triggered event
 *  Return:     None
 *  Purpose:   Touchscreen detection event handler that controls the snake's movement
*/
static void event_handler_snake_gesture_cb(lv_obj_t *obj, lv_event_t e)
{
	if (e == LV_EVENT_GESTURE)
	{
		switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
		{
		case LV_GESTURE_DIR_TOP:
			g_pt_page_game_snake->gesture |= 1;
			break;
		case LV_GESTURE_DIR_BOTTOM:
			g_pt_page_game_snake->gesture |= 2;
			break;
		case LV_GESTURE_DIR_RIGHT:
			g_pt_page_game_snake->gesture |= 4;
			break;
		case LV_GESTURE_DIR_LEFT:
			g_pt_page_game_snake->gesture |= 8;
			break;
		default:
			break;
		}
		page_game_snake_update_snake_data();
	}
	// else if (e == LV_EVENT_LONG_PRESSED) // accelerate when long-pressed
	// {
	// 	page_game_snake_update_snake_data();
	// }
	switch (e)
	{
	case LV_EVENT_LONG_PRESSED: /* Long press */
		page.PagePop();
		printf("Long press\n");
		break;
	}
}

/*
 *  Function:    static void page_game_snake_update_snake_data(void)
 *  Input:      None
 *  Return:     None
 *  Purpose:   Update the snake data structure
*/
static void page_game_snake_update_snake_data(void)
{
	static lv_coord_t x = KSDIY_GAME_SNAKE_SPEED, y = 0;					 // Move right by default
	volatile lv_coord_t obj_size_x = KSDIY_GAME_SNAKE_SPACE, obj_size_y = 0; // Spacing compensation
	if (((g_pt_page_game_snake->gesture) & 1) == 1)								 // Up
	{
		y = -KSDIY_GAME_SNAKE_SPEED;
		x = 0;
		obj_size_y = -KSDIY_GAME_SNAKE_SPACE;
		obj_size_x = 0;
	}
	else if (((g_pt_page_game_snake->gesture) & 2) == 2) // Down
	{
		y = KSDIY_GAME_SNAKE_SPEED;
		x = 0;
		obj_size_y = KSDIY_GAME_SNAKE_SPACE;
		obj_size_x = 0;
	}
	else if (((g_pt_page_game_snake->gesture) & 4) == 4) // Left
	{
		y = 0;
		x = KSDIY_GAME_SNAKE_SPEED;
		obj_size_y = 0;
		obj_size_x = KSDIY_GAME_SNAKE_SPACE;
	}
	else if (((g_pt_page_game_snake->gesture) & 8) == 8) // Right
	{
		y = 0;
		x = -KSDIY_GAME_SNAKE_SPEED;
		obj_size_y = 0;
		obj_size_x = -KSDIY_GAME_SNAKE_SPACE;
	}
	g_pt_page_game_snake->gesture = 0; // Clear the flag

	link_snake *temp_list = NULL;
	temp_list = spriteSnake; //Point the temp_list pointer back to the head node
	while (temp_list->next)	 // Move to the tail of the list
	{
		temp_list = temp_list->next;
	}
	while (temp_list->prior->prior) // Exclude the head node
	{
		// Iterate backwards to update the coordinate data
		temp_list->x = temp_list->prior->x;
		temp_list->y = temp_list->prior->y;
		lv_obj_set_pos(temp_list->obj, temp_list->x, temp_list->y);
		temp_list = temp_list->prior;
	}
	// Handle the head node
	temp_list->x = lv_obj_get_x(temp_list->obj) + x;
	temp_list->y = lv_obj_get_y(temp_list->obj) + y;
	lv_obj_set_pos(temp_list->obj, temp_list->x, temp_list->y);
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
		if (g_pt_page_game_snake->task_handle != NULL)
			lv_task_del(g_pt_page_game_snake->task_handle); // Delete the task.
		if (g_pt_page_game_snake->bg != NULL)
			lv_obj_del(g_pt_page_game_snake->bg); // Delete the background
		if (g_pt_page_game_snake->obj_food != NULL)
			lv_obj_del(g_pt_page_game_snake->obj_food); // Delete the food
		if (g_pt_page_game_snake->label_info != NULL)
			lv_obj_del(g_pt_page_game_snake->label_info); // Delete the game info label

		/* Delete the snake body */
		page_game_snake_clear_list((spriteSnake)); // Clear the linked list
		g_pt_page_game_snake->sroce = 0;		   // Clear the game score
		g_pt_page_game_snake->len = 0;			   // Clear the length

		free(g_pt_page_game_snake);

		// lv_obj_set_event_cb(lv_layer_top(), NULL); /* Assign event handler */
		// lv_obj_set_click(lv_layer_top(), false);
		// lv_obj_clean(lv_layer_top());

		// page_anim_out_all(lv_scr_act(), 0);
		// page_demo_home(10);
	}
}

void page_game_snake_load(void)
{
	page_game_snake();

	obj_add_anim(
		g_pt_page_game_snake->bg,						  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
		300,							  //Animation speed
		-APP_WIN_WIDTH,							  //Start value
		0,								  //End value
		lv_anim_path_bounce				  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
}
static void Exit(void)
{
	obj_add_anim(
		g_pt_page_game_snake->bg,						  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
		300,							  //Animation speed
		0,							  //Start value
		-APP_WIN_WIDTH,								  //End value
		lv_anim_path_bounce				  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	if (g_pt_page_game_snake->task_handle != NULL)
		lv_task_del(g_pt_page_game_snake->task_handle); // Delete the task.
	// if (g_pt_page_game_snake->bg != NULL)
	// 	lv_obj_del(g_pt_page_game_snake->bg); // delete background
	// if (g_pt_page_game_snake->obj_food != NULL)
	// 	lv_obj_del(g_pt_page_game_snake->obj_food); // delete food
	// if (g_pt_page_game_snake->label_info != NULL)
	// 	lv_obj_del(g_pt_page_game_snake->label_info); // delete game info label
	
	/* Delete the snake body */
	page_game_snake_clear_list((spriteSnake)); // Clear the linked list
	g_pt_page_game_snake->sroce = 0;		   // Clear the game score
	g_pt_page_game_snake->len = 0;			   // Clear the length
	lv_obj_set_click(lv_layer_top(), false);
	lv_obj_clean(lv_layer_top());
	lv_obj_set_event_cb(lv_layer_top(), NULL); /* Assign the event handler */

	free(g_pt_page_game_snake);
}
static void Setup(void)
{
	//Get the available heap size
	printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_game_snake_load();
}
void move_task_game_snake(uint8_t move)
{

	switch (move)
	{

	case BT1_LONG: //Move up
		// PAGE_game_snake_game_key_down();
		g_pt_page_game_snake->gesture |= 4;
		break;
	case BT1_LONGFREE: //Move up

		break;
	case BT3_LONG: //Move down
		// PAGE_game_snake_game_key_up();
		g_pt_page_game_snake->gesture |= 8;
		break;
	case BT3_LONGFREE: //Move up

		break;
	case BT1_DOWN: //Move up
		// PAGE_game_snake_game_key_left();
		printf("UP\n\r");
		g_pt_page_game_snake->gesture |= 1;
		break;
	case BT3_DOWN: //Move down
		// PAGE_game_snake_game_key_right();
		printf("UP\n\r");
		g_pt_page_game_snake->gesture |= 2;
		break;
		case BT1_DOUBLE: //Move up
		g_pt_page_game_snake->gesture |= 8;
		break;
	case BT3_DOUBLE: //Move down
		g_pt_page_game_snake->gesture |= 4;
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
void PageRegister_Game_Snake(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register Game_Snake with the page scheduler */\r\n");
}
