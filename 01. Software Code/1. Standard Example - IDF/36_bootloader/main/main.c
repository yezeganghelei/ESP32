

#include "nvs_flash.h"
#include "iic.h"
#include "xl9555.h"
#include "lvgl_demo.h"
#include "spi.h"
#include "key.h"
#include "led.h"
#include "24cxx.h"
#include "manage.h"
#include "app_test.h"
#include "adc1.h"
#include "qma6100p.h"
#include "es8388.h"
#include "i2s.h"
#include "exfuns.h"
#include "esp_rtc.h"


i2c_obj_t i2c0_master;


void app_main(void)
{
    esp_err_t ret;
    
    ret = nvs_flash_init();             /*  */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    i2c0_master = iic_init(I2C_NUM_0);  /*  */
    spi2_init();                        /*  */
    led_init();                         /*  */
    key_init();                         /*  */
    xl9555_init(i2c0_master);           /*  */
    at24cxx_init(i2c0_master);          /*  */
    ap3216c_init(i2c0_master);          /*  */
    qma6100p_init(i2c0_master);         /*  */
    es8388_init(i2c0_master);           /*  */
    rtc_set_time(2023,12,26,11,32,00);  /*  */
    lcd_init();                         /*  */
    exfuns_init();                      /*  */
    func_test();                        /*  */
    lvgl_demo();
}
