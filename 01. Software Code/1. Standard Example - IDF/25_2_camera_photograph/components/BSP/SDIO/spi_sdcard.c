/**
 ****************************************************************************************************
 * @file        spi_sdcard.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       SDCard Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "spi_sdcard.h"

spi_device_handle_t MY_SD_Handle = NULL;                            /* SD card handle */
sdmmc_card_t *card;                                                 /* SD/MMC card structure */
const char mount_point[] = MOUNT_POINT;                             /* Mounting point/Root directory */
esp_err_t ret = ESP_OK;
esp_err_t mount_ret = ESP_OK;

/**
 * @brief       SDCard initialization
 * @param       none
 * @retval      esp_err_t
 */
esp_err_t sd_spi_init(void)
{
    if (MY_SD_Handle != NULL)                                       /* Mount or initialize againSDCard */
    {
        spi_bus_remove_device(MY_SD_Handle);                        /* Remove SD card device on SPI */

        if (mount_ret == ESP_OK)
        {
            esp_vfs_fat_sdcard_unmount(mount_point, card);          /* Unmount */
        }
    }

    /* SPIDriver interface configuration,SPISDCardclockyes20-25MHz */
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20 * 1000 * 1000,                         /* SPI clock */
        .mode = 0,                                                  /* SPI mode 0 */
        .spics_io_num = SD_NUM_CS,                                  /* Piece selection pins */
        .queue_size = 7,                                            /* Transaction queue size 7indivual */
    };

    /* Add SPI bus device */
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &MY_SD_Handle);

    /* File system mount configuration */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,                            /* If the mount fails：trueWill be repartitioned and formatted/falseWill not repartition and format */
        .max_files = 5,                                             /* Maximum number of open files */
        .allocation_unit_size = 4 * 1024 * sizeof(uint8_t)          /* The size of the hard disk partition cluster */
    };

    /* SDCardParameter configuration */
    sdmmc_host_t host = {0};
    host.flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_DEINIT_ARG;  /* Flags that define host properties: SPI protocol and call deinit function */
    host.slot = SPI2_HOST;                                          /* useSPI2port */
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;                         /* The maximum frequency supported by the host: 20000 */
    host.io_voltage = 3.3f;                                         /* The controller is usedI/OVoltage */
    host.init = &sdspi_host_init;                                   /* The host used to initialize the driverfunction */
    host.set_bus_width = NULL;                                      /* Host function to set bus width */
    host.get_bus_width = NULL;                                      /* The host function that takes the bus width */
    host.set_bus_ddr_mode = NULL;                                   /* Setting the host function in DDR mode */
    host.set_card_clk = &sdspi_host_set_card_clk;                   /* set upplateCardclockFrequency hostfunction */
    host.do_transaction = &sdspi_host_do_transaction;               /* Host function that performs transactions */
    host.deinit_p = &sdspi_host_remove_device;                      /* Host used to uninitialize driversfunction */
    host.io_int_enable = &sdspi_host_io_int_enable;                 /* EnableSDIOHost function of interrupt line */
    host.io_int_wait = &sdspi_host_io_int_wait;                     /* Host function waiting for SDIO to interrupt line activation */
    host.command_timeout_ms = 0;                                    /* time out，Default is0*/
    
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
    host.get_real_freq = &sdspi_host_get_real_freq;
    host.set_cclk_always_on = NULL;
#endif

    /* SDCardPin configuration */
    sdspi_device_config_t slot_config = {0};
    slot_config.host_id   = host.slot;
    slot_config.gpio_cs   = SD_NUM_CS;
    slot_config.gpio_cd   = GPIO_NUM_NC;
    slot_config.gpio_wp   = GPIO_NUM_NC;
    slot_config.gpio_int  = GPIO_NUM_NC;

    mount_ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);      /* Mount the file system */
    ret |= mount_ret;

    return ret;
}

/**
 * @brief       GetSDCardRelated information
 * @param       out_total_bytes：Total size
 * @param       out_free_bytes：Remaining size
 * @retval      none
 */
void sd_get_fatfs_usage(size_t *out_total_bytes, size_t *out_free_bytes)
{
    FATFS *fs;
    size_t free_clusters;
    int res = f_getfree("0:", (size_t *)&free_clusters, &fs);
    assert(res == FR_OK);
    size_t total_sectors = (fs->n_fatent - 2) * fs->csize;
    size_t free_sectors = free_clusters * fs->csize;

    size_t sd_total = total_sectors / 1024;
    size_t sd_total_KB = sd_total * fs->ssize;
    size_t sd_free = free_sectors / 1024;
    size_t sd_free_KB = sd_free*fs->ssize;

    /* Assume that the total size is smaller than4GiB，forSPI FlashIt should betrue */
    if (out_total_bytes != NULL)
    {
        *out_total_bytes = sd_total_KB;
    }
    
    if (out_free_bytes != NULL)
    {
        *out_free_bytes = sd_free_KB;
    }
}