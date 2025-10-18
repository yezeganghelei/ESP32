/**
 ****************************************************************************************************
 * @file        demo_show.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       SPILCD show code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "demo_show.h" 
#include "spilcd.h"
#include <math.h>

#define PI (float)(3.1415926)

static float cube[8][3] = {
    {-16, -16, -16},
    {-16, +16, -16},
    {+16, +16, -16},
    {+16, -16, -16},
    {-16, -16, +16},
    {-16, +16, +16},
    {+16, +16, +16},
    {+16, -16, +16}
};

static uint8_t line_id[24] = {
    1, 2, 2, 3,
    3, 4, 4, 1,
    5, 6, 6, 7,
    7, 8, 8, 5,
    8, 4, 7, 3,
    6, 2, 5, 1
};

/**
 * @brief       calculatematrixmultiplication
 * @param       a      : matrixa
 *              b[3][3]: matrixb
 * @retval      Calculation results
 */
static float *demo_matconv(float *a, float b[3][3])
{
    float res[3];
    uint8_t res_index;
    uint8_t a_index;
    
    for (res_index = 0; res_index < 3; res_index++)
    {
        res[res_index] = b[res_index][0] * a[0] + b[res_index][1] * a[1] + b[res_index][2] * a[2];
    }
    
    for (a_index = 0; a_index < 3; a_index++)
    {
        a[a_index] = res[a_index];
    }
    
    return a;
}

/**
 * @brief       Rotate vector
 * @param       point: vector to be rotated
 *              x    : XShaft rotation amount
 *              y    : YShaft rotation amount
 *              z    : ZShaft rotation amount
 * @retval      Calculation results
 */
static void demo_rotate(float *point, float x, float y, float z)
{
    float rx[3][3];
    float ry[3][3];
    float rz[3][3];
    
    x /= PI;
    y /= PI;
    z /= PI;
    
    rx[0][0] = cos(x);
    rx[0][1] = 0;
    rx[0][2] = sin(x);
    rx[1][0] = 0;
    rx[1][1] = 1;
    rx[1][2] = 0;
    rx[2][0] = -sin(x);
    rx[2][1] = 0;
    rx[2][2] = cos(x);
    
    ry[0][0] = 1;
    ry[0][1] = 0;
    ry[0][2] = 0;
    ry[1][0] = 0;
    ry[1][1] = cos(y);
    ry[1][2] = -sin(y);
    ry[2][0] = 0;
    ry[2][1] = sin(y);
    ry[2][2] = cos(y);
    
    rz[0][0] = cos(z);
    rz[0][1] = -sin(z);
    rz[0][2] = 0;
    rz[1][0] = sin(z);
    rz[1][1] = cos(z);
    rz[1][2] = 0;
    rz[2][0] = 0;
    rz[2][1] = 0;
    rz[2][2] = 1;
    
    demo_matconv(demo_matconv(demo_matconv(point, rz), ry), rx);
}

/**
 * @brief       Demo cube3DRotate
 * @param       none
 * @retval      none
 */
void demo_show_cube(void)
{
    uint8_t point_index;
    uint8_t line_index;
    
    for (point_index = 0; point_index < 8; point_index++)
    {
        demo_rotate(cube[point_index], 0.5f, 0.3f, 0.2f);
    }
    
    for (line_index = 0; line_index < 24; line_index += 2)
    {
        /* LCD line segment */
        lcd_draw_line(120 + cube[line_id[line_index] - 1][0],
                      210 + cube[line_id[line_index] - 1][1],
                      120 + cube[line_id[line_index + 1] - 1][0],
                      210 + cube[line_id[line_index + 1] - 1][1],
                      BLUE);
    }
    
    delay(100);
    
    lcd_fill(92, 182, 148, 238, WHITE);   /* Fill cube active area */
}