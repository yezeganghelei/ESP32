/**
 ****************************************************************************************************
 * @file        camera.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       CAMERA driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "camera.h"
#include "esp_camera.h"
#include "spilcd.h"
#include "xl9555.h"

camera_fb_t *fb = NULL;   /* Define a pointercamera_fb_tPointer to variables(camera_fb_tStructure storage image buffer related information) */

/**
 * @brief Camera (OV5640 / OV2640) Initialization
 * @param None
 * @retval 0: indicates successful initialization / 1: indicates failure 
 */
uint8_t camera_init(void)
{
    camera_config_t camera_config;

    camera_config.ledc_channel = LEDC_CHANNEL_0;  /* Generate XCLK clock channel */
    camera_config.ledc_timer = LEDC_TIMER_0;      /* produceXCLKTimer for clock  */
    camera_config.xclk_freq_hz = 20000000;        /* Set the external clock frequency to 20M */

    camera_config.pin_d7 = OV_D7_PIN;             /* Data line7 */
    camera_config.pin_d6 = OV_D6_PIN;             /* Data line6 */
    camera_config.pin_d5 = OV_D5_PIN;             /* Data line5 */
    camera_config.pin_d4 = OV_D4_PIN;             /* Data line4 */
    camera_config.pin_d3 = OV_D3_PIN;             /* Data line3 */
    camera_config.pin_d2 = OV_D2_PIN;             /* Data line2 */
    camera_config.pin_d1 = OV_D1_PIN;             /* Data line1 */
    camera_config.pin_d0 = OV_D0_PIN;             /* Data line0 */

    camera_config.pin_xclk  = OV_XCLK_PIN;        /* External clock pin */ 
    camera_config.pin_pclk  = OV_PCLK_PIN;        /* Pixel clock foot */
    camera_config.pin_vsync = OV_VSYNC_PIN;       /* Vertical sync pin  */
    camera_config.pin_href  = OV_HREF_PIN;        /* Horizontal sync feet */

    camera_config.pin_sscb_sda = OV_SDA_PIN;      /* SCCBData line */
    camera_config.pin_sscb_scl = OV_SCL_PIN;      /* SCCBClock line */

    camera_config.pin_pwdn  = OV_PWDN_PIN;        /* Power down pin */
    camera_config.pin_reset = OV_RESET_PIN;       /* Reset pin */
    
    camera_config.frame_size   = FRAMESIZE_QVGA;  /* Image size */  
    camera_config.pixel_format = PIXFORMAT_RGB565;/* set upImage format */
    camera_config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;  /* Camera initializationof configuration structure */
    camera_config.fb_location  = CAMERA_FB_IN_PSRAM;      /* Camera image buffer storage location */

    camera_config.jpeg_quality = 12;              /* Set JPEG image quality (0~63, the lower the number, the higher the quality) */          
    camera_config.fb_count     = 1;               /* Number of image buffers */            

    if (spilcd_dir == 0)      /* In the case of vertical screen,Only display240*240Images with resolution */
    {
        camera_config.frame_size   = FRAMESIZE_240X240; 
    }

    if (OV_PWDN_PIN == -1)                        /* CameraPower on */  
    {
        xl9555_io_config(OV_PWDN, IO_SET_OUTPUT);   /* PWDNPin usedXL9555ofIO */
        xl9555_pin_set(OV_PWDN, IO_SET_LOW);        /* Power on */
    } 

    if (OV_RESET_PIN == -1)                       /* hardware reset */
    { 
        xl9555_io_config(OV_RESET, IO_SET_OUTPUT);  /* RESETPin usedXL9555ofIO */
        xl9555_pin_set(OV_RESET, IO_SET_LOW);
        delay(20);
        xl9555_pin_set(OV_RESET, IO_SET_HIGH);
        delay(20);
    } 

    esp_err_t err =  esp_camera_init(&camera_config);   /* Camerainitialization */
    if (err != ESP_OK) 
    {
        Serial.printf("Camerainitializationfail,Error code:0x%x", err);
        return 1;
    }

    sensor_t * s = esp_camera_sensor_get();             /* GetCamerainformation */
    Serial.printf("CameraID:%#x  \r\n", s->id.PID);      /* PrintCameraID */
    
    s->set_brightness(s, 0);      /* Set brightness (-2 ~ 2) */         
    s->set_contrast(s, 0);        /* set upContrast (-2 ~ 2) */
    s->set_saturation(s, 0);      /* Set saturation (-2 ~ 2) */
    s->set_hmirror(s, 0);         /* No horizontal flip */
    s->set_vflip(s, 1);           /* set upFlip vertically */

    if (s->id.PID == OV2640_PID)
    {
        s->set_vflip(s, 0);       /* Flip vertically without setting up */
    }

    return 0;
}

/**
 * @brief       LCDDisplay camera capture data
 * @param       none
 * @retval      0:success / 1:There is a problem getting the screen 
 */
uint8_t camera_capture_show(void)
{
  fb = esp_camera_fb_get();                     /* Capture a frame of image data */
  if (!fb) 
  {
      Serial.printf("noneHow to obtain image data \r\n");
      return 1;
  }

  if (spilcd_dir == 1)                          /* In the case of horizontal screen,show320*240Images with resolution */
  {
      lcd_show_pic(0, 0, 320, 240, fb->buf);    /* Full screen display */
  }
  else                                          /* In the case of vertical screen,show240*240Images with resolution */
  {
      lcd_show_pic(0, 39, 240, 240, fb->buf);   /* Display in the center of the screen */
  }

  esp_camera_fb_return(fb);                     /* ClearCameracache */
  fb = NULL;

  return 0;
}