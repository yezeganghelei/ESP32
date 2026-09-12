/**
 ****************************************************************************************************
 * @file        exfuns.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       FATFS Extended code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "exfuns.h"

#define FILE_MAX_TYPE_NUM       7       /* At most FILE_MAX_TYPE_NUM major categories */
#define FILE_MAX_SUBT_NUM       7       /* Up to FILE_MAX_SUBT_NUM subcategories */

/* File type definition */
static const char *FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM] = {
    {"BIN"," "," "," "," "," "," "},                        /* BIN file */
    {"LRC"," "," "," "," "," "," "},                        /* LRC file */
    {"NES", "SMS"," "," "," "," "," "},                     /* NES/SMS files */
    {"TXT", "C", "H"," "," "," "," "},                      /* Text file */
    {"WAV", "MP3", "OGG", "FLAC", "AAC", "WMA", "MID"},     /* Music files */
    {"BMP", "JPG", "JPEG", "GIF","PNG"," "," "},            /* Image files */
    {"AVI"," "," "," "," "," "," "},                        /* Video files */
};

/******************************************************************************************/
/* Public file area, allocated with malloc */

/* Logical disk workspace (before calling any FATFS function, memory must first be allocated for fs) */
FATFS *fs[FF_VOLUMES];  

/******************************************************************************************/

/**
 * @brief       Allocate memory for exfuns
 * @param       none
 * @retval      0, success; 1, fail.
 */
uint8_t exfuns_init(void)
{
    uint8_t i;
    uint8_t res = 0;

    for (i = 0; i < FF_VOLUMES; i++)
    {
        fs[i] = (FATFS *)malloc(sizeof(FATFS));   /* Allocate memory for the disk workspace */

        if (!fs[i])break;
    }
    
    if (i == FF_VOLUMES && res == 0)
    {
        return 0;   /* If any allocation fails, the whole operation fails. */
    }
    else 
    {
        return 1;
    }
}

/**
 * @brief       Convert a lowercase letter to uppercase; numbers are unchanged.
 * @param       c : Letters to convert
 * @retval      Converted letter (uppercase)
 */
uint8_t exfuns_char_upper(uint8_t c)
{
    if (c < 'A')return c;   /* Number, unchanged. */

    if (c >= 'a')
    {
        return c - 0x20;    /* Convert to uppercase. */
    }
    else
    {
        return c;           /* Already uppercase, unchanged */
    }
}

/**
 * @brief       Type of report file
 * @param       fname : file name
 * @retval      File Type
 *   @arg       0XFF , no matching file type.
 *   @arg       other , The high nibble is the major category, the low nibble is the subcategory.
 */
uint8_t exfuns_file_type(char *fname)
{
    uint8_t tbuf[5];
    char *str_tbuf;
    char *attr = 0;   /* File suffix */
    uint8_t i = 0, j;

    str_tbuf = malloc(5);

    while (i < 250)
    {
        i++;

        if (*fname == '\0')break;   /* Reached the end. */

        fname++;
    }

    if (i == 250)return 0XFF;   /* String too long. */

    for (i = 0; i < 5; i++)     /* Find the suffix */
    {
        fname--;

        if (*fname == '.')
        {
            fname++;
            attr = fname;
            break;
        }
    }

    if (attr == 0)return 0XFF;

    strcpy((char *)tbuf, (const char *)attr);       /* copy */

    for (i = 0; i < 4; i++)tbuf[i] = exfuns_char_upper(tbuf[i]);    /* Convert all to uppercase */

    for (i = 0; i < FILE_MAX_TYPE_NUM; i++)         /* Major categories comparison */
    {
        for (j = 0; j < FILE_MAX_SUBT_NUM; j++)     /* Subclass comparison */
        {
            if (*FILE_TYPE_TBL[i][j] == 0)
            {
                free(str_tbuf);
                break;    /* No more entries in this group. */
            }

            sprintf(str_tbuf,"%s",FILE_TYPE_TBL[i][j]);

            if (strcmp((const char *)str_tbuf, (const char *)tbuf) == 0) /* Match found */
            {
                return (i << 4) | j;
            }
        }
    }

    return 0XFF;    /* No match found */
}

/**
 * @brief       Get the remaining disk capacity
 * @param       pdrv : disk number("0:"~"9:")
 * @param       total: Total capacity (KB)
 * @param       free : Remaining capacity (KB)
 * @retval      0, normal; other, Error code
 */
uint8_t exfuns_get_free(uint8_t *pdrv, uint32_t *total, uint32_t *free)
{
    FATFS *fs1;
    uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    
    /* Get disk information and number of free clusters */
    res = (uint32_t)f_getfree((const TCHAR *)pdrv, (DWORD *)&fre_clust, &fs1);

    if (res == 0)
    {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize;    /* Total number of sectors */
        fre_sect = fre_clust * fs1->csize;              /* Get the number of free sectors */
#if FF_MAX_SS!=512  /* The sector size is not 512 bytes, then it is converted to 512 bytes. */
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif
        *total = tot_sect >> 1;     /* Unit: KB */
        *free = fre_sect >> 1;      /* Unit: KB */
    }

    return res;
}

/**
 * @brief       File Copy
 *   @note      Copy the file psrc to pdst.
 *              Note: file size must not exceed 4GB.

 * @param       fcpymsg : Function pointer, used to display information during copying
 *                  pname: file/folder name
 *                  pct: percentage
 *                  mode:
 *                      bit0 : update file name
 *                      bit1 : update percentage pct
 *                      bit2 : update folder
 *                      other : reserved
 *                  Return value: 0, normal; 1, force quit;

 * @param       psrc    : source file
 * @param       pdst    : Target file
 * @param       totsize : Total size(0 means single-file copy only)
 * @param       cpdsize : Copied size.
 * @param       fwmode  : File writing mode
 *   @arg       0: Do not overwrite the original file
 *   @arg       1: Overwrite the original file

 * @retval      Execution results
 *   @arg       0   , normal
 *   @arg       0XFF, force quit
 *   @arg       other, Error code
 */
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, 
                                      uint32_t totsize, uint32_t cpdsize, uint8_t fwmode)
{
    uint8_t res;
    uint16_t br = 0;
    uint16_t bw = 0;
    FIL *fsrc = 0;
    FIL *fdst = 0;
    uint8_t *fbuf = 0;
    uint8_t curpct = 0;
    unsigned long long lcpdsize = cpdsize;
    
    fsrc = (FIL *)malloc(sizeof(FIL));    /* Apply for memory */
    fdst = (FIL *)malloc(sizeof(FIL));
    fbuf = (uint8_t *)malloc(8192);

    if (fsrc == NULL || fdst == NULL || fbuf == NULL)
    {
        res = 100;  /* Reserve the previous error code for FatFs */
    }
    else
    {
        if (fwmode == 0)
        {
            fwmode = FA_CREATE_NEW;     /* Do not overwrite */
        }
        else 
        {
            fwmode = FA_CREATE_ALWAYS;  /* Overwrite existing files */
        }
        
        res = f_open(fsrc, (const TCHAR *)psrc, FA_READ | FA_OPEN_EXISTING);        /* Open a read-only file */

        if (res == 0)res = f_open(fdst, (const TCHAR *)pdst, FA_WRITE | fwmode);    /* Open the destination file after the source opens successfully */

        if (res == 0)           /* Both files opened successfully */
        {
            if (totsize == 0)   /* Single-file copy only */
            {
                totsize = fsrc->obj.objsize;
                lcpdsize = 0;
                curpct = 0;
            }
            else
            {
                curpct = (lcpdsize * 100) / totsize;            /* Update the percentage */
            }
            
            fcpymsg(psrc, curpct, 0X02);                        /* Update percentage */

            while (res == 0)    /* Start copying */
            {
                res = f_read(fsrc, fbuf, 8192, (UINT *)&br);    /* Source readout512byte */

                if (res || br == 0)break;

                res = f_write(fdst, fbuf, (UINT)br, (UINT *)&bw);/* Write to the destination file */
                lcpdsize += bw;

                if (curpct != (lcpdsize * 100) / totsize)       /* Check whether the percentage needs updating */
                {
                    curpct = (lcpdsize * 100) / totsize;

                    if (fcpymsg(psrc, curpct, 0X02))            /* Update percentage */
                    {
                        res = 0XFF;                             /* Force quit */
                        break;
                    }
                }

                if (res || bw < br)break;
            }

            f_close(fsrc);
            f_close(fdst);
        }
    }

    free(fsrc); /* Free memory */
    free(fdst);
    free(fbuf);
    return res;
}

/**
 * @brief       Get the folder name from a path
 *   @note      Strip the path and keep only the folder name.
 * @param       pname : Detailed path 
 * @retval      0   , The path is a volume number.
 *              other, Pointer to the folder name
 */
uint8_t *exfuns_get_src_dname(uint8_t *pname)
{
    uint16_t temp = 0;

    while (*pname != 0)
    {
        pname++;
        temp++;
    }

    if (temp < 4)return 0;

    while ((*pname != 0x5c) && (*pname != 0x2f))pname--;    /* Find the last "\" or "/" */

    return ++pname;
}

/**
 * @brief       Get the folder size
 *   @note      Note: folder size must not exceed 4GB.
 * @param       pname : Detailed path 
 * @retval      0   , Folder size is 0, or a read error occurred.
 *              other, Folder size
 */
uint32_t exfuns_get_folder_size(uint8_t *fdname)
{
#define MAX_PATHNAME_DEPTH  512 + 1     /* Maximum destination path + file name depth */
    uint8_t res = 0;
    FF_DIR *fddir = 0;         /* Directory */
    FILINFO *finfo = 0;     /* File information */
    uint8_t *pathname = 0;  /* Destination folder path + file name */
    uint16_t pathlen = 0;   /* target path length */
    uint32_t fdsize = 0;

    fddir = (FF_DIR *)malloc(sizeof(FF_DIR));   /* Apply for memory */
    finfo = (FILINFO *)malloc(sizeof(FILINFO));

    if (fddir == NULL || finfo == NULL)res = 100;

    if (res == 0)
    {
        pathname = malloc(MAX_PATHNAME_DEPTH);

        if (pathname == NULL)res = 101;

        if (res == 0)
        {
            pathname[0] = 0;
            strcat((char *)pathname, (const char *)fdname);     /* Copy path */
            res = f_opendir(fddir, (const TCHAR *)fdname);      /* Open the source directory */

            if (res == 0)   /* Open directory successfully */
            {
                while (res == 0)   /* Copy the folder contents */
                {
                    res = f_readdir(fddir, finfo);                  /* Read a file in the directory */

                    if (res != FR_OK || finfo->fname[0] == 0)break; /* An error/It's the end,quit */

                    if (finfo->fname[0] == '.')continue;            /* Ignore the parent directory entry */

                    if (finfo->fattrib & 0X10)                      /* Is a subdirectory (file attributes: 0X20 archive, 0X10 subdirectory) */
                    {
                        pathlen = strlen((const char *)pathname);   /* Get the length of the current path */
                        strcat((char *)pathname, (const char *)"/");/* Append a slash */
                        strcat((char *)pathname, (const char *)finfo->fname);   /* Source path plus subdirectory name */
                        //printf("\r\nsub folder:%s\r\n",pathname);             /* Print subdirectory name */
                        fdsize += exfuns_get_folder_size(pathname);             /* Get the subdirectory size recursively */
                        pathname[pathlen] = 0;                                  /* Append the terminator */
                    }
                    else
                    {
                        fdsize += finfo->fsize;                                 /* Not a directory, add the file size directly */
                    }
                }
            }

            free(pathname);
        }
    }

    free(fddir);
    free(finfo);

    if (res)
    {
        return 0;
    }
    else 
    {
        return fdsize;
    }
}

/**
 * @brief       Folder Copy
 *   @note      Copy the folder psrc to pdst.
 *              Note: file size must not exceed 4GB.

 * @param       fcpymsg : Function pointer, used to display information during copying
 *                  pname: file/folder name
 *                  pct: percentage
 *                  mode:
 *                      bit0 : update file name
 *                      bit1 : update percentage pct
 *                      bit2 : update folder
 *                      other : reserved
 *                  Return value: 0, normal; 1, force quit;

 * @param       psrc    : source folder
 * @param       pdst    : Destination folder
 *   @note      Must be in the form "X:"/"X:XX"/"X:XX/XX"; the parent folder must exist

 * @param       totsize : Total size(0 means single-file copy only)
 * @param       cpdsize : Copied size.
 * @param       fwmode  : File writing mode
 *   @arg       0: Do not overwrite the original file
 *   @arg       1: Overwrite the original file

 * @retval      Execution results
 *   @arg       0   , normal
 *   @arg       0XFF, force quit
 *   @arg       other, Error code
 */
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, 
                           uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode)
{
#define MAX_PATHNAME_DEPTH 512 + 1  /* Maximum destination path + file name depth */
    uint8_t res = 0;
    FF_DIR *srcdir = 0;     /* Source directory */
    FF_DIR *dstdir = 0;     /* Destination directory */
    FILINFO *finfo = 0;     /* File information */
    uint8_t *fn = 0;        /* Long file name */

    uint8_t *dstpathname = 0;   /* Destination folder path + file name */
    uint8_t *srcpathname = 0;   /* Source folder path + file name */

    uint16_t dstpathlen = 0;    /* target path length */
    uint16_t srcpathlen = 0;    /* Source path length */

    srcdir = (FF_DIR *)malloc(sizeof(FF_DIR));  /* Apply for memory */
    dstdir = (FF_DIR *)malloc(sizeof(FF_DIR));
    finfo = (FILINFO *)malloc(sizeof(FILINFO));

    if (srcdir == NULL || dstdir == NULL || finfo == NULL)res = 100;

    if (res == 0)
    {
        dstpathname = malloc(MAX_PATHNAME_DEPTH);
        srcpathname = malloc(MAX_PATHNAME_DEPTH);

        if (dstpathname == NULL || srcpathname == NULL)res = 101;

        if (res == 0)
        {
            dstpathname[0] = 0;
            srcpathname[0] = 0;
            strcat((char *)srcpathname, (const char *)psrc);    /* Copy the original source file path */
            strcat((char *)dstpathname, (const char *)pdst);    /* Copy the original destination path */
            res = f_opendir(srcdir, (const TCHAR *)psrc);       /* Open the source directory */

            if (res == 0)   /* Open directory successfully */
            {
                strcat((char *)dstpathname, (const char *)"/"); /* Append a slash */
                fn = exfuns_get_src_dname(psrc);

                if (fn == 0)   /* Copying a volume label */
                {
                    dstpathlen = strlen((const char *)dstpathname);
                    dstpathname[dstpathlen] = psrc[0];          /* Record the label */
                    dstpathname[dstpathlen + 1] = 0;            /* Terminator */
                }
                else strcat((char *)dstpathname, (const char *)fn); /* Append the file name */

                fcpymsg(fn, 0, 0X04);   /* Update folder name */
                res = f_mkdir((const TCHAR *)dstpathname);  /* Do not create if the folder already exists; otherwise create it. */

                if (res == FR_EXIST)res = 0;

                while (res == 0)        /* Copy the folder contents */
                {
                    res = f_readdir(srcdir, finfo);         /* Read a file in the directory */

                    if (res != FR_OK || finfo->fname[0] == 0)break; /* An error/It's the end,quit */

                    if (finfo->fname[0] == '.')continue;    /* Ignore the parent directory entry */

                    fn = (uint8_t *)finfo->fname;           /* Get the file name */
                    dstpathlen = strlen((const char *)dstpathname); /* Get the current destination path length */
                    srcpathlen = strlen((const char *)srcpathname); /* Get the source path length */

                    strcat((char *)srcpathname, (const char *)"/"); /* Append a slash to the source path */

                    if (finfo->fattrib & 0X10)  /* Is a subdirectory (file attributes: 0X20 archive, 0X10 subdirectory) */
                    {
                        strcat((char *)srcpathname, (const char *)fn);  /* Source path plus subdirectory name */
                        res = exfuns_folder_copy(fcpymsg, srcpathname, dstpathname, totsize, cpdsize, fwmode);   /* Copy the folder */
                    }
                    else     /* Non-directory */
                    {
                        strcat((char *)dstpathname, (const char *)"/"); /* Append a slash to the destination path */
                        strcat((char *)dstpathname, (const char *)fn);  /* Append the file name to the destination path */
                        strcat((char *)srcpathname, (const char *)fn);  /* Append the file name to the source path */
                        fcpymsg(fn, 0, 0X01);       /* Update file name */
                        res = exfuns_file_copy(fcpymsg, srcpathname, dstpathname, *totsize, *cpdsize, fwmode);  /* Copy the file */
                        *cpdsize += finfo->fsize;   /* Add the file size */
                    }

                    srcpathname[srcpathlen] = 0;    /* Append the terminator */
                    dstpathname[dstpathlen] = 0;    /* Append the terminator */
                }
            }

            free(dstpathname);
            free(srcpathname);
        }
    }

    free(srcdir);
    free(dstdir);
    free(finfo);
    return res;
}