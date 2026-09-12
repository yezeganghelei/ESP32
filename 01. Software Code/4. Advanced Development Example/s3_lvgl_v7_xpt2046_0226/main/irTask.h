#ifndef _IRTASK_
#define _IRTASK_
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"
#include "irnvs.h"
#include "ir_decode.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"

#define RMT_RX_ACTIVE_LEVEL  0   /*!< If we connect with a IR receiver, the data is active low */
#define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */

#define RMT_TX_CHANNEL    1     /*!< RMT channel for transmitter */
#define RMT_TX_GPIO_NUM   48     /*!< GPIO number for transmitter signal */
#define RMT_RX_CHANNEL    3     /*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM  4     /*!< GPIO number for receiver */
#define RMT_CLK_DIV      100    /*!< RMT counter clock divider; the counter increments every 80M/100=1.25us */
#define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us. (Source clock is APB clock); can time 10 us */

#define HEADER_HIGH_9000US    9000                         /*!< NEC protocol header: positive 9ms */
#define HEADER_LOW_4500US     4500                         /*!< NEC protocol header: negative 4.5ms*/
#define HEADER_HIGH_4300US    4300
#define HEADER_LOW_4300US    4300
#define HEADER_HIGH_7300US    7300
#define HEADER_LOW_5800US    5800
#define HEADER_HIGH_3000US   3000
#define HEADER_LOW_3000US   3000
#define HEADER_HIGH_4500US   4500



#define NEC_CONNECT_HIGH_US     646                         /*!< NEC protocol CONNECT: positive 0.6ms*/
#define NEC_CONNECT_LOW_US     20000                          /*!< NEC protocol CONNECT: negative 2ms*/
#define NEC_BIT_ONE_HIGH_US    646                         /*!< NEC protocol data bit 1: positive 0.56ms */
#define NEC_BIT_ONE_LOW_US    1643   /*!< NEC protocol data bit 1: negative 1.69ms */
#define NEC_BIT_ZERO_HIGH_US   646                         /*!< NEC protocol data bit 0: positive 0.56ms */
#define NEC_BIT_ZERO_LOW_US   516  /*!< NEC protocol data bit 0: negative 0.56ms */
#define NEC_BIT_END            646                         /*!< NEC protocol end: positive 0.56ms */
#define NEC_BIT_MARGIN         200                          /*!< NEC parse margin time */

#define NEC_ITEM_DURATION(d)  ((d & 0x7fff)*10/RMT_TICK_10_US)  /*!< Parse duration time from memory register value */
#define NEC_DATA_ITEM_NUM   70  /*!< NEC code item number: header + 35bit data + connect +32bit +end*/
#define RMT_TX_DATA_NUM  3    /*!< NEC tx test data number */
#define rmt_item32_tIMEOUT_US  21000   /*!< RMT receiver timeout value (us); the connect code lasts about 20600 us, so set a longer time */


enum ac_band
{
    band_gree = 0,
    band_meidi = 1,
    songxia = 2,
    band_dajin = 3,
    band_haier = 4,
    band_haixin = 5,
    band_aux = 6,
    band_max
};
enum ac_pro_code
{
    code_1 = 0,
    code_2 = 1,
    code_3 = 2,
    code_4 = 3,
    code_5 = 4,
   
    code_maxa
};

// Brand + protocol code = IR code library
struct AC_Control
{
    uint8_t code;
    t_remote_ac_status status;  // Air conditioner control structure
};

// Air conditioner structure, used to control the air conditioner
static struct AC_Control ac_handle; 

// Received IR signal
struct RX_signal
{
    uint32_t item_num;  // Number of items
    uint32_t lowlevel;  // Low-level time (us)
    uint32_t highlevel_1;   // High-level-1 time
    uint32_t highlevel_0;   // High-level-0 time
    uint32_t encode;    // Code composed of 0s and 1s
};




TaskHandle_t ir_tx_handle;
uint8_t ac_set_code_lib(uint8_t band, uint8_t pro_code);
int ac_set_temp(int temp);
int ac_open(bool open);
int ac_set_wind_speed(int speed);
int ac_set_swing(bool open);
void ac_set_type(uint8_t ac);
void ac_set_type1(uint8_t ac,uint8_t code);
void ir_study();    // Start learning
int IR_init();  // Initialize
int storage_init(); // Initialize the storage system and file system


void rmt_ir_txTask(void *agr);
void rmt_ir_rxTask(void *agr);
#endif