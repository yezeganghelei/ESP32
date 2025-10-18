/**
 ****************************************************************************************************
 * @file        gif.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image decoding-gifdecoding code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20200404
 * First release

 ****************************************************************************************************
 */

#ifndef __GIF_H
#define __GIF_H

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "ff.h"

/******************************************************************************************/
/* User Configuration Area */

#define GIF_USE_MALLOC          1       /* Define whether to use malloc, here we choose to use malloc */

/******************************************************************************************/

#define LCD_MAX_LOG_COLORS      256
#define MAX_NUM_LWZ_BITS        12

#define GIF_INTRO_TERMINATOR    ';'    /* 0X3B   GIFdocumentEnd symbol */
#define GIF_INTRO_EXTENSION     '!'     /* 0X21 */
#define GIF_INTRO_IMAGE         ','     /* 0X2C */

#define GIF_COMMENT             0xFE
#define GIF_APPLICATION         0xFF
#define GIF_PLAINTEXT           0x01
#define GIF_GRAPHICCTL          0xF9

typedef struct
{
    uint8_t    aBuffer[258];                    /*  Input buffer for data block */
    short aCode  [(1 << MAX_NUM_LWZ_BITS)];     /*  This array stores the LZW codes for the compressed strings */
    uint8_t    aPrefix[(1 << MAX_NUM_LWZ_BITS)];/*  Prefix character of the LZW code */
    uint8_t    aDecompBuffer[3000];             /*  Decompression buffer. The higher the compression, the more bytes are needed in the buffer */
    uint8_t   *sp;                              /*  Pointer into the decompression buffer */
    int   CurBit;
    int   LastBit;
    int   GetDone;
    int   LastByte;
    int   ReturnClear;
    int   CodeSize;
    int   SetCodeSize;
    int   MaxCode;
    int   MaxCodeSize;
    int   ClearCode;
    int   EndCode;
    int   FirstCode;
    int   OldCode;
} LZW_INFO;

/* Logical screen description block */
typedef struct
{
    uint16_t width;     /* GIFwidth */
    uint16_t height;    /* GIF height */
    uint8_t flag;       /* Identifier  1:3:1:3=Global color table logo(1):Color depth(3):Classification mark(1):overall situationColor table size(3) */
    uint8_t bkcindex;   /* Index of background color in global color table(Only valid if a global color table exists) */
    uint8_t pixratio;   /* Pixel aspect ratio */
} LogicalScreenDescriptor;

/* Image description block */
typedef struct
{
    uint16_t xoff;      /* xDirection offset */
    uint16_t yoff;      /* yDirection offset */
    uint16_t width;     /* width */
    uint16_t height;    /* high */
    uint8_t flag;       /* Identifier 1:1:1:2:3=Local color table logo (1):Interleaved flag (1):Reserved (2):Local color table size (3) */
} ImageScreenDescriptor;

/* Image description */
typedef struct
{
    LogicalScreenDescriptor gifLSD; /* Logical screen description block */
    ImageScreenDescriptor gifISD;   /* Image descriptionquick */
    uint16_t colortbl[256];         /* Currently used color table */
    uint16_t bkpcolortbl[256];      /* Backup color table.Used when a local color table exists */
    uint16_t numcolors;             /* Color table size */
    uint16_t delay;                 /* Delay time */
    LZW_INFO *lzw;                  /* LZW Information */
} gif89a;

extern uint8_t g_gif_decoding;      /* GIFDecoding marks */

/* GIF codec interface function */
void gif_quit(void);    /* Exit the current decoding */
uint8_t gif_getinfo(FIL *file, gif89a *gif);    /* GetGIFinformation */
uint8_t gif_decode(const char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height);/* Decode a GIF file in a specified area */

#endif