/**
 ****************************************************************************************************
 * @file        mjpeg.c
 * @author      ALIENTEK Team (ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       MJPEG video processing
 * @license     Copyright (c) 2020-2032, Guangzhou Xingyi Electronic Technology Co., Ltd.
 ****************************************************************************************************
 * @attention
 *
 * Platform: ALIENTEK ESP32-S3 development board
 * Online video: www.yuanzige.com
 * Technical forum: www.openedv.com
 * Company website: www.alientek.com
 * Purchase: openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "mjpeg.h"


uint8_t * video_buf;
struct jpeg_decompress_struct *cinfo;
struct my_error_mgr *jerr;
int Windows_Width = 0;
int Windows_Height = 0;
uint16_t imgoffx, imgoffy;                  /* Image offset in the x and y directions */
typedef struct my_error_mgr* my_error_ptr;

/**
 * @brief       Error exit
 * @param       cinfo   : JPEG codec control structure
 * @retval      None
 */
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * @brief       Emit a message
 * @param       cinfo       : JPEG codec control structure
 * @param       msg_level   : Message level
 * @retval      None
 */
METHODDEF(void) my_emit_message(j_common_ptr cinfo, int msg_level)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    if (msg_level < 0)
    {
        printf("emit msg:%d\r\n", msg_level);
        longjmp(myerr->setjmp_buffer, 1);
    }
}

/**
 * @brief       Allocate video memory
 * @param       None
 * @retval      None
 */
void mjpegdec_malloc(void)
{
    video_buf = heap_caps_malloc(Windows_Width * Windows_Height * 2, MALLOC_CAP_8BIT);
}

/**
 * @brief       Free video memory
 * @param       None
 * @retval      None
 */
void mjpegdec_video_free(void)
{
    heap_caps_free(video_buf);
    video_buf = NULL;
}

/**
 * @brief       Decode one JPEG image
 * @param       buf: JPEG data stream array
 * @param       bsize: Array size
 * @retval      0, success; 1, failure
 */
uint8_t mjpegdec_decode(uint8_t* buf, uint32_t bsize,lcd_write_cb lcd_cb)
{
    JSAMPARRAY buffer;
    if (bsize == 0) return 1;
    int row_stride = 0;
    int j = 0;                      /* Record the current decoded row count */
    int lineR = 0;                  /* Start position of the R component in each row */
    
    cinfo->err = jpeg_std_error(&jerr->pub);
    jerr->pub.error_exit = my_error_exit;
    jerr->pub.emit_message = my_emit_message;
    cinfo->out_color_space = JCS_RGB;

    if (setjmp(jerr->setjmp_buffer)) /* Error handling */
    {
        jpeg_abort_decompress(cinfo);
        jpeg_destroy_decompress(cinfo);
        return 2;
    }

    jpeg_create_decompress(cinfo);

    jpeg_mem_src(cinfo, buf, bsize);    /* Works as expected */
    jpeg_read_header(cinfo, TRUE);

    jpeg_start_decompress(cinfo); 

    row_stride = cinfo->output_width * cinfo->output_components;

    /* Calculate the buffer size and allocate the corresponding space */
    buffer = (*cinfo->mem->alloc_sarray)
        ((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);
    
    while (cinfo->output_scanline < cinfo->output_height)
    {
        int i = 0;

        jpeg_read_scanlines(cinfo, buffer, 1);
        unsigned short tmp_color565;

        /* Assign values for the above image data */
        for (int k = 0; k < Windows_Width * 2; k += 2)
        {
            tmp_color565 = rgb565(buffer[0][i],buffer[0][i + 1],buffer[0][i + 2]);
            video_buf[lineR + k] = tmp_color565 & 0x00FF;
            video_buf[lineR + k + 1] =  (tmp_color565 & 0xFF00) >> 8;

            i += 3;
        }
        
        j++;
        lineR = j * Windows_Width * 2;
    }
    lcd_cb(Windows_Width,Windows_Height,video_buf);
    
    jpeg_finish_decompress(cinfo);
    jpeg_destroy_decompress(cinfo);
    
    return 0;
}

/**
 * @brief       MJPEG decoding initialization
 * @param       offx,offy: Offset in the x and y directions
 * @retval      0, success; 1, failure
 */
char mjpegdec_init(uint16_t offx, uint16_t offy)
{
    Windows_Width = 0;
    Windows_Height = 0;
    cinfo = (struct jpeg_decompress_struct *)malloc(sizeof(struct jpeg_decompress_struct));
    jerr = (struct my_error_mgr *)malloc(sizeof(struct my_error_mgr));

    if (cinfo == NULL || jerr == NULL)
    {
        printf("[E][mjpeg.cpp] mjpegdec_init(): malloc failed to apply for memory\r\n");
        mjpegdec_free();
        return -1;
    }

    /* Save the image offset in the x and y directions */
    imgoffx = offx;
    imgoffy = offy;

    return 0;
}

/**
 * @brief       MJPEG end, free memory
 * @param       None
 * @retval      None
 */
void mjpegdec_free(void)
{
    free(cinfo);
    free(jerr);
    cinfo = NULL;
    jerr = NULL;
}
