#include "page_start.h"
#include "PageManager.h"
#include "app_wifi.h"
#include "app_camera.h"
#include "lv_png.h"
// #include "lv_port_fs.h"
#include "page_baiduai.h"
#include "DisplayPrivate.h"
lv_obj_t *scr;
lv_obj_t *scr_body;
lv_obj_t *cont_head;
char *file_name_with_path = NULL;
LV_FONT_DECLARE(myFont);
LV_IMG_DECLARE(kevin); //Bilibili image
LV_IMG_DECLARE(coooool);
LV_IMG_DECLARE(kevincoooool);
LV_IMG_DECLARE(airkiss);					  //WeChat network configuration
extern EventGroupHandle_t s_wifi_event_group; //WiFi event group
extern const int CONNECTED_BIT;
extern const int ESPTOUCH_DONE_BIT;
extern const int WIFI_SMART;
extern const int WIFI_CONNET_BIT;
extern const int MQTT_CONNET_BIT;
struct //logo screen elements
{
	lv_obj_t *label_smartconfig;
	lv_obj_t *bar_sys;
	lv_obj_t *label_rate;
} page_logo;
struct _ksdiy_sys_t ksdiy_sys_t;

lv_img_dsc_t img_home = {
	.header.always_zero = 0,
	.header.w = 240,
	.header.h = 240,
	.data_size = 240 * 240 * 2,
	.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
	.data = NULL,
};

// lv_img_dsc_t img_airkiss = {
// 	.header.always_zero = 0,
// 	.header.w = 320,
// 	.header.h = 240,
// 	.data_size = 320 * 240 * 2,
// 	.header.cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,
// 	.data = NULL,
// };
char *bg_buff = NULL;
char *airkiss_buff = NULL;

lv_obj_t *Imgbg;
void ks_esp_cam_init()
{

	scr = lv_scr_act(); //Get the currently active screen object

	static lv_style_t style1;
	lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, LV_COLOR_BLACK); //Set the screen background
	lv_style_set_bg_opa(&style1, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(scr, LV_BTN_PART_MAIN, &style1); /*Default button style*/

	scr_body = lv_cont_create(scr, NULL); //Screen body
	lv_cont_set_fit2(scr_body, LV_FIT_NONE, LV_FIT_NONE);
	lv_obj_set_size(scr_body, APP_WIN_WIDTH, APP_WIN_HEIGHT - 25);
	lv_obj_set_pos(scr_body, 0, 25);
	lv_style_set_border_color(&style1, LV_STATE_DEFAULT, LV_COLOR_BLACK); //Border background color
	lv_style_set_border_width(&style1, LV_STATE_DEFAULT, 0);			  //Border width
	lv_style_set_border_opa(&style1, LV_STATE_DEFAULT, 255);			  //Border transparency
	lv_obj_add_style(scr_body, LV_BTN_PART_MAIN, &style1);				  /*Default button style*/

	// Imgbg = lv_img_create(scr, NULL);
	// 	/* Set src of image with file name */
	// 	lv_img_set_src(Imgbg, "/spiffs/imgbg.png");
	// 	/* Align object */
	// 	lv_obj_align(Imgbg, NULL,LV_ALIGN_CENTER, 0, 0);
	// FILE *ff = fopen("/spiffs/imgbg.bin", "r");
	// if (ff == NULL)
	// {
	// 	ESP_LOGE("TAG", "Failed to open file for reading");
	// 	return;
	// }
	// else
	// {
	// 	fseek(ff, 0, SEEK_END);
	// 	long lSize = ftell(ff);
	// 	rewind(ff);
	// 	ESP_LOGI("TAG", "Lsize %ld", lSize);
	// 	static uint8_t first_in = 1;
	// 	if (first_in == 1)
	// 	{
	// 		first_in = 0;
	// 		bg_buff = (char *)malloc(sizeof(char) * lSize);
	// 	}
	// 	int br = fread(bg_buff, 1, lSize, ff);
	// 	ESP_LOGI("TAG", "Bytes read %d", br);
	// 	img_home.data = (uint8_t *)bg_buff;
	// 	lv_img_set_src(Imgbg, &img_home);
	// 	fclose(ff);
	// }
}
void lv_wifi_init()
{
	cont_head = lv_cont_create(scr, NULL);
	//lv_cont_set_fit(cont_head, LV_FIT_NONE);
	lv_cont_set_fit2(cont_head, LV_FIT_TIGHT, LV_FIT_NONE);
	lv_cont_set_layout(cont_head, LV_LAYOUT_ROW_TOP);
	lv_obj_set_size(cont_head, APP_WIN_WIDTH, 25);

	static lv_style_t style_cont;
	lv_style_set_pad_left(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //Set the screen background
	lv_obj_add_style(cont_head, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/

	//WiFi icon
	ksdiy_sys_t.ico.lv_wifi = lv_label_create(cont_head, NULL);
	lv_label_set_recolor(ksdiy_sys_t.ico.lv_wifi, true);
	lv_label_set_text(ksdiy_sys_t.ico.lv_wifi, "#00ff00 " LV_SYMBOL_WIFI);
	lv_obj_set_pos(ksdiy_sys_t.ico.lv_wifi, 0, 0); //Set coordinates


	//Animation
	obj_add_anim(
		cont_head,							//Animation object
		lv_obj_set_x,						//Animation function
		lv_anim_speed_to_time(500, 0, 500), //Animation speed
		APP_WIN_WIDTH,						//Start value
		0,									//End value
		lv_anim_path_ease_out				//Animation effect: simulate a bouncing object falling
	);
	ANIEND
	static lv_obj_t *img44;
	img44 = lv_img_create(scr, NULL);
	lv_img_set_src(img44, &kevincoooool);
	lv_obj_set_pos(img44, APP_WIN_WIDTH - 190, 0);

	obj_add_anim(
		img44,								//Animation object
		lv_obj_set_x,						//Animation function
		lv_anim_speed_to_time(500, 0, 500), //Animation speed
		APP_WIN_WIDTH,						//Start value
		APP_WIN_WIDTH - 190,				//End value
		lv_anim_path_ease_out				//Animation effect: simulate a bouncing object falling
	);
		ANIEND
			ksdiy_sys_t.state.wifi = 1;
}
lv_obj_t *img22 = NULL, *img33 = NULL, *img44 = NULL;

void start_logo()
{
	lv_obj_clean(scr_body);

	img22 = lv_img_create(scr_body, NULL);
	lv_img_set_src(img22, &kevin);
	lv_obj_set_pos(img22, -(APP_WIN_WIDTH / 2), 71);

	img33 = lv_img_create(scr_body, NULL);
	lv_img_set_src(img33, &coooool);
	lv_obj_set_pos(img33, APP_WIN_WIDTH, 70);

	//Animation
	obj_add_anim(
		img22,								 //Animation object
		lv_obj_set_x,						 //Animation function
		lv_anim_speed_to_time(1000, 0, 500), //Animation speed
		-(APP_WIN_WIDTH / 2),				 //Start value
		APP_WIN_WIDTH / 2 - 115,			 //End value
		lv_anim_path_ease_out				 //Animation effect: simulate a bouncing object falling
	);
	obj_add_anim(
		img33,								 //Animation object
		lv_obj_set_x,						 //Animation function
		lv_anim_speed_to_time(1000, 0, 500), //Animation speed
		APP_WIN_WIDTH,						 //Start value
		APP_WIN_WIDTH / 2,					 //End value
		lv_anim_path_ease_out				 //Animation effect: simulate a bouncing object falling
	);

	ANIEND
	//Set text
	lv_obj_t *label_speech = lv_label_create(scr_body, NULL);
	lv_obj_set_style_local_text_font(label_speech, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_recolor(label_speech, true);				 /*Enable re-coloring by commands in the text*/
	lv_label_set_align(label_speech, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
	lv_label_set_text(label_speech, "KuShi DIY");
	lv_obj_set_width(label_speech, 150);
	lv_obj_align(label_speech, NULL, LV_ALIGN_CENTER, 0, 30);
	lv_label_set_text(label_speech, " ");
}
/*
Network configuration
*/
static lv_obj_t *img_airkiss;
void smartconfig_set()
{
	// lv_obj_clean(scr_body);
	// lv_obj_clean(scr);
	xTaskCreate(smartconfig_example_task, "smartconfig_task", 1024 * 2, NULL, 3, NULL);

	img_airkiss = lv_img_create(scr, NULL);
	lv_img_set_src(img_airkiss, "/spiffs/airkiss.png");
	/* Align object */
	lv_obj_align(img_airkiss, NULL, LV_ALIGN_CENTER, 0, 0);

	obj_add_anim(
		img_airkiss,						//Animation object
		lv_obj_set_x,						//Animation function
		lv_anim_speed_to_time(500, 0, 500), //Animation speed
		APP_WIN_WIDTH,						//Start value
		APP_WIN_WIDTH / 2 - lv_obj_get_width(img_airkiss)/2,									//End value
		lv_anim_path_overshoot				//Animation effect: simulate a bouncing object falling
	);

	// page_logo.label_smartconfig = lv_label_create(scr_body, NULL);
	// lv_label_set_long_mode(page_logo.label_smartconfig, LV_LABEL_LONG_BREAK); /*Break the long lines*/
	// lv_label_set_recolor(page_logo.label_smartconfig, true);				  /*Enable re-coloring by commands in the text*/
	// lv_label_set_align(page_logo.label_smartconfig, LV_LABEL_ALIGN_CENTER);	  /*Center aligned lines*/
	// lv_obj_set_style_local_text_font(page_logo.label_smartconfig, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	// lv_label_set_text(page_logo.label_smartconfig, "Scan with WeChat or use ESP_TOUCH");
	// lv_obj_set_width(page_logo.label_smartconfig, APP_WIN_HEIGHT);
	// lv_obj_align(page_logo.label_smartconfig, NULL, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}
static void event_handler_wifi(lv_obj_t *obj, lv_event_t event)
{

	if (event == LV_EVENT_VALUE_CHANGED)
	{

		if (lv_msgbox_get_active_btn(obj) == 0)
		{
			lv_msgbox_start_auto_close(obj, 0);
			ESP_LOGI("WIFI", "start smartconfig");
			smartconfig_set();
			ksdiy_sys_t.state.wifi = 3;
		}
		else
		{
			lv_msgbox_start_auto_close(obj, 0);
			ksdiy_sys_t.state.wifi = 0;
		}
	}
}
void switch_wifi()
{
	lv_obj_set_hidden(page_logo.bar_sys, 1);
	lv_obj_set_hidden(page_logo.label_rate, 1);
	static const char *btns[] = {"OK", "NO", ""};
	lv_obj_t *mbox_wifi;
	mbox_wifi = lv_msgbox_create(scr, NULL);
	lv_obj_set_style_local_text_font(mbox_wifi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_msgbox_set_text(mbox_wifi, "Enable WeChat provisioning?");

	lv_msgbox_add_btns(mbox_wifi, btns);
	lv_obj_set_width(mbox_wifi, 200);
	lv_obj_set_height(mbox_wifi, 50);
	lv_obj_set_event_cb(mbox_wifi, event_handler_wifi);
	lv_obj_align(mbox_wifi, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/

	ksdiy_sys_t.group = lv_group_create();
	lv_group_add_obj(ksdiy_sys_t.group, mbox_wifi); //Add to the decoder group
	lv_button_set_group(ksdiy_sys_t.group);
	encoder_handler(1);
}
void wifi_rate()
{
	EventBits_t uxBits;

	lv_label_set_text(page_logo.label_rate, "Connecting WiFi");
	lv_obj_align(page_logo.label_rate, NULL, LV_ALIGN_CENTER, 0, 30);
	ANIEND
	if (app_wifi_init() != ESP_OK)
	{
		smartconfig_set();

		while (1)
		{
			uxBits = xEventGroupGetBits(s_wifi_event_group);
			if (uxBits & CONNECTED_BIT) //Network configuration succeeded
				break;
			vTaskDelay(pdMS_TO_TICKS(1));
			lv_task_handler();
		}
		// if (ksdiy_sys_t.state.wifi == 3) // if WeChat network provisioning is enabled
		// {
		uxBits = xEventGroupWaitBits(s_wifi_event_group, WIFI_SMART, true, false, portMAX_DELAY);
		if (uxBits & WIFI_SMART) //Wait for network configuration to complete
		{

			obj_add_anim(
				img_airkiss,									   //Animation object
				lv_obj_set_x,									   //Animation function
				lv_anim_speed_to_time(500, 0, 500),				   //Animation speed
				lv_obj_get_x(img_airkiss),												   //Start value
				APP_WIN_WIDTH , //End value
				lv_anim_path_overshoot							   //Animation effect: simulate a bouncing object falling
			);
			// lv_obj_del(page_logo.label_smartconfig);
			ESP_LOGI("WIFI", "wx over");
		}
		// }
	}
	lv_wifi_init();
	ANIEND
}
void fans_rate()
{
	lv_label_set_text(page_logo.label_rate, "Getting fans");
	lv_obj_align(page_logo.label_rate, NULL, LV_ALIGN_CENTER, 0, 30);
	read_fans();
}
void time_rate()
{
	lv_label_set_text(page_logo.label_rate, "Getting time");
	lv_obj_set_hidden(page_logo.bar_sys, 0);
	lv_obj_set_hidden(page_logo.label_rate, 0);
	lv_obj_align(page_logo.label_rate, NULL, LV_ALIGN_CENTER, 0, 30);
	lv_task_handler();
	time_init();
}
void weather_rate()
{
	lv_label_set_text(page_logo.label_rate, "Getting weather");
	lv_obj_align(page_logo.label_rate, NULL, LV_ALIGN_CENTER, 0, 30);
	read_weather();
}
void show_rate(void (*fun)(), uint8_t rate)
{
	fun();
	lv_bar_set_value(page_logo.bar_sys, rate, LV_ANIM_ON);
	ANIEND
}
void start_rate()
{
	lv_label_set_text(page_logo.label_rate, "Loading drivers");
	lv_obj_align(page_logo.label_rate, NULL, LV_ALIGN_CENTER, 0, 30);
}
void ocr_rate()
{
	lv_label_set_text(page_logo.label_rate, "Getting OCR token");
	lv_obj_align(page_logo.label_rate, NULL, LV_ALIGN_CENTER, 0, 30);
	// app_camera_init();
	// get_ocr_token(); // get Baidu image recognition token
}
/**
 * @Descripttion: Baidu image recognition token progress
 * @param {*}
 * @return {*}
 * @Author: Kevincoooool
 */
void img_rate()
{
	lv_label_set_text(page_logo.label_rate, "Getting image token");
	lv_obj_align(page_logo.label_rate, NULL, LV_ALIGN_CENTER, 0, 30);
	// app_camera_init();
	// get_img_token(); // get text recognition token
}

/**
 * @descripttion: Percentage display
 * @param {*}
 * @return {*}
 * @author: Kevincoooool
 */
void dev_init()
{
	show_rate(start_rate, 10);
	show_rate(wifi_rate, 45);

	show_rate(time_rate, 60);
	show_rate(fans_rate, 80);
	show_rate(weather_rate, 100);
	// show_rate(ocr_rate, 80);
	// vTaskDelay(3000);
	// show_rate(img_rate, 100);
}
/**
 * @Descripttion: Create the progress bar and display text
 * @param {*}
 * @return {*}
 * @Author: Kevincoooool
 */
void ksdiy_sys_init()
{

	static lv_style_t bar_bg;
	lv_style_init(&bar_bg);
	page_logo.bar_sys = lv_bar_create(scr_body, NULL);
	lv_obj_set_size(page_logo.bar_sys, 150, 15);
	lv_obj_align(page_logo.bar_sys, NULL, LV_ALIGN_CENTER, 0, 80);
	lv_style_set_radius(&bar_bg, LV_STATE_DEFAULT, 10);
	lv_style_set_bg_opa(&bar_bg, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&bar_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&bar_bg, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_dir(&bar_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_obj_add_style(page_logo.bar_sys, LV_BAR_PART_BG, &bar_bg);
	lv_bar_set_anim_time(page_logo.bar_sys, 200);

	//Set text
	page_logo.label_rate = lv_label_create(scr_body, NULL);
	static lv_style_t label_shadow_style;
	lv_style_init(&label_shadow_style);
	lv_style_set_text_opa(&label_shadow_style, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&label_shadow_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);

	lv_obj_set_style_local_text_font(page_logo.label_rate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_label_set_recolor(page_logo.label_rate, true);				 /*Enable re-coloring by commands in the text*/
	lv_label_set_align(page_logo.label_rate, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
	lv_obj_set_height(page_logo.label_rate, 10);
	lv_obj_set_width(page_logo.label_rate, 150);
	lv_obj_add_style(page_logo.label_rate, LV_LABEL_PART_MAIN, &label_shadow_style);
	dev_init();
	ANIEND
}
/**
 * @Descripttion: Delete this page
 * @param {*}
 * @return {*}
 * @Author: Kevincoooool
 */
void del_page()
{
	obj_add_anim(
		img22,								//Animation object
		lv_obj_set_x,						//Animation function
		lv_anim_speed_to_time(500, 0, 500), //Animation speed
		APP_WIN_WIDTH / 2 - 115,			//End value
		-(APP_WIN_WIDTH / 2),				//Start value

		lv_anim_path_ease_out //Animation effect: simulate a bouncing object falling
	);
	obj_add_anim(
		img33,								//Animation object
		lv_obj_set_x,						//Animation function
		lv_anim_speed_to_time(500, 0, 500), //Animation speed
		APP_WIN_WIDTH / 2,					//End value
		APP_WIN_WIDTH,						//Start value
		lv_anim_path_ease_out				//Animation effect: simulate a bouncing object falling
	);
	obj_add_anim(
		page_logo.bar_sys,					//Animation object
		lv_obj_set_y,						//Animation function
		lv_anim_speed_to_time(500, 0, 500), //Animation speed
		lv_obj_get_y(page_logo.bar_sys),	//End value
		APP_WIN_HEIGHT,						//Start value
		lv_anim_path_ease_out				//Animation effect: simulate a bouncing object falling
	);
	obj_add_anim(
		page_logo.label_rate,				//Animation object
		lv_obj_set_y,						//Animation function
		lv_anim_speed_to_time(500, 0, 500), //Animation speed
		lv_obj_get_y(page_logo.label_rate), //End value
		APP_WIN_HEIGHT,						//Start value
		lv_anim_path_ease_out				//Animation effect: simulate a bouncing object falling
	);

	ANIEND

	// obj_move(&c22, 0);
	// ANIEND
	lv_obj_clean(scr_body); //Delete scr_body
							// move_free(&c22);
}
/**
 * @Descripttion: System notification message box
 * @param {const char} *str
 * @param {uint16_t} t
 * @return {*}
 * @Author: Kevincoooool
 */
void lv_message(const char *str, uint16_t t) //System notification box
{

	ksdiy_sys_t.lv_bsgbox = lv_msgbox_create(scr, NULL); //Global notification box
	lv_obj_t *mbox1 = ksdiy_sys_t.lv_bsgbox;
	lv_obj_set_style_local_text_font(mbox1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_msgbox_set_text(mbox1, str);
	lv_msgbox_start_auto_close(mbox1, t);
	lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}
void ksdiy_task() //Main task, always running in the background of LVGL
{
	static int t = 0, clock = 0;
	char str[20];
	get_time(&ksdiy_sys_t.timeinfo); //Get time
	if (t == 3 * 60 * 10)			 //Analyze follower count changes every 10 minutes
	{
		ESP_LOGI("ksdiy", "Updating fan count");
		read_fans();
		read_weather();
		t = 0;

		uint16_t fensi, guanzhu;
		fensi = get_value_ksdiyfs(1);
		guanzhu = get_value_ksdiyfs(0);
		if (ksdiy_sys_t.wp.following != guanzhu)
			ksdiy_sys_t.wp.following = guanzhu;
		if (ksdiy_sys_t.wp.follow != fensi)
		{
			if (ksdiy_sys_t.wp.follow > fensi)
			{
				int t = ksdiy_sys_t.wp.follow - fensi; //Follower count decreased
				sprintf(str, "Fans-%d", t);
				lv_message((const char *)str, 5000);
			}
			else
			{
				int t = fensi - ksdiy_sys_t.wp.follow; //Follower count increased
				sprintf(str, "Fans+%d", t);
				lv_message((const char *)str, 5000);
			}

			update_fensi();

			ksdiy_sys_t.wp.follow = fensi;
		}
	}
	t++;
}

void wifi_state_task()
{
	uint8_t state;
	EventBits_t uxBits;
	uxBits = xEventGroupGetBits(s_wifi_event_group);
	if (uxBits & CONNECTED_BIT)
		state = 1;
	else
		state = 0;
	if (state != ksdiy_sys_t.state.wifi)
	{
		if (state) //Connected to network
		{

			ESP_LOGI("SYSTEM", "WIFI_CONCONT");
			lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_wifi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
			lv_label_set_text(ksdiy_sys_t.ico.lv_wifi, LV_SYMBOL_WIFI);
		}
		else
		{
			ESP_LOGI("SYSTEM", "WIFI_DISCONCONT");
			lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_wifi, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
			lv_label_set_text(ksdiy_sys_t.ico.lv_wifi, LV_SYMBOL_WIFI);
		}
		ksdiy_sys_t.state.wifi = state;
	}
}
void page_init()
{
	group_button = lv_group_create();
	ks_esp_cam_init();												//LVGL initialization
	start_logo();													//Boot logo
	ksdiy_sys_init();												//Driver initialization
	del_page();														//Delete the current page
	ksdiy_sys_info_init();											//System information initialization
	get_time(&ksdiy_sys_t.timeinfo);								//Get time
	lv_task_create(ksdiy_task, 500, LV_TASK_PRIO_MID, NULL);		//Main task; handles time and notifications
	lv_task_create(wifi_state_task, 10000, LV_TASK_PRIO_LOW, NULL); //WiFi detection task

	ESP_LOGI("SYSTEM", "esp_get_free_heap_size : %d ", esp_get_free_heap_size());
	Display_Init();
}
