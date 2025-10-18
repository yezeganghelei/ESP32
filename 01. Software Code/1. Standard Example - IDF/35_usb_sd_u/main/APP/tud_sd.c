/**
 ****************************************************************************************************
 * @file        tud_sd.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       SDcard emulationUplate（USB）code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "tud_sd.h"

static const char *TAG = "usb_msc";
const char *disk_path = "/disk";                /* 磁plate的路径 */
static uint8_t s_pdrv = 0;                      /* Physical drives used to identify drives */
static int s_disk_block_size = 0;               /* 磁plate块的大小 */
#define LOGICAL_DISK_NUM        1               /* Number of disks */
static bool ejected[LOGICAL_DISK_NUM] = {true}; /* Pop-up status */
__usbdev g_usbdev;                              /* USB controller */
//--------------------------------------------------------------------+
// The following is the USB callback function, which is generally used to determine the connection process.
//--------------------------------------------------------------------+

/**
 * @brief       During installationUSBThis function is called when the device
 * @param       none
 * @retval      none
 */
void tud_mount_cb(void)
{
    /* Each insertionUSBReset and track。This way you can get the insert，Pop-up status，It is beneficial to re-plug the drive */
    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++)
    {
        ejected[i] = false;
    }

    g_usbdev.status |= 0x01;

    ESP_LOGI(__func__, "");
}

/**
 * @brief This function is called when the USB device is uninstalled
 * @param none
 * @retval None
 */
void tud_umount_cb(void)
{
    ESP_LOGW(__func__, "");
}

/**
 * @brief This function is called when the USB bus is suspended
 * @param none
 * @retval None
 */
void tud_suspend_cb(bool remote_wakeup_en)
{
    g_usbdev.status &= 0x00;
    ESP_LOGW(__func__, "");
}

/**
 * @brief       recoverUSBCalled on the bus
 * @param       none
 * @retval      none
 */
void tud_resume_cb(void)
{
    ESP_LOGW(__func__, "");
}

/**
 * @brief       Used to flush any pending caches
 * @param       none
 * @retval      none
 */
void tud_msc_write10_complete_cb(uint8_t lun)
{
    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    /* This write is completed，Start automatic reloading of the clock */
    ESP_LOGD(__func__, "");
}

/**
 * @brief       已弹出磁plate
 * @param       none
 * @retval      none
 */
static bool _logical_disk_ejected(void)
{
    bool all_ejected = true;

    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++)
    {
        all_ejected &= ejected[i];
    }

    return all_ejected;
}

/**
 * @brief       receiveSCSI_CMD_INQUIRYThis function is called when，用于从目标设备获取基本信息
 * @param       lun         :磁plate数量
 * @param       vendor_id   :supplierid
 * @param       product_id  :productid
 * @param       product_rev :revised version
 * @retval      none
 */
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    const char vid[] = "ESPS3-S3";
    const char pid[] = "Mass Storage";
    const char rev[] = "1.0";

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

/**
 * @brief       receive测试单元就绪Called on command
 * @param       lun:磁plate数量
 * @retval      trueAllow host to read/write thisLUN，For example, insertedSDCard
 */
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (_logical_disk_ejected())
    {
        /* 为不存在的磁plate设置0x3a */
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
        return false;
    }

    return true;
}

/**
 * @brief       当receiveSCSI_CMD_READ_CAPACITY_10andSCSI_CMD _READ_FORMAT_CCAPITATIONThis function is called when，以确定磁plate大小
 * @param       lun         :磁plate数量
 * @param       block_count :Number of blocks
 * @param       block_size  :block size
 * @retval      trueAllow host to read/write thisLUN，For example, insertedSDCard
 */
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    disk_ioctl(s_pdrv, GET_SECTOR_COUNT, block_count);
    disk_ioctl(s_pdrv, GET_SECTOR_SIZE, block_size);
    s_disk_block_size = *block_size;
    ESP_LOGD(__func__, "GET_SECTOR_COUNT = %"PRIu32"，GET_SECTOR_SIZE = %d", *block_count, *block_size);
}

/**
 * @brief       Called to check if the device is available asSCSI
 * @param       lun         :磁plate数量
 * @retval      true:Can;false:不Can
 */
bool tud_msc_is_writable_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    return true;
}

/**
 * @brief       receive“start up-Stop unit”Called on command
 * @param       lun             :磁plate数量
 * @param       power_condition :Power conditions
 * @param       start           :Start = 0：Stop power mode;Start = 1：Activity Mode
 * @param       load_eject      :Start = 0,load_eject = 1：卸载磁plate存储;Start = 1,load_eject=1：加载磁plate存储
 * @retval      true:加载磁plate存储成功;false:卸载磁plate存储成功
 */
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    ESP_LOGI(__func__, "");
    (void) power_condition;

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (load_eject)
    {
        if (!start)
        {
            /* 弹出磁plate */
            if (disk_ioctl(s_pdrv, CTRL_SYNC, NULL) != RES_OK)
            {
                return false;
            }
            else
            {
                ejected[lun] = true;
            }
        }
        else
        {
            /* Can only be loaded if it doesn't pop up */
            return !ejected[lun];
        }
    }
    else
    {
        if (!start)
        {
            /* Stop device，But it doesn't pop up */
            if (disk_ioctl(s_pdrv, CTRL_SYNC, NULL) != RES_OK)
            {
                return false;
            }
        }

        /* Always start the device，Even if it pops up */
    }

    return true;
}

/**
 * @brief       receiveREAD10Call this function when command
 * @param       lun     :磁plate数量
 * @param       lba     :Block address
 * @param       offset  :Data offset
 * @param       buffer  :Read data storage area
 * @param       bufsize :Read data size
 * @retval      Returns the number of read bytes
 */
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    const uint32_t block_count = bufsize / s_disk_block_size;
    /* 磁plate读取 */
    disk_read(s_pdrv, buffer, lba, block_count);

    return block_count * s_disk_block_size;
}

/**
 * @brief       receiveWRITE10Call this function when command
 * @param       lun     :磁plate数量
 * @param       lba     :Block address
 * @param       offset  :write offset
 * @param       buffer  :The storage area for writing data
 * @param       bufsize :Write data size
 * @retval      Returns the number of bytes written
 */
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");
    (void) offset;

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    const uint32_t block_count = bufsize / s_disk_block_size;
    /* 磁plate写入 */
    disk_write(s_pdrv, buffer, lba, block_count);

    return block_count * s_disk_block_size;
}

/**
 * @brief       When receiving a message that is not in the built-in list belowSCSIThis function is called when commanding
 * @param       lun         :磁plate数量
 * @param       scsi_cmd    :scsiCommand content，应用程序必须检查该Command content才能做出相应响应
 * @param       buffer      :SCSIBuffer for data stage
 * @param       bufsize     :buffer length
 * @retval      Returns the number of bytes written
 */
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    /* read10&write10 has its own callback and cannot be processed here */
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM)
    {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    void const *response = NULL;
    uint16_t resplen = 0;

    /* Most of the dealscsiAll are inputs */
    bool in_xfer = true;

    switch (scsi_cmd[0])
    {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            /* The host is about to be read/Write, etc.。。。最好不要断开磁plate连接 */
            resplen = 0;
            break;

        default:
            /* Set invalid command operation */
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

            /* Negation means error->tinyusb可能会暂停and/Or respond in a failed state */
            resplen = -1;
            break;
    }

    /* resplenNot greater thanbufsize */
    if (resplen > bufsize)
    {
        /* if greater than，butresplen = bufsize */
        resplen = bufsize;
    }

    if (response && (resplen > 0))
    {
        if (in_xfer)
        {
            memcpy(buffer, response, resplen);
        }
        else
        {
            /* SCSI output */
        }
    }

    return resplen;
}

//--------------------------------------------------------------------+
// The above isUSBcallback function，Generally used to judge the connection process
//--------------------------------------------------------------------+

/**
 * @brief       SDcard emulationUplate函数初始化
 * @param       none
 * @retval      none
 */
void tud_usb_sd(void)
{
    const tinyusb_config_t tusb_cfg = {0};
    /* USBEquipment registration */
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB MSC initialization DONE");
}