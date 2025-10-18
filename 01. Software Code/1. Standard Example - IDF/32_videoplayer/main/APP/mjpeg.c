/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file mjpeg.c
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief MJPEG video processing code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "mjpeg.h"

struct jpeg_decompress_struct *cinfo;
struct my_error_mgr *jerr;
int Windows_Width = 0;
int Windows_Height = 0;
uint16_t imgoffx, imgoffy;                  /* Images inx,yDirection offset量 */
typedef struct my_error_mgr* my_error_ptr;

/**
 * @brief Error exit
 * @param cinfo: JPEG encoding and decoding control structure
 * @retval None
 */
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

/**
 * @brief       send message
 * @param       cinfo       : JPEGEncoding and decoding control structure
 * @param       msg_level   : Message level
 * @retval      none
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

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

/**
 * @brief       Decode a pairJPEGpicture
 * @param       buf: jpegdata流数组
 * @param       bsize: Array size
 * @retval      0,success; 1,fail
 */
uint8_t mjpegdec_decode(uint8_t* buf, uint32_t bsize)
{

    JSAMPARRAY buffer;
    if (bsize == 0) return 1;
    int row_stride = 0;
    int j = 0;                      /* Record the number of lines currently decoded */
    int lineR = 0;                  /* The starting position of R components in each row */
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

    jpeg_mem_src(cinfo, buf, bsize);    /* test normal */
    jpeg_read_header(cinfo, TRUE);

    jpeg_start_decompress(cinfo); 

    row_stride = cinfo->output_width * cinfo->output_components;

    /* calculatebufferSize and apply for the corresponding space */
    buffer = (*cinfo->mem->alloc_sarray)
        ((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);
    
    while (cinfo->output_scanline < cinfo->output_height)
    {
        int i = 0;

        jpeg_read_scanlines(cinfo, buffer, 1);
        unsigned short tmp_color565;

        /* Assign values ​​to the above image data */
        for (int k = 0; k < Windows_Width * 2; k += 2)
        {
            tmp_color565 = rgb565(buffer[0][i],buffer[0][i + 1],buffer[0][i + 2]);
            lcd_buf[lineR + k] = (tmp_color565 & 0xFF00) >> 8;
            lcd_buf[lineR + k + 1] =  tmp_color565 & 0x00FF;

            i += 3;
        }
        
        j++;
        lineR = j * Windows_Width * 2;
    }

    lcd_set_window(imgoffx, imgoffy - 30, imgoffx + cinfo->output_width - 1, imgoffy - 30 + cinfo->output_height - 1);
    taskENTER_CRITICAL(&my_spinlock);
    /* For example：96*96*2/1536 = 12;point12SendRGBdata */
    for(int x = 0; x < (cinfo->output_width * cinfo->output_height * 2 / LCD_BUF_SIZE); x++)
    {
        /* &lcd_buf[j * LCD_BUF_SIZE] 偏移地址发送data */
        lcd_write_data(&lcd_buf[x * LCD_BUF_SIZE] , LCD_BUF_SIZE);
    }
    
    taskEXIT_CRITICAL(&my_spinlock);
    lcd_set_window(0, 0, lcd_self.width, lcd_self.height);  /* recovery window */
    jpeg_finish_decompress(cinfo);
    jpeg_destroy_decompress(cinfo);
    return 0;
}

/**
 * @brief       mjpeg Decoding initialization
 * @param       offx,offy:x,yDirection offset
 * @retval      0,success; 1,fail
 */
char mjpegdec_init(uint16_t offx, uint16_t offy)
{
    cinfo = (struct jpeg_decompress_struct *)malloc(sizeof(struct jpeg_decompress_struct));
    jerr = (struct my_error_mgr *)malloc(sizeof(struct my_error_mgr));

    if (cinfo == NULL || jerr == NULL)
    {
        printf("[E][mjpeg.cpp] mjpegdec_init(): malloc failed to apply for memory\r\n");
        mjpegdec_free();
        return -1;
    }

    /* Save the offset of the image in the x and y directions */
    imgoffx = offx;
    imgoffy = offy;

    return 0;
}

/**
 * @brief       mjpegFinish,Free memory
 * @param       none
 * @retval      none
 */
void mjpegdec_free(void)
{
    free(cinfo);
    free(jerr);
}