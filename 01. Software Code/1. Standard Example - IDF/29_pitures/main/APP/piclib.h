/**
 ****************************************************************************************************
 * @file        piclib.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image decoding library Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20200404
 * First release

 ****************************************************************************************************
 */

#ifndef __PICLIB_H
#define __PICLIB_H

#include "lcd.h"
#include <unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "ff.h"
#include "exfuns.h"
#include "gif.h"
#include "bmp.h"
#include "jpeg.h"
#include "png.h"

#define PIC_FORMAT_ERR      0x27    /* Error in format */
#define PIC_SIZE_ERR        0x28    /* Image size error */
#define PIC_WINDOW_ERR      0x29    /* Window setting error */
#define PIC_MEM_ERR         0x11    /* memory error */

/* judge TRUE and FALSE Is it already defined?, If not, define it! */
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

/* Picture display physical layer interface */
/* During porting, the user must implement these functions themselves */
typedef struct
{
    /* void draw_point(uint16_t x,uint16_t y,uint32_t color) Draw a dot function */
    void(*draw_point)(uint16_t, uint16_t, uint16_t);
    
    /* void fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color) Monochrome fill function */
    void(*fill)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    
    /* void draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color) draw horizontal line function */
    void(*draw_hline)(uint16_t, uint16_t, uint16_t, uint16_t);
    
    /* void piclib_fill_color(uint16_t x,uint16_t y,uint16_t size,uint16_t *color) Multi-point fill */
    void(*multicolor)(uint16_t, uint16_t, uint16_t, uint16_t *);
} _pic_phy;

extern _pic_phy pic_phy;

/* Image information */
typedef struct
{
    uint16_t lcdwidth;      /* The width of the LCD */
    uint16_t lcdheight;     /* LCDheight */
} _pic_info;

extern _pic_info picinfo;   /* Image information */

/* Image decoding library Interface functions */
void piclib_mem_free (void *paddr);     /* Free memory */
void *piclib_mem_malloc (uint32_t size);/* Apply for memory */
void piclib_init(void);                 /* Initialize drawing */
uint8_t piclib_ai_load_picfile(char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height); /* Smart drawing */

#endif