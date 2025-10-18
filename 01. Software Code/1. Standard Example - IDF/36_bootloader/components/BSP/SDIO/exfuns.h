/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file exfuns.h
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief FATFS extension code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#ifndef __EXFUNS_H
#define __EXFUNS_H

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "ff.h"

extern FATFS *fs[FF_VOLUMES];
extern FIL *file;
extern FIL *ftemp;
extern UINT br, bw;
extern FILINFO fileinfo;
extern FF_DIR dir;
extern uint8_t *fatbuf;     /* SDCard data cache area */

/* The type definition returned by exfuns_file_type
 * Obtained according to the table FILE_TYPE_TBL. Defined in exfuns.c
 */
#define T_BIN   0x00    /* BINdocument */
#define T_LRC   0x10    /* LRC files */
#define T_NES   0x20    /* NESdocument */
#define T_SMS   0x21    /* SMS files */
#define T_TEXT  0x30    /* TXTdocument */
#define T_C     0x31    /* Cdocument */
#define T_H     0x32    /* Hdocument */
#define T_WAV   0x40    /* WAVdocument */
#define T_MP3   0x41    /* MP3document */
#define T_APE   0x42    /* APEdocument */
#define T_FLAC  0x43    /* FLAC Files */
#define T_BMP   0x50    /* BMPdocument */
#define T_JPG   0x51    /* JPGdocument */
#define T_JPEG  0x52    /* JPEGdocument */
#define T_GIF   0x53    /* GIFdocument */
#define T_PNG   0x54    /* GIFdocument */
#define T_AVI   0x60    /* AVIdocument */

uint8_t exfuns_init(void);                              /* Apply for memory */
uint8_t exfuns_file_type(char *fname);                  /* identifydocumenttype */
uint8_t exfuns_get_free(uint8_t *pdrv, uint32_t *total, uint32_t *free);    /* Get the total disk capacity and remaining capacity */
uint32_t exfuns_get_folder_size(uint8_t *fdname);       /* getdocumentClip size */
uint8_t *exfuns_get_src_dname(uint8_t *dpfn);
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);       /* documentcopy */
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode);   /* Folder Copy */

#endif