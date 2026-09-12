/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-05 09:04:44
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-19 15:46:53
 * @FilePath: \s3_lvgl_v7\main\page\PageManager.c
 */

#include "PageManager.h"
#include "esp_log.h"
#include "page_menu.h"
#include "page_home.h"
#include "page_wakeup.h"
#include "page_cam.h"
#include "page_fft.h"
#include "page_start.h"
#include "page_calendar.h"
#include "page_baiduai.h"
#include "page_game_2048.h"
#include "page_game_snake.h"

#include "page_daily.h"
#include "page_imu.h"
#include "page_about.h"

#include "page_fft.h"
#include "updata.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"

#include "esp_err.h"
#include "cJSON.h"
#include "mqtt_client.h"
#include "app_camera.h"
#include "web_server.h"
#include "app_wifi.h"
extern struct _ksdiy_sys_t ksdiy_sys_t;

extern lv_obj_t *cont_head;

void save_clock(uint8_t clock_index, uint8_t s, uint8_t h, uint8_t m)
{
	char index[10];
	char info[30];
	if (h > 23 || m > 60 || s > 2)
	{
		ESP_LOGI("CLOCK", "Saved time format error 1 s:%d,h:%d,m:%d", s, h, m);
		return;
	}
	sprintf(info, "{\"s\":%d,\"h\":%d,\"m\":%d}", s, h, m);
	sprintf(index, "ck%d", clock_index);
	save_nvs((const char *)index, info);
}
void lv_page_clock_init() //Get alarm information
{
	//char info[15];
	// if(read_nvs("t_all",info))// get bilibili uid
	// {
	//     sprintf(str,"%s%s%s",BILIBILI_REQUEST1,uid,BILIBILI_REQUEST2);
	//     ESP_LOGI(TAG_HTTP,"Got uid");
	// }
}
void lv_ico_web_show(uint8_t state)
{
	if (state)
	{
		ksdiy_sys_t.ico.lv_web = lv_label_create(cont_head, NULL);
		lv_label_set_recolor(ksdiy_sys_t.ico.lv_web, true);
		lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_web, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
		lv_label_set_text(ksdiy_sys_t.ico.lv_web, LV_SYMBOL_DIRECTORY);
	}
	else
	{
		if (ksdiy_sys_t.ico.lv_web != NULL)
			lv_obj_del(ksdiy_sys_t.ico.lv_web);
	}
}
void lv_ico_clock_show(uint8_t state)
{
	if (state)
	{
		ksdiy_sys_t.ico.lv_clock = lv_label_create(cont_head, NULL);
		lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_clock, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
		lv_label_set_recolor(ksdiy_sys_t.ico.lv_clock, true);
		lv_label_set_text(ksdiy_sys_t.ico.lv_clock, LV_SYMBOL_BELL);
	}
	else
	{
		if (ksdiy_sys_t.ico.lv_clock != NULL)
			lv_obj_del(ksdiy_sys_t.ico.lv_clock);
	}
}
void lv_ico_temp_show(uint8_t state)
{
	if (state)
	{
		ksdiy_sys_t.ico.lv_temp = lv_label_create(cont_head, NULL);
		lv_label_set_recolor(ksdiy_sys_t.ico.lv_temp, true);
		lv_obj_set_style_local_text_color(ksdiy_sys_t.ico.lv_temp, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
		lv_label_set_text(ksdiy_sys_t.ico.lv_temp, "#3CB371 " LV_SYMBOL_REFRESH);
	}
	else
	{
		if (ksdiy_sys_t.ico.lv_temp != NULL)
			lv_obj_del(ksdiy_sys_t.ico.lv_temp);
	}
}
void web_temp_save(uint8_t i)
{
	if (i)
	{
		save_nvs("temp", "y");
		lv_message("Temp/Humidity upload on", 1000);
	}
	else
	{
		save_nvs("temp", "n");
		lv_message("Temp/Humidity upload off", 1000);
	}
}
void web_switch_save(uint8_t i)
{
	if (i)
	{
		save_nvs("web_set", "y");
		lv_message("WEB on", 1000);
	}
	else
	{
		save_nvs("web_set", "n");
		lv_message("WEB off", 1000);
	}
}
void cam_switch_save(uint8_t i)
{
	if (i)
	{
		save_nvs("cam_set", "y");
		lv_message("CAM WEB on", 1000);
	}
	else
	{
		save_nvs("cam_set", "n");
		lv_message("CAM WEB off", 1000);
	}
}
void cam_vflip_switch_save(uint8_t i)
{
	if (i)
	{
		save_nvs("cam_vflip", "y");
		lv_message("vflip on", 1000);
	}
	else
	{
		save_nvs("cam_vflip", "n");
		lv_message("vflip off", 1000);
	}
}
void cam_hmirror_switch_save(uint8_t i)
{
	if (i)
	{
		save_nvs("cam_hmirror", "y");
		lv_message("hmirror on", 1000);
	}
	else
	{
		save_nvs("cam_hmirror", "n");
		lv_message("hmirror off", 1000);
	}
}
void clock_switch_save(uint8_t i)
{
	if (i)
	{
		save_nvs("clock", "y");
		lv_message("Alarm on", 1000);
	}
	else
	{
		save_nvs("clock", "n");
		lv_message("Alarm off", 1000);
	}
}
void lv_ksdiy_web_init()
{
	char info[5];
	if (read_nvs("web_set", info)) //Read the web state
	{
		if (!strcmp("y", info)) //Enable web
		{
			if (start_file_server("/spiffs") != ESP_OK) //Start the file system
			{
				ksdiy_sys_t.state.web_set = 0;
				ESP_LOGI("web_set", "web_set start failed");
			}
			ESP_LOGI("web_set", "Starting web_set service");
			ksdiy_sys_t.state.web_set = 1;
		}
		else //Disable
		{
			save_nvs("web_set", "n");
			ksdiy_sys_t.state.web_set = 0;
		}
	}
	else //Not selected on first use
	{
		ESP_LOGI("web_set", "web_set not started");
		ksdiy_sys_t.state.web_set = 0;
	}
	if (ksdiy_sys_t.state.web_set) //Show the icon
	{
		lv_ico_web_show(1);
	}
}
void lv_ksdiy_cam_init()
{
	// app_camera_init();
	char info[5];
	if (read_nvs("cam_vflip", info)) //Read the web state
	{
		if (!strcmp("y", info)) //Enable web
		{

			ksdiy_sys_t.state.cam_vflip = 1;
			sensor_t *s = esp_camera_sensor_get();
			s->set_vflip(s, ksdiy_sys_t.state.cam_vflip);
			ESP_LOGI("web_set", "cam_vflip on");
		}

		else //Disable
		{
			ksdiy_sys_t.state.cam_vflip = 0;
			sensor_t *s = esp_camera_sensor_get();
			s->set_vflip(s, ksdiy_sys_t.state.cam_vflip);
			ESP_LOGI("web_set", "cam_vflip off");
		}
	}
	else //Not selected on first use
	{
		ESP_LOGI("web_set", "cam_vflip not set");
		ksdiy_sys_t.state.cam_vflip = 0;
		ksdiy_sys_t.state.cam_hmirror = 0;
	}
	if (read_nvs("cam_hmirror", info)) //Read the web state
	{
		if (!strcmp("y", info)) //Enable web
		{

			ksdiy_sys_t.state.cam_hmirror = 1;
			sensor_t *s = esp_camera_sensor_get();
			s->set_hmirror(s, ksdiy_sys_t.state.cam_hmirror);
			ESP_LOGI("web_set", "cam_hmirror on");
		}

		else //Disable
		{
			ksdiy_sys_t.state.cam_hmirror = 0;
			sensor_t *s = esp_camera_sensor_get();
			s->set_hmirror(s, ksdiy_sys_t.state.cam_hmirror);
			ESP_LOGI("web_set", "cam_hmirror off");
		}
	}

	else //Not selected on first use
	{
		ESP_LOGI("web_set", "cam_hmirror not set");
		ksdiy_sys_t.state.cam_vflip = 0;
		ksdiy_sys_t.state.cam_hmirror = 0;
	}
	// esp_camera_deinit();
}
void ksdiy_sys_info_init()
{

	ksdiy_sys_t.state.sys_button = 0;
	ksdiy_sys_t.user_data = 0;
	ksdiy_sys_t.wp.follow = get_value_ksdiyfs(1);
	ksdiy_sys_t.wp.following = get_value_ksdiyfs(0);
	lv_ksdiy_web_init();
	// lv_ksdiy_cam_init();

}

PageList_TypeDef PageList[Disp_Max_Page];
// PageList_TypeDef *PageList;
uint8_t PageStack[Disp_Max_Page];
static uint8_t PageStackSize;
static uint8_t PageStackTop;
static uint8_t MaxPage;
static bool IsPageBusy;

#define IS_PAGE(page) ((page) < (MaxPage))

void PageManager(uint8_t pageMax, uint8_t pageStackSize);
bool PageClear(uint8_t pageID);
bool PageRegister(
	uint8_t pageID,
	CallbackFunction_t setupCallback,
	CallbackFunction_t loopCallback,
	CallbackFunction_t exitCallback,
	EventFunction_t eventCallback);
void PageEventTransmit(void *obj, int event);
void PageChangeTo(uint8_t pageID);
bool PagePush(uint8_t pageID);
bool PagePop(void);
void PageStackClear(void);
void Running(void);

_PageManager page = {
	PageManager,
	0,
	0,
	0,
	0,
	0,
	PageRegister,
	PageClear,
	PagePush,
	PagePop,
	PageChangeTo,
	PageEventTransmit,
	PageStackClear,
	Running,
};

/**
  * @brief  Initialize the page scheduler
  * @param  pageMax: maximum number of pages
  * @param  eventMax: maximum number of events
  * @retval None
  */
void PageManager(uint8_t pageMax, uint8_t pageStackSize)
{
	MaxPage = pageMax;
	page.NewPage = 0;
	page.OldPage = 0;
	IsPageBusy = false;

	/* Allocate memory and clear the list */
	// PageList = (PageList_TypeDef *)malloc(sizeof(PageList_TypeDef));
	for (uint8_t page = 0; page < MaxPage; page++)
	{
		PageClear(page);
	}
	/*Page stack configuration*/
	PageStackSize = pageStackSize;
	// PageStack = (uint8_t *)heap_caps_malloc(pageStackSize * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
	// PageStack = (uint8_t *)malloc(pageStackSize);
	PageStackClear();
}

/**
  * @brief  Page scheduler destructor
  * @param  None
  * @retval None
  */
// ~PageManager()
// {
//     delete[] PageList;
//     delete[] PageStack;
// }

/**
  * @brief  Clear one page
  * @param  pageID: page number
  * @retval true:success false:failure
  */
bool PageClear(uint8_t pageID)
{
	if (!IS_PAGE(pageID))
		return false;

	PageList[pageID].SetupCallback = NULL;
	PageList[pageID].LoopCallback = NULL;
	PageList[pageID].ExitCallback = NULL;
	PageList[pageID].EventCallback = NULL;

	return true;
}

/**
  * @brief  Register a basic page, including an initialization function, loop function, exit function, and event function
  * @param  pageID: page number
  * @param  setupCallback: initialization function callback
  * @param  loopCallback: loop function callback
  * @param  exitCallback: exit function callback
  * @param  eventCallback: event function callback
  * @retval true:success false:failure
  */
bool PageRegister(
	uint8_t pageID,
	CallbackFunction_t setupCallback,
	CallbackFunction_t loopCallback,
	CallbackFunction_t exitCallback,
	EventFunction_t eventCallback)
{
	if (!IS_PAGE(pageID))
		return false;

	PageList[pageID].SetupCallback = setupCallback;
	PageList[pageID].LoopCallback = loopCallback;
	PageList[pageID].ExitCallback = exitCallback;
	PageList[pageID].EventCallback = eventCallback;
	return true;
}

/**
  * @brief  Page event transmission
  * @param  obj: object that raised the event
  * @param  event: event ID
  * @retval None
  */
void PageEventTransmit(void *obj, int event)
{
	/*Pass the event to the current page*/
	if (PageList[page.NowPage].EventCallback != NULL)
		PageList[page.NowPage].EventCallback(obj, event);
}

/**
  * @brief  Switch to the specified page
  * @param  pageID: page number
  * @retval None
  */
void PageChangeTo(uint8_t pageID)
{
	if (!IS_PAGE(pageID))
		return;
	/*Check whether the page is busy*/
	if (!IsPageBusy)
	{

		/*New page ID*/
		page.NextPage = page.NewPage = pageID;

		/*Mark as busy*/
		IsPageBusy = true;
	}
}

/**
  * @brief  Push a page onto the stack and jump to it
  * @param  pageID: page number
  * @retval true:success false:failure
  */
bool PagePush(uint8_t pageID)
{
	if (!IS_PAGE(pageID))
		return false;

	/*Check whether the page is busy*/
	if (IsPageBusy)
		return false;
	/*Prevent stack overflow*/
	if (PageStackTop >= PageStackSize - 1)
		return false;
	/*Prevent pushing a duplicate page*/
	if (pageID == PageStack[PageStackTop])
		return false;
	/*Move the stack top pointer up*/
	PageStackTop++;

	/*Push the page onto the stack*/
	PageStack[PageStackTop] = pageID;

	/*Switch page*/
	PageChangeTo(PageStack[PageStackTop]);

	return true;
}

/**
  * @brief  Pop a page from the stack and jump to the previous page
  * @param  None
  * @retval true:success false:failure
  */
bool PagePop()
{
	/*Check whether the page is busy*/
	if (IsPageBusy)
		return false;

	/*Prevent stack overflow*/
	if (PageStackTop == 0)
		return false;

	/*Clear the current page*/
	PageStack[PageStackTop] = 0;

	/*Pop the stack; move the stack top pointer down*/
	PageStackTop--;

	/*Switch page*/
	PageChangeTo(PageStack[PageStackTop]);

	return true;
}

/**
  * @brief  Clear the page stack
  * @param  None
  * @retval None
  */
void PageStackClear()
{
	/*Check whether the page is busy*/
	if (IsPageBusy)
		return;

	/*Clear all data in the stack*/
	for (uint8_t i = 0; i < PageStackSize; i++)
	{
		PageStack[i] = 0;
	}
	/*Reset the stack top pointer*/
	PageStackTop = 0;
}

/**
  * @brief  Page scheduler state machine
  * @param  None
  * @retval None
  */
void Running()
{
	/*Page switch event*/
	if (page.NewPage != page.OldPage)
	{
		/*Mark as busy*/
		IsPageBusy = true;

		/*Trigger the old page's exit event*/
		if (PageList[page.OldPage].ExitCallback != NULL && IS_PAGE(page.OldPage))
			PageList[page.OldPage].ExitCallback();

		/*Mark the old page*/
		page.LastPage = page.OldPage;

		/*Mark the new page as the current page*/
		page.NowPage = page.NewPage;

		/*Trigger the new page's initialization event*/
		if (PageList[page.NewPage].SetupCallback != NULL && IS_PAGE(page.NewPage))
			PageList[page.NewPage].SetupCallback();
		/*New page initialization complete; mark it as the old page*/
		page.OldPage = page.NewPage;
	}
	else
	{
		/*Mark the page as not busy; it is in the loop state*/
		IsPageBusy = false;

		/*Page loop event*/
		if (PageList[page.NowPage].LoopCallback != NULL && IS_PAGE(page.NowPage))
			PageList[page.NowPage].LoopCallback();
	}
}
