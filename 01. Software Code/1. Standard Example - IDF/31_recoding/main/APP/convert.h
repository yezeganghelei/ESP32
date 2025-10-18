/**
 ****************************************************************************************************
* @file        convert.h
* @author      
* @version     V1.0
* @date        2023-12-01
* @brief       UTF8andGBKTransfer to each other Code
* @license     Copyright (c) 2020-2032, 
****************************************************************************************************
* @attention

****************************************************************************************************
*/

#ifndef CONVERY_H_
#define CONVERY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Array size*/
#define ARRAY_SIZE(a)       ((sizeof(a))/(sizeof((a)[0])))

int convet_gbk_to_utf8(char **ptr, void *pin_buf, int in_len);
int convet_utf8_to_gbk(char **ptr, void *pin_buf, int in_len);
void convet_test(void);

#endif