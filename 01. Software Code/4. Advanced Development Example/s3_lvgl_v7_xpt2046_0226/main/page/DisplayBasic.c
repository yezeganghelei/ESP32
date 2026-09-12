
#include "DisplayPrivate.h"
#include "page_menu.h"


/* Page registrar */
#define PAGE_REG(name)                               \
  do                                                 \
  {                                                  \
    extern void PageRegister_##name(uint8_t pageID); \
    PageRegister_##name(Disp_##name);                \
  } while (0)

/**
  * @brief  Page initialization
  * @param  none
  * @retval none
  */
static void Pages_Init()
{
  page.PageManager(Disp_Max_Page, Disp_Max_Page);

  PAGE_REG(Home);
  PAGE_REG(Menu);
  PAGE_REG(Cam);
  PAGE_REG(Color);
  PAGE_REG(Face);
  PAGE_REG(Baidu);
  PAGE_REG(IMU);
  PAGE_REG(FFT);
  PAGE_REG(Daily);
  PAGE_REG(Game_2048);
  PAGE_REG(Game_Snake);
  PAGE_REG(Music);
  PAGE_REG(Calendar);
  PAGE_REG(Setting);
  PAGE_REG(About);

  page.PagePush(Disp_Home); //Open the home page
}

/**
  * @brief  Display initialization
  * @param  none
  * @retval none
  */
void Display_Init()
{

  /*APP window initialization*/
  // AppWindow_Create();

  /*Page initialization*/
  Pages_Init();
}

/**
  * @brief  Display update
  * @param  none
  * @retval none
  */
void Display_Update()
{
  lv_task_handler();
  page.Running();
}

/**
  * @brief  Blocking page delay that keeps LVGL updating
  * @param  none
  * @retval none
  */
void PageDelay(uint32_t ms)
{
  uint32_t lastTime = xTaskGetTickCount();
  while (xTaskGetTickCount() - lastTime <= ms)
  {
    lv_task_handler();
  }
}
