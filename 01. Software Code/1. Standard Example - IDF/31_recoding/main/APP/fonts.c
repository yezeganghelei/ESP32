/**
 ****************************************************************************************************
 * @file        font.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Font library Code
 *              Provides fonts_update_font and fonts_init for font library update and initialization
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "fonts.h"

/* Total sectors occupied by the font library area (3 font libraries + UNIGBK table + font library info = 3238700 bytes, approximately 791 sectors, 4 Kbytes per sector) */
#define FONTSECSIZE         791
/* Each operation is limited to 4K Within */
#define SECTOR_SIZE         0X1000

/* Starting address where the font library is stored in the partition
 * UNIGBK.BIN, total size 3.09M, 791 sectors, occupied by the font library, cannot be moved!
 */
#define FONTINFOADDR        0

/* Used to save basic font library information: address, size, etc. */
_font_info ftinfo;

static const char *TAG = "storage_partition";
const esp_partition_t *storage_partition;

/* Path where the font library is stored on the disk */
char *const FONT_GBK_PATH[4] =
{
    "/SYSTEM/FONT/UNIGBK.BIN",      /* UNIGBK.BINStorage location */
    "/SYSTEM/FONT/GBK12.FON",       /* GBK12Storage location */
    "/SYSTEM/FONT/GBK16.FON",       /* GBK16Storage location */
    "/SYSTEM/FONT/GBK24.FON",       /* Storage location of GBK24 */
};

/* Update prompt information */
char *const FONT_UPDATE_REMIND_TBL[4] =
{
    "Updating UNIGBK.BIN",          /* Prompt updateUNIGBK.bin */
    "Updating GBK12.FON ",          /* Tip: GBK12 is being updated */
    "Updating GBK16.FON ",          /* Tip: GBK16 is being updated */
    "Updating GBK24.FON ",          /* Prompt updateGBK24 */
};

/**
 * @brief       Partition table reads data
 * @param       buffer    : Storage area for read data
 * @param       offset    : Starting address of the data to read
 * @param       length    : Read size
 * @retval      ESP_OK: indicates success; other: indicates failure
 */
esp_err_t fonts_partition_read(void * buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL || (length > SECTOR_SIZE))
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_read(storage_partition, offset, buffer,length);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash read failed.");
        return err;
    }
    
    return err;
}

/**
 * @brief       Write data to the partition
 * @param       buffer    : The storage area for writing data
 * @param       offset    : Starting address of the data to write
 * @param       length    : Write size
 * @retval      ESP_OK: indicates success; other: indicates failure
 */
esp_err_t fonts_partition_write(void * buffer, uint32_t offset, uint32_t length)
{
    esp_err_t err;

    if (buffer == NULL || (length > SECTOR_SIZE))
    {
        ESP_LOGE(TAG, "ESP_ERR_INVALID_ARG");
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_partition_write(storage_partition, offset, buffer, length);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash write failed.");
        return err;
    }

    return err;
}

/**
 * @brief       Erase certain sectors
 * @param       offset    : Erase start address
 * @retval      ESP_OK: indicates success; other: indicates failure
 */
esp_err_t fonts_partition_erase_sector(uint32_t offset)
{
    esp_err_t err;

    err = esp_partition_erase_range(storage_partition, offset, SECTOR_SIZE);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Flash erase failed.");
        return err;
    }

    return err;
}

/**
 * @brief       Show current font update progress
 * @param       x, y    : coordinate
 * @param       size    : Font size
 * @param       totsize : Total file size
 * @param       pos     : Current file pointer location
 * @param       color   : Font color
 * @retval      none
 */
void fonts_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100)t = 100;

        lcd_show_num(x, y, t, 3, size, color);  /* Display value */
    }
}

/**
 * @brief       Update a font library
 * @param       x, y    : Display address of the prompt information
 * @param       size    : Font size of the prompt information
 * @param       fpath   : Font path
 * @param       fx      : Content to update
 *   @arg                 0, ungbk;
 *   @Arg                 1, gbk12;
 *   @arg                 2, gbk16;
 *   @arg                 3, gbk24;
 * @param       color   : Font color
 * @retval      0, success; other, Error code;
 */
static uint8_t fonts_update_fontx(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;
    fftemp = (FIL *)malloc(sizeof(FIL));  /* Allocate memory */

    if (fftemp == NULL)rval = 1;

    tempbuf = malloc(4096);               /* distribute4096byte space */

    if (tempbuf == NULL)rval = 1;

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);

    if (res)rval = 2;   /* Failed to open the file */

    if (rval == 0)
    {
        switch (fx)
        {
            case 0: /* Update UNIGBK.BIN */
                ftinfo.ugbkaddr = FONTINFOADDR + sizeof(ftinfo);    /* Follow the information header, follow the UNIGBK conversion code table */
                ftinfo.ugbksize = fftemp->obj.objsize;              /* UNIGBKsize */
                flashaddr = ftinfo.ugbkaddr;
                break;

            case 1: /* Update GBK12.BIN */
                ftinfo.f12addr = ftinfo.ugbkaddr + ftinfo.ugbksize; /* UNIGBKafter，FollowGBK12Font library */
                ftinfo.gbk12size = fftemp->obj.objsize;             /* GBK12Font librarysize */
                flashaddr = ftinfo.f12addr;                         /* GBK12The starting address of */
                break;

            case 2: /* Update GBK16.BIN */
                ftinfo.f16addr = ftinfo.f12addr + ftinfo.gbk12size; /* After GBK12, follow GBK16 font library */
                ftinfo.gbk16size = fftemp->obj.objsize;             /* GBK16Font librarysize */
                flashaddr = ftinfo.f16addr;                         /* GBK16The starting address of */
                break;

            case 3: /* renew GBK24.BIN */
                ftinfo.f24addr = ftinfo.f16addr + ftinfo.gbk16size; /* GBK16after，FollowGBK24Font library */
                ftinfo.gbk24size = fftemp->obj.objsize;             /* GBK24Font librarysize */
                flashaddr = ftinfo.f24addr;                         /* The starting address of GBK24 */
                break;
        }

        while (res == FR_OK)   /* Violent loop execution */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* Read data */

            if (res != FR_OK)break;     /* execution error */

            fonts_partition_write(tempbuf, offx + flashaddr, bread);       /* from0Start writingbreadData */
            offx += bread;
            fonts_progress_show(x, y, size, fftemp->obj.objsize, offx, color);    /* Progress display */

            if (bread != 4096) break;    /* Finished reading */
        }

        f_close(fftemp);
    }

    free(fftemp);     /* Free memory */
    free(tempbuf);    /* Free memory */
    return res;
}

/**
 * @brief       Update font files
 *   @note      Update all font libraries together (UNIGBK, GBK12, GBK16, GBK24)
 * @param       x, y    : Display address of the prompt information
 * @param       size    : Font size of the prompt information
 * @param       src     : Font library source disk
 *   @arg                 "0:", SD card;
 * @Arg                 "1:", FLASHplate
 * @param       color   : Font color
 * @retval      0, success; other, Error code;
 */
uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0XFF;
    ftinfo.fontok = 0XFF;
    pname = malloc(100);                  /* Apply for 100 bytes of memory */
    buf = malloc(4096);                   /* Apply4KByte memory */
    fftemp = (FIL *)malloc(sizeof(FIL));  /* Allocate memory */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        free(fftemp);
        free(pname);
        free(buf);
        return 5;   /* Memory application failed */
    }

    for (i = 0; i < 4; i++) /* Find the file firstUNIGBK,GBK12,GBK16,GBK24 Is it normal */
    {
        strcpy((char *)pname, (char *)src);                 /* copy src content to pname */
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);    /* Append specific file path */
        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);/* Try to open */

        if (res)
        {
            rval |= 1 << 7; /* Tag failed to open file */
            break;          /* Something went wrong,Exit directly */
        }
    }

    free(fftemp); /* Free memory */

    if (rval == 0)          /* All font library files exist. */
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);    /* Tip: Sectors are being erased */

        for (i = 0; i < FONTSECSIZE; i++)       /* Erase the font library area first to improve writing speed */
        {
            fonts_progress_show(x + 20 * size / 2, y, size, FONTSECSIZE, i, color);     /* Progress display */
            fonts_partition_read((uint8_t *)buf, ((FONTINFOADDR / 4096) + i) * 4096, 4096);    /* Read out the contents of entire sectors */

            for (j = 0; j < 1024; j++)          /* Verify data */
            {
                if (buf[j] != 0XFFFFFFFF)break; /* Need to be erased */
            }

            if (j != 1024)
            {
                fonts_partition_erase_sector(((FONTINFOADDR / 4096) + i) * 4096); /* Sectors to be erased */
            }
        }

        for (i = 0; i < 4; i++) /* Update UNIGBK, GBK12, GBK16, GBK24 in sequence */
        {
            lcd_show_string(x, y, 240, 320, size, FONT_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);             /* copy src content to pname */
            strcat((char *)pname, (char *)FONT_GBK_PATH[i]);/* Append specific file path */
            res = fonts_update_fontx(x + 20 * size / 2, y, size, pname, i, color);    /* renewFont library */

            if (res)
            {
                free(buf);
                free(pname);
                return 1 + i;
            }
        }

        /* All updates completed */
        ftinfo.fontok = 0XAA;
        fonts_partition_write((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));    /* Save font library information */
    }

    free(pname);  /* Free memory */
    free(buf);
    return rval;            /* No errors */
}

/**
 * @brief       Initialize the font
 * @param       none
 * @retval      0, font library intact; other, font library missing;
 */
uint8_t fonts_init(void)
{
    uint8_t t = 0;

    storage_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY,"storage");
    
    if (storage_partition == NULL)
    {
        ESP_LOGE(TAG, "Flash partition not found.");
        return 1;
    }
    
    while (t < 10)  /* Continuous reading10Second-rate,All are wrong,It means there is indeed a problem,Need to update the font library */
    {
        t++;
        fonts_partition_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo)); /* Read outftinfoStructure data */

        if (ftinfo.fontok == 0XAA)
        {
            break;
        }
        
        vTaskDelay(20);
    }

    if (ftinfo.fontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}