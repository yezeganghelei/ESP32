/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       SPIFFSexperiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"
#include "iic.h"
#include "spi.h"
#include "lcd.h"
#include "xl9555.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "nvs_flash.h"

i2c_obj_t i2c0_master;

#define DEFAULT_FD_NUM          5
#define DEFAULT_MOUNT_POINT     "/spiffs"
#define WRITE_DATA              "ALIENTEK ESP32-S3\r\n"
static const char               *TAG = "spiffs";

/**
 * @brief       spiffsinitialization
 * @param       partition_label:Partition name of the partition table
 * @param       mount_point:File systemThe associated file path prefix
 * @param       max_files:The maximum number of files that can be opened at the same time
 * @retval      none
 */
esp_err_t spiffs_init(char *partition_label, char *mount_point, size_t max_files)
{
    /* ConfigurationspiffsFile system parameters */
    esp_vfs_spiffs_conf_t conf = {
        .base_path = mount_point,
        .partition_label = partition_label,
        .max_files = max_files,
        .format_if_mount_failed = true,
    };

    /* Use the settings defined above to initialize and mount the SPIFFS file system */
    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    /* judgeSPIFFSIs the mount and initialization successful? */
    if (ret_val != ESP_OK)
    {
        if (ret_val == ESP_FAIL)
        {
            printf("Failed to mount or format filesystem\n");
        }
        else if (ret_val == ESP_ERR_NOT_FOUND)
        {
            printf("Failed to find SPIFFS partition\n");
        }
        else
        {
            printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret_val));
        }

        return ESP_FAIL;
    }

    /* Print SPIFFS storage information */
    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);

    if (ret_val != ESP_OK)
    {
        ESP_LOGI(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}

/**
 * @brief       Log outspiffsinitialization
 * @param       partition_label：Partition table identification
 * @retval      none
 */
esp_err_t spiffs_deinit(char *partition_label)
{
    return esp_vfs_spiffs_unregister(partition_label);
}

/**
 * @brief test spiffs
 * @param None
 * @retval None
 */
void spiffs_test(void)
{

    ESP_LOGI(TAG, "Opening file");
    /* Create a name called/spiffs/hello.txtWrite only files */
    FILE* f = fopen("/spiffs/hello.txt", "w");

    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    /* Write characters */
    fprintf(f, WRITE_DATA);

    fclose(f);
    ESP_LOGI(TAG, "File written");
 
    /* Check whether the target file exists before renaming */
    struct stat st;

    if (stat("/spiffs/foo.txt", &st) == 0) /* Get file information, return 0 after successful acquisition */
    {
        /*  fromFile systemDelete a name in。
            If the name is the last connection to the file，And no other process opens the file，
            The file corresponding to the name will be actually deleted。 */
        unlink("/spiffs/foo.txt");
    }
 
    /* Rename the created file */
    ESP_LOGI(TAG, "Renaming file");

    if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0)
    {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }
 
    /* Open the renamed file and read it */
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/spiffs/foo.txt", "r");

    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    
    char* pos = strchr(line, '\n'); /* Pos point to the first find '\n' */

    if (pos)
    {
        *pos = '\0';                /* Will‘\n’Replace with‘\0’ */
    }

    ESP_LOGI(TAG, "Read from file: '%s'", line);

    lcd_show_string(90, 110, 200, 16, 16, line, RED);
}

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();                                         /* Initialize NVS */
    
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    led_init();                                                     /* LCDinitialization */
    i2c0_master = iic_init(I2C_NUM_0);                              /* initializationIIC0 */
    spi2_init();                                                    /* SPIinitialization */
    xl9555_init(i2c0_master);                                       /* XL9555initialization */
    lcd_init();                                                     /* LCDinitialization */
    spiffs_init("storage", DEFAULT_MOUNT_POINT, DEFAULT_FD_NUM);    /* SPIFFS Initialization */

    /* Display experimental information */
    lcd_show_string(10, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(10, 70, 200, 16, 16, "SPIFFS TEST", RED);
    lcd_show_string(10, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(10, 110, 200, 16, 16, "Read file:", BLUE);

    spiffs_test();                                                  /* SPIFFS Testing */

    while (1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}