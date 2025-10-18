/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file emission.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief RMT infrared decoding driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "emission.h"

/* Save the address and command bytes of NEC decoded */
uint16_t s_nec_code_address;
uint16_t s_nec_code_command;

QueueHandle_t receive_queue;
uint8_t tbuf[40];

/**
 * @brief       initializationRMT
 * @param       none
 * @retval      none
 */
void emission_init(void)
{
    uint8_t t = 0;

    /* Configure the receiving channel */
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                                                                                 /* RMTReceive channel clock source */
        .resolution_hz = RMT_RESOLUTION_HZ,                                                                             /* RMT receiving channel clock resolution */
        .mem_block_symbols = 64,                                                                                        /* The channel can be stored at one timeRMTNumber of symbols */
        .gpio_num = RMT_RX_PIN,                                                                                         /* RMT Receive Channel Pin */
    };
    rmt_channel_handle_t rx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));                                                  /* Create aRMTReceive channel */

    /* Create a message queue，Receive infrared encoding */
    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));                                    /* Define a message queue，Used to deal withRMTReceived callback function */
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = RMT_Rx_Done_Callback,                                                                           /* Event callback, called when an RMT channel receives transactions complete */
    };
    /* Register infrared callback function */
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));                                  /* forRMT RXChannel setting callback */

    /* The following time requirements are based onNECprotocol */
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,                                                                                    /* NECMinimum duration of signalfor560us，1250ns＜560us，Valid signals are not considered noise */
        .signal_range_max_ns = 12000000,                                                                                /* The maximum duration of NEC signal is 9000us, 12000000ns>9000us, reception will not stop early */
    };

    /* Configure the sending channel */
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                                                                                 /* RMTSend channel clock source */
        .resolution_hz = RMT_RESOLUTION_HZ,                                                                             /* RMTSend channel clock resolution */
        .mem_block_symbols = 64,                                                                                        /* The channel can be stored at one timeRMTNumber of symbols */
        .trans_queue_depth = 4,                                                                                         /* The number of transactions allowed to be suspended in the background. This example will not queue multiple transactions, so the queue depth >1 is enough. */
        .gpio_num = RMT_TX_PIN,                                                                                         /* RMTSend channel pin */
    };
    rmt_channel_handle_t tx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));                                                  /* Create aRMTSend channel */

    /* Configure carrier and duty cycle s */
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = 38000,                                                                                          /* Carrier frequency，0Indicates that carrier is disabled */
        .duty_cycle = 0.33,                                                                                             /* Carrier duty cycle */
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_channel, &carrier_cfg));                                                       /* Apply modulation function to the sending channel */

    /* Not sent in a loopNECframe */
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,                                                                                                /* 0 is non-loop, -1 is infinite loop */
    };

    /* Configure the encoder */
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = RMT_RESOLUTION_HZ,                                                                                /* Encoder resolution */
    };
    rmt_encoder_handle_t nec_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));                                            /* Configure the encoder */

    /* Enable sending、Receive channel */
    ESP_ERROR_CHECK(rmt_enable(tx_channel));                                                                            /* Enable the sending channel */
    ESP_ERROR_CHECK(rmt_enable(rx_channel));                                                                            /* Enable the receiving channel */

    /* Save the received RMT symbol */
    rmt_symbol_word_t raw_symbols[64];                                                                                  /* 64 symbols should be sufficient for standard NEC frameworks */
    rmt_rx_done_event_data_t rx_data;

    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));                        /* Ready to receive */

    while (1)
    {
        if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS)                                      /* Wait for RX to complete signal */
        {
            example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);                                     /* Parses the received symbol and prints the result */
            ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));                /* Restart receiving */
        }
        else                                                                                                            /* time out，Transfer predefinedIR NECData Packet */
        {
            t++;

            if (t == 0)
            {
                t = 1;
            }

            const ir_nec_scan_code_t scan_code = {
                .command = t,
            };

            lcd_fill(116, 110, 176, 150, WHITE);
            sprintf((char *)tbuf, "%d", scan_code.command);
            printf("TX KEYVAL = %d\n", scan_code.command);
            lcd_show_string(116, 110, 200, 16, 16, (char *)tbuf, BLUE);
            ESP_ERROR_CHECK(rmt_transmit(tx_channel, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));    /* passRMTSend channel transmission data */
        }
    }
}

/**
 * @brief       Determine whether the data timing length isNECWithin the tolerance range of timing duration Positive and negative RMT_NEC_DECODE_MARGIN Within the value of
 * @param       none
 * @retval      none
 */
inline bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + RMT_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - RMT_NEC_DECODE_MARGIN));
}

/**
 * @brief compares the data timing length to determine whether it is logical 0
 * @param None
 * @retval None
 */
bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief       Comparing the data timing length to determine whether it isforlogic1
 * @param       none
 * @retval      none
 */
bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief       WillRMTDecode the received resultNECAddress and command
 * @param       none
 * @retval      none
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

    /* Save data address and command，Used to judgerepeatbutton */
    s_nec_code_address = address;
    s_nec_code_command = command;

    return true;
}

/**
 * @brief       Check the dataframewhetherforrepeatbutton：Keep holding the same key
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
    switch (symbol_num) /* Decode RMT receive data */
    {
        case 34:        /* normalNECdataframe */
        {
            if (nec_parse_frame(rmt_nec_symbols) )
            {
                lcd_fill(116, 130, 176, 150, WHITE);
                sprintf((char *)tbuf, "%d", s_nec_code_command);
                printf("RX KEYCNT = %d\n", s_nec_code_command);
                lcd_show_string(116, 130, 200, 16, 16, (char *)tbuf, BLUE);
            }
            break;
        }
        
        case 2:         /* repeatNEC dataframe */
        {
            if (nec_parse_frame_repeat(rmt_nec_symbols))
            {
                printf("RX KEYCNT = %d, repeat\n", s_nec_code_command);
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

    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup); /* Will receiveRMTdatapassMessage queue sent to parsing task */
    return high_task_wakeup == pdTRUE;
}