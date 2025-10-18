/**
 ****************************************************************************************************
 * @file        emission.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       RMT红外decoding驱动代码
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "emission.h"

/* saveNECdecoding的Addresses and commands字节 */
uint16_t s_nec_code_address;
uint16_t s_nec_code_command;

QueueHandle_t receive_queue;
uint8_t tbuf[40];
extern uint8_t rmt_flag;

/**
 * @brief       initializationRMT
 * @param       none
 * @retval      none
 */
void emission_init(void)
{
    uint8_t t = 0;
    uint8_t xl_key = 0;

    /* Configure the receiving channel */
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                                                                                 /* RMT receiving channel clock source */
        .resolution_hz = RMT_RESOLUTION_HZ,                                                                             /* RMT receiving channel clock resolution */
        .mem_block_symbols = 64,                                                                                        /* The channel can be stored at one timeRMTsymbol数量 */
        .gpio_num = RMT_RX_PIN,                                                                                         /* RMT Receive Channel Pin */
    };
    rmt_channel_handle_t rx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));                                                  /* Create aRMTReceive channel */

    /* Configure message queue */
    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));                                    /* Define a message queue，Used to deal withRMTReceive callback function */
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = RMT_Rx_Done_Callback,                                                                           /* Event callback，When aRMTCalled when the channel receives transaction is completed */
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));                                  /* forRMT RXChannel setting callback */

    /* The following time requirements are based onNECprotocol */
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,                                                                                    /* NEC信号的最短持续时间for560us，1250ns＜560us，有效信号不会被视for噪声 */
        .signal_range_max_ns = 12000000,                                                                                /* NEC信号的最长持续时间for9000us，12000000ns>9000us，Reception will not stop early */
    };

    /* Configure the sending channel */
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                                                                                 /* RMTTransmit channel clock source */
        .resolution_hz = RMT_RESOLUTION_HZ,                                                                             /* RMT transmission channel clock resolution */
        .mem_block_symbols = 64,                                                                                        /* The channel can be stored at one timeRMTsymbol数量 */
        .trans_queue_depth = 4,                                                                                         /* Number of transactions allowed to be pending in the background，This example will not queue multiple transactions，Therefore, the queue depth>1That's enough */
        .gpio_num = RMT_TX_PIN,                                                                                         /* RMTTransmit channel pin */
    };
    rmt_channel_handle_t tx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));                                                  /* Create an RMT sending channel */

    /* Configure carrier and duty cycle s */
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = 38000,                                                                                          /* Carrier frequency，0Indicates that carrier is disabled */
        .duty_cycle = 0.33,                                                                                             /* Carrier duty cycle */
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_channel, &carrier_cfg));                                                       /* Apply a modulation function to the transmit channel */

    /* will not be sent in a loopNECframe */
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,                                                                                                /* 0for不循环，-1fornone限循环 */
    };

    /* Configure the encoder */
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = RMT_RESOLUTION_HZ,                                                                                /* Encoder resolution */
    };
    rmt_encoder_handle_t nec_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));                                            /* Configure the encoder */

    /* Enable sending、Receive channel */
    ESP_ERROR_CHECK(rmt_enable(tx_channel));                                                                            /* Enable sending通道 */
    ESP_ERROR_CHECK(rmt_enable(rx_channel));                                                                            /* Enable receive channel */

    /* Save receivedRMTsymbol */
    rmt_symbol_word_t raw_symbols[64];                                                                                  /* 64个symbol对于标准NECThe framework should be sufficient */
    rmt_rx_done_event_data_t rx_data;

    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));                        /* ready to receive */

    while (1)
    {
        if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS)                                      /* Wait for RX to complete signal */
        {
            example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);                                     /* 解析接收symbol并打印结果 */
            ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));                /* Restart receiving */
        }
        else                                                                                                            /* Timeout, transmit predefined IR NEC packets */
        {
            t++;

            if (t == 0)
            {
                t = 1;
            }

            const ir_nec_scan_code_t scan_code = {
                .command = t,
            };

            xl_key = xl9555_key_scan(0);

            if (rmt_flag == 1)
            {
                printf("******OK!******\r\n");
                break;
            }

            if (xl_key == KEY0_PRES)
            {
                rmt_flag = 0;
                printf("******FAIL!******\r\n");
                break;
            }
            
            printf("TX CMD = %d\n", scan_code.command);
            ESP_ERROR_CHECK(rmt_transmit(tx_channel, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));    /* passRMTSend channel transmission data */
        }
    }

    rmt_disable(tx_channel);
    rmt_disable(rx_channel);
    vQueueDelete(receive_queue);
}

/**
 * @brief       Determine whether the data timing length isNECWithin the timing tolerance range Positive or negative RMT_NEC_DECODE_MARGIN within the value of
 * @param       none
 * @retval      none
 */
inline bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + RMT_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - RMT_NEC_DECODE_MARGIN));
}

/**
 * @brief       对比数据时序长度判断是否for逻辑0
 * @param       none
 * @retval      none
 */
bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief       对比数据时序长度判断是否for逻辑1
 * @param       none
 * @retval      none
 */
bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief Decode the RMT reception result to get the NEC address and command
 * @param none
 * @retval None
 */
bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols)
{
    rmt_symbol_word_t *cur = rmt_nec_symbols;
    uint16_t address = 0;
    uint16_t command = 0;

    bool valid_leading_code = nec_check_in_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                              nec_check_in_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);

    if (!valid_leading_code) 
    {
        return false;
    }

    cur++;

    for (int i = 0; i < 16; i++)
    {
        if (nec_parse_logic1(cur)) 
        {
            address |= 1 << i;
        } 
        else if (nec_parse_logic0(cur))
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
        if (nec_parse_logic1(cur))
        {
            command |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            command &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }

    /* Save data address and command，Used to judge repeated keys */
    s_nec_code_address = address;
    s_nec_code_command = command;

    return true;
}

/**
 * @brief       检查数据frame是否forrepeat按键：Keep holding the same key
 * @param       none
 * @retval      none
 */
bool nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_REPEAT_CODE_DURATION_1);
}

/**
 * @brief parses the infrared protocol according to NEC encoding and prints the instruction result
 * @param None
 * @retval None
 */
void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num)
{
    switch (symbol_num) /* decodingRMTreceive data */
    {
        case 34:        /* normalNEC数据frame */
        {
            if (nec_parse_frame(rmt_nec_symbols) )
            {
                rmt_flag = 1;
                printf("RX CMD = %d\n", s_nec_code_command);
            }
            break;
        }
        
        case 2:         /* Duplicate NEC data frame */
        {
            if (nec_parse_frame_repeat(rmt_nec_symbols))
            {
            }
            break;
        }

        default:        /* Unknown NEC data frame */
        {
            break;
        }
    }
}

/**
 * @brief       RMTData reception completion callback function
 * @param       none
 * @retval      none
 */
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;

    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup); /* will be receivedRMTData is sent to parsing tasks via message queue */
    return high_task_wakeup == pdTRUE;
}