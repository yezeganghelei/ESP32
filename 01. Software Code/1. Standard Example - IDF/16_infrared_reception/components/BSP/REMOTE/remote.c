/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file remote.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief RMT infrared decoding driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "remote.h"

const static char *TAG = "REMOTE_RECEIVE TEST";

/* Save the address and command bytes of NEC decoded */
uint16_t s_nec_code_address;
uint16_t s_nec_code_command;

QueueHandle_t receive_queue;
rmt_channel_handle_t rx_channel = NULL;
rmt_symbol_word_t raw_symbols[64];                      /* It should be sufficient for standard NEC frameworks */
rmt_receive_config_t receive_config;
rmt_rx_done_event_data_t rx_data;

/**
 * @brief       initializationRMT
 * @param       none
 * @retval      none
 */
void remote_init(void)
{
    /* Configure the receiving channel */
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                 /* set upRMTClock source */
        .resolution_hz = RMT_RESOLUTION_HZ,             /* set upprotocolfrequency */
        .mem_block_symbols = 64,                        /* aisleCan be stored at onceRMTNumber of symbols */
        .gpio_num = RMT_IN_GPIO_PIN,                    /* set upaisle pins */
    };
    /* Create receive aisle */
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));      /* Create receive aisle */

    /* Create a message queue for receiving infrared encoding */
    receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));      /* Configure message queue */
    assert(receive_queue);

    /* Configure infrared receiving callback function */
    rmt_rx_event_callbacks_t cbs = {
    .on_recv_done = RMT_Rx_Done_Callback,                                   /* RMT signal reception callback function */
    };
    /* Register infrared receiving callback function */
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));  /* ConfigurationRMTReceive channel callback function */

    /* The following timing requirements are based onNECprotocol */
    rmt_receive_config_t receive_config = {
    .signal_range_min_ns = 1250,                /* NECThe minimum duration of the signal is560us，1250ns＜560us，Valid signals are not considered noise */
    .signal_range_max_ns = 12000000,            /* The maximum duration of NEC signal is 9000us, 12000000ns>9000us, reception will not stop early */
    };

    /* Create aNECprotocolofRMTEncoder */
    ir_nec_encoder_config_t nec_encoder_cfg = {
    .resolution = RMT_RESOLUTION_HZ,                                                                        /* Encoder resolution */
    };

    rmt_encoder_handle_t nec_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));                                /* Configure the RMT encoder */

    /* OpenRMTaisle */
    ESP_ERROR_CHECK(rmt_enable(rx_channel));                                                                /* EnableRMTtake overaisle */
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));            /* Ready to receive */

    while (1)
    {
        /* The following time requirements are based onNECprotocol */
        if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            remote_scan(rx_data.received_symbols, rx_data.num_symbols);                                     /* Parses the received symbol and prints the result */

            ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));    /* Restart receiving */
        }
    }
}

/**
 * @brief       Determine whether the data timing length isNECWithin the tolerance range of timing duration Positive and negative REMOTE_NEC_DECODE_MARGIN Within the value of
 * @param       none
 * @retval      none
 */
inline bool rmt_nec_check_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + RMT_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - RMT_NEC_DECODE_MARGIN));
}

/**
 * @brief compares the data timing length to determine whether it is logical 0
 * @param None
 * @retval None
 */
bool rmt_nec_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return rmt_nec_check_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           rmt_nec_check_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief       Comparing the data timing length to determine whether it is logical1
 * @param       none
 * @retval      none
 */
bool rmt_nec_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return rmt_nec_check_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           rmt_nec_check_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief       WillRMTDecode the received resultNECAddress and command
 * @param       none
 * @retval      none
 */
bool rmt_nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols)
{
    rmt_symbol_word_t *cur = rmt_nec_symbols;
    uint16_t address = 0;
    uint16_t command = 0;

    bool valid_leading_code = rmt_nec_check_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                              rmt_nec_check_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);

    if (!valid_leading_code) 
    {
        return false;
    }

    cur++;

    for (int i = 0; i < 16; i++)
    {
        if (rmt_nec_logic1(cur)) 
        {
            address |= 1 << i;
        } 
        else if (rmt_nec_logic0(cur))
        {
            address &= ~(1 << i);
        } 
        else 
        {
            return false;
        }
        cur++;
    }

    for (int i = 0; i < 16; i++)
    {
        if (rmt_nec_logic1(cur))
        {
            command |= 1 << i;
        }
        else if (rmt_nec_logic0(cur))
        {
            command &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }

    /* Save data address and command，Used to judgerepeatbutton */
    s_nec_code_address = address;
    s_nec_code_command = command;

    return true;
}

/**
 * @brief       Check if the data frame isrepeatbutton：Keep holding the same key
 * @param       none
 * @retval      none
 */
bool rmt_nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols)
{
    return rmt_nec_check_range(rmt_nec_symbols->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           rmt_nec_check_range(rmt_nec_symbols->duration1, NEC_REPEAT_CODE_DURATION_1);
}

/**
 * @brief parses infrared protocol according to NEC encoding and prints the instruction result
 * @param None
 * @retval None
 */
void remote_scan(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num)
{
    uint8_t rmt_data = 0;
    uint8_t tbuf[40];
    char *str = 0;

    switch (symbol_num)         /* Decode RMT receive data */
    {
        case 34:                /* normalNECData frames */
        {
            if (rmt_nec_parse_frame(rmt_nec_symbols) )
            {
                rmt_data = (s_nec_code_command >> 8);

                switch (rmt_data)
                {
                    case 0xBA:
                    {
                        str = "POWER";
                        break;
                    }
                    
                    case 0xB9:
                    {
                        str = "UP";
                        break;
                    }
                    
                    case 0xB8:
                    {
                        str = "ALIENTEK";
                        break;
                    }
                    
                    case 0xBB:
                    {
                        str = "BACK";
                        break;
                    }
                    
                    case 0xBF:
                    {
                        str = "PLAY/PAUSE";
                        break;
                    }
                    
                    case 0xBC:
                    {
                        str = "FORWARD";
                        break;
                    }
                    
                    case 0xF8:
                    {
                        str = "vol-";
                        break;
                    }
                    
                    case 0xEA:
                    {
                        str = "DOWN";
                        break;
                    }
                    
                    case 0xF6:
                    {
                        str = "VOL+";
                        break;
                    }
                    
                    case 0xE9:
                    {
                        str = "1";
                        break;
                    }
                    
                    case 0xE6:
                    {
                        str = "2";
                        break;
                    }
                    
                    case 0xF2:
                    {
                        str = "3";
                        break;
                    }
                    
                    case 0xF3:
                    {
                        str = "4";
                        break;
                    }
                    
                    case 0xE7:
                    {
                        str = "5";
                        break;
                    }
                    
                    case 0xA1:
                    {
                        str = "6";
                        break;
                    }
                    
                    case 0xF7:
                    {
                        str = "7";
                        break;
                    }
                    
                    case 0xE3:
                    {
                        str = "8";
                        break;
                    }
                    
                    case 0xA5:
                    {
                        str = "9";
                        break;
                    }
                    
                    case 0xBD:
                    {
                        str = "0";
                        break;
                    }

                    case 0xB5:
                    {
                        str = "DLETE";
                        break;
                    }
                        
                }
                lcd_fill(86, 110, 176, 150, WHITE);
                sprintf((char *)tbuf, "%d", rmt_data);
                printf("KEYVAL = %d, Command=%04X\n", rmt_data, s_nec_code_command);
                lcd_show_string(86, 110, 200, 16, 16, (char *)tbuf, BLUE);
                sprintf((char *)tbuf, "%s", str);
                lcd_show_string(86, 130, 200, 16, 16, (char *)tbuf, BLUE);
            }
            break;
        }
        
        case 2:         /* repeatNEC data frame */
        {
            if (rmt_nec_parse_frame_repeat(rmt_nec_symbols))
            {
                printf("KEYVAL = %d, Command = %04X, repeat\n", rmt_data, s_nec_code_command);
            }
            break;
        }

        default:        /* unknownNEC dataframe */
        {
            printf("Unknown NEC frame\r\n\r\n");
            break;
        }
    }
}

/**
 * @brief       RMTData reception complete callback function
 * @param       none
 * @retval      none
 */
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;

    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup); /* Will receiveRMTData is sent to parsing tasks via message queue */

    return high_task_wakeup == pdTRUE;
}