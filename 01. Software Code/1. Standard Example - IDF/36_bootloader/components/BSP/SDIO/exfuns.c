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

#define FILE_MAX_TYPE_NUM       7       /* mostFILE_MAX_TYPE_NUMA big category */
#define FILE_MAX_SUBT_NUM       7       /* Up to FILE_MAX_SUBT_NUM subcategories */

/* File type definition */
static const char *FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM] = {
    {"BIN"," "," "," "," "," "," "},                        /* BIN file */
    {"LRC"," "," "," "," "," "," "},                        /* LRC file */
    {"NES", "SMS"," "," "," "," "," "},                     /* NES/SMS files */
    {"TXT", "C", "H"," "," "," "," "},                      /* Text file */
    {"WAV", "MP3", "OGG", "FLAC", "AAC", "WMA", "MID"},     /* musicdocument */
    {"BMP", "JPG", "JPEG", "GIF","PNG"," "," "},            /* picturedocument */
    {"AVI"," "," "," "," "," "," "},                        /* Video files */
};

/******************************************************************************************/
/* Public file area, usemallocWhen */

/* Logical disk workspace(Calling anyFATFSBefore the related functions,Must give firstfsApply for memory) */
FATFS *fs[FF_VOLUMES];  

/******************************************************************************************/

/**
 * @brief       forexfunsApply for memory
 * @param       none
 * @retval      0, success; 1, fail.
 */
uint8_t exfuns_init(void)
{
    uint8_t i;
    uint8_t res = 0;

    for (i = 0; i < FF_VOLUMES; i++)
    {
        fs[i] = (FATFS *)malloc(sizeof(FATFS));   /* fordiskiworkspaceApply for memory */

        if (!fs[i])break;
    }
    
    if (i == FF_VOLUMES && res == 0)
    {
        return 0;   /* If there is one failure in the application, that is, failure. */
    }
    else 
    {
        return 1;
    }
}

/**
 * @brief       WillLowercase letter conversionforcapitalletter,ifyesnumber,Stay unchanged.
 * @param       c : Letters to convert
 * @retval      Converted letters,capital
 */
uint8_t exfuns_char_upper(uint8_t c)
{
    if (c < 'A')return c;   /* number,Stay unchanged. */

    if (c >= 'a')
    {
        return c - 0x20;    /* Change to capitalization. */
    }
    else
    {
        return c;           /* Capitalization, stay the same */
    }
}

/**
 * @brief       Type of report file
 * @param       fname : documentname
 * @retval      File Type
 *   @arg       0XFF , expressnoneMethod identifiedFile Typeserial number.
 *   @arg       other , The four high digits represent the major category, The lower four digits represent the subcategory.
 */
uint8_t exfuns_file_type(char *fname)
{
    uint8_t tbuf[5];
    char *str_tbuf;
    char *attr = 0;   /* Suffix name */
    uint8_t i = 0, j;

    str_tbuf = malloc(5);

    while (i < 250)
    {
        i++;

        if (*fname == '\0')break;   /* OffsetarriveIt's the last. */

        fname++;
    }

    if (i == 250)return 0XFF;   /* Wrong string. */

    for (i = 0; i < 5; i++)     /* have toarriveSuffix name */
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

    for (i = 0; i < 4; i++)tbuf[i] = exfuns_char_upper(tbuf[i]);    /* All changeforcapital */

    for (i = 0; i < FILE_MAX_TYPE_NUM; i++)         /* Major categories comparison */
    {
        for (j = 0; j < FILE_MAX_SUBT_NUM; j++)     /* Subclass comparison */
        {
            if (*FILE_TYPE_TBL[i][j] == 0)
            {
                free(str_tbuf);
                break;    /* This group has no comparable members. */
            }

            sprintf(str_tbuf,"%s",FILE_TYPE_TBL[i][j]);

            if (strcmp((const char *)str_tbuf, (const char *)tbuf) == 0) /* Found */
            {
                return (i << 4) | j;
            }
        }
    }

    return 0XFF;    /* Didn't look for itarrive */
}

/**
 * @brief       get diskRemaining capacity
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
        tot_sect = (fs1->n_fatent - 2) * fs1->csize;    /* have toarriveTotal sectors */
        fre_sect = fre_clust * fs1->csize;              /* Get the number of free sectors */
#if FF_MAX_SS!=512  /* The sector size is not 512 bytes, then it is converted to 512 bytes. */
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif
        *total = tot_sect >> 1;     /* unitforKB */
        *free = fre_sect >> 1;      /* unitforKB */
    }

    return res;
}

/**
 * @brief       File Copy
 *   @note      Willpsrcdocument,copyarrivepdst.
 *              Notice: Do not exceed the file size4GB.

 * @param       fcpymsg : Function pointer, Used to realize the information display during copying
 *                  pname:document/Foldersname
 *                  pct:percentage
 *                  mode:
 *                      bit0 : 更newdocumentname
 *                      bit1 : 更newpercentagepct
 *                      bit2 : 更newFolders
 *                      other : reserve
 *                  Return value: 0, normal; 1, Mandatoryquit;

 * @param       psrc    : sourcedocument
 * @param       pdst    : Target file
 * @param       totsize : Total size(whentotsizefor0When,Indicates onlyforsingledocumentcopy)
 * @param       cpdsize : Copyed size.
 * @param       fwmode  : File writing mode
 *   @arg       0: Do not overwrite the original file
 *   @arg       1: Overwrite the original file

 * @retval      Execution results
 *   @arg       0   , normal
 *   @arg       0XFF, Mandatoryquit
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
        res = 100;  /* The previous value is left to fatfs */
    }
    else
    {
        if (fwmode == 0)
        {
            fwmode = FA_CREATE_NEW;     /* Not covered */
        }
        else 
        {
            fwmode = FA_CREATE_ALWAYS;  /* Overwrite existing files */
        }
        
        res = f_open(fsrc, (const TCHAR *)psrc, FA_READ | FA_OPEN_EXISTING);        /* Open a read-only file */

        if (res == 0)res = f_open(fdst, (const TCHAR *)pdst, FA_WRITE | fwmode);    /* The first one opens successfully,The second one started to be opened */

        if (res == 0)           /* Both have been successfully opened */
        {
            if (totsize == 0)   /* 仅仅yessingleFile Copy */
            {
                totsize = fsrc->obj.objsize;
                lcpdsize = 0;
                curpct = 0;
            }
            else
            {
                curpct = (lcpdsize * 100) / totsize;            /* have toarrivenewpercentage */
            }
            
            fcpymsg(psrc, curpct, 0X02);                        /* 更newpercentage */

            while (res == 0)    /* Start copying */
            {
                res = f_read(fsrc, fbuf, 8192, (UINT *)&br);    /* Source readout512byte */

                if (res || br == 0)break;

                res = f_write(fdst, fbuf, (UINT)br, (UINT *)&bw);/* Write to the destination file */
                lcpdsize += bw;

                if (curpct != (lcpdsize * 100) / totsize)       /* Does it need to be updated?percentage */
                {
                    curpct = (lcpdsize * 100) / totsize;

                    if (fcpymsg(psrc, curpct, 0X02))            /* 更newpercentage */
                    {
                        res = 0XFF;                             /* Mandatoryquit */
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
 * @brief       have toarriveunder the pathFolders
 *   @note      Remove all the paths, Leave only the folder name.
 * @param       pname : Detailed path 
 * @retval      0   , The path is a volume number.
 *              other, Foldersname字首地址
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

    while ((*pname != 0x5c) && (*pname != 0x2f))pname--;    /* recallarriveThe last one"\"or"/"Where */

    return ++pname;
}

/**
 * @brief       have toarriveFolderssize
 *   @note      Notice: Folderssize不要超过4GB.
 * @param       pname : Detailed path 
 * @retval      0   , Folderssizefor0, orAn error occurred during reading.
 *              other, Folderssize
 */
uint32_t exfuns_get_folder_size(uint8_t *fdname)
{
#define MAX_PATHNAME_DEPTH  512 + 1     /* maximumTarget filepath+File name depth */
    uint8_t res = 0;
    FF_DIR *fddir = 0;         /* Table of contents */
    FILINFO *finfo = 0;     /* File information */
    uint8_t *pathname = 0;  /* Destination folderpath+documentname */
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
            res = f_opendir(fddir, (const TCHAR *)fdname);      /* OpenSource Directory */

            if (res == 0)   /* Open directory successfully */
            {
                while (res == 0)   /* Start copyingFoldersSomething inside */
                {
                    res = f_readdir(fddir, finfo);                  /* Read a file in the directory */

                    if (res != FR_OK || finfo->fname[0] == 0)break; /* An error/It's the end,quit */

                    if (finfo->fname[0] == '.')continue;            /* Ignore the previous directory */

                    if (finfo->fattrib & 0X10)                      /* yesSubdirectory(File properties,0X20,Archive files;0X10,Subdirectory;) */
                    {
                        pathlen = strlen((const char *)pathname);   /* Get the length of the current path */
                        strcat((char *)pathname, (const char *)"/");/* Add slashes */
                        strcat((char *)pathname, (const char *)finfo->fname);   /* Source path plus subdirectory name */
                        //printf("\r\nsub folder:%s\r\n",pathname);             /* Print subdirectory name */
                        fdsize += exfuns_get_folder_size(pathname);             /* have toarriveSubdirectorysize,Recursive call */
                        pathname[pathlen] = 0;                                  /* Add End symbol */
                    }
                    else
                    {
                        fdsize += finfo->fsize;                                 /* Non-directory, directly add the file size */
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
 *   @note      WillpsrcFolders, copyarrivepdstFolders.
 *              Notice: Do not exceed the file size4GB.

 * @param       fcpymsg : Function pointer, Used to realize the information display during copying
 *                  pname:document/Foldersname
 *                  pct:percentage
 *                  mode:
 *                      bit0 : 更newdocumentname
 *                      bit1 : 更newpercentagepct
 *                      bit2 : 更newFolders
 *                      other : reserve
 *                  Return value: 0, normal; 1, Mandatoryquit;

 * @param       psrc    : sourceFolders
 * @param       pdst    : Destination folder
 *   @note      must be in the form of"X:"/"X:XX"/"X:XX/XX"Something like that. And confirm the previous levelFoldersexist

 * @param       totsize : Total size(whentotsizefor0When,Indicates onlyforsingledocumentcopy)
 * @param       cpdsize : Copyed size.
 * @param       fwmode  : File writing mode
 *   @arg       0: Do not overwrite the original file
 *   @arg       1: Overwrite the original file

 * @retval      Execution results
 *   @arg       0   , normal
 *   @arg       0XFF, Mandatoryquit
 *   @arg       other, Error code
 */
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, 
                           uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode)
{
#define MAX_PATHNAME_DEPTH 512 + 1  /* maximumTarget filepath+File name depth */
    uint8_t res = 0;
    FF_DIR *srcdir = 0;     /* Source Directory */
    FF_DIR *dstdir = 0;     /* Source Directory */
    FILINFO *finfo = 0;     /* File information */
    uint8_t *fn = 0;        /* longdocumentname */

    uint8_t *dstpathname = 0;   /* Destination folderpath+documentname */
    uint8_t *srcpathname = 0;   /* sourceFolderspath+documentname */

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
            strcat((char *)dstpathname, (const char *)pdst);    /* Copy the originalTarget filepath */
            res = f_opendir(srcdir, (const TCHAR *)psrc);       /* OpenSource Directory */

            if (res == 0)   /* Open directory successfully */
            {
                strcat((char *)dstpathname, (const char *)"/"); /* Add slashes */
                fn = exfuns_get_src_dname(psrc);

                if (fn == 0)   /* Copy of the tag */
                {
                    dstpathlen = strlen((const char *)dstpathname);
                    dstpathname[dstpathlen] = psrc[0];          /* record label */
                    dstpathname[dstpathlen + 1] = 0;            /* End symbol */
                }
                else strcat((char *)dstpathname, (const char *)fn); /* Add file name */

                fcpymsg(fn, 0, 0X04);   /* 更newFoldersname */
                res = f_mkdir((const TCHAR *)dstpathname);  /* ifFoldersAlready exists,Do not create.Create a new folder if it does not exist. */

                if (res == FR_EXIST)res = 0;

                while (res == 0)        /* Start copyingFoldersSomething inside */
                {
                    res = f_readdir(srcdir, finfo);         /* Read a file in the directory */

                    if (res != FR_OK || finfo->fname[0] == 0)break; /* An error/It's the end,quit */

                    if (finfo->fname[0] == '.')continue;    /* Ignore the previous directory */

                    fn = (uint8_t *)finfo->fname;           /* get filename */
                    dstpathlen = strlen((const char *)dstpathname); /* have toarrivewhen前Target path的long度 */
                    srcpathlen = strlen((const char *)srcpathname); /* have toarriveSource path length */

                    strcat((char *)srcpathname, (const char *)"/"); /* source pathAdd slashes */

                    if (finfo->fattrib & 0X10)  /* yesSubdirectory(File properties,0X20,Archive files;0X10,Subdirectory;) */
                    {
                        strcat((char *)srcpathname, (const char *)fn);  /* Source path plus subdirectory name */
                        res = exfuns_folder_copy(fcpymsg, srcpathname, dstpathname, totsize, cpdsize, fwmode);   /* copyFolders */
                    }
                    else     /* Non-directory */
                    {
                        strcat((char *)dstpathname, (const char *)"/"); /* Target pathAdd slashes */
                        strcat((char *)dstpathname, (const char *)fn);  /* Target pathAdd file name */
                        strcat((char *)srcpathname, (const char *)fn);  /* source pathAdd file name */
                        fcpymsg(fn, 0, 0X01);       /* 更newdocumentname */
                        res = exfuns_file_copy(fcpymsg, srcpathname, dstpathname, *totsize, *cpdsize, fwmode);  /* Copy the file */
                        *cpdsize += finfo->fsize;   /* Increase a file size */
                    }

                    srcpathname[srcpathlen] = 0;    /* Add End symbol */
                    dstpathname[dstpathlen] = 0;    /* Add End symbol */
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