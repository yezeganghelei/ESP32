/**
 ****************************************************************************************************
 * @file        emission.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       RMT infrared decoding driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "emission.h"

/* Save the address and command bytes decoded from NEC */
uint16_t s_nec_code_address;
uint16_t s_nec_code_command;

QueueHandle_t receive_queue;
uint8_t tbuf[40];
extern uint8_t rmt_flag;

/**
 * @brief       Initialize RMT
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
        .mem_block_symbols = 64,                                                                                        /* Number of RMT symbols the channel can store at one time */
        .gpio_num = RMT_RX_PIN,                                                                                         /* RMT receive channel pin */
    };
    rmt_channel_handle_t rx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));                                                  /* Create an RMT receive channel */

    /* Configure message queue */
    QueueHandle_t receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));                                    /* Create a message queue used by the RMT receive callback */
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = RMT_Rx_Done_Callback,                                                                           /* Event callback, called when the RMT channel completes a receive transaction */
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));                                  /* Register the callback for the RMT RX channel */

    /* The following timing requirements are based on the NEC protocol */
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,                                                                                    /* Minimum NEC signal duration is 560us; 1250ns < 560us so valid signals are not treated as noise */
        .signal_range_max_ns = 12000000,                                                                                /* Maximum NEC signal duration is 9000us; 12000000ns > 9000us so reception does not stop early */
    };

    /* Configure the sending channel */
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                                                                                 /* RMT transmit channel clock source */
        .resolution_hz = RMT_RESOLUTION_HZ,                                                                             /* RMT transmit channel clock resolution */
        .mem_block_symbols = 64,                                                                                        /* Number of RMT symbols the channel can store at one time */
        .trans_queue_depth = 4,                                                                                         /* Number of transactions allowed to be pending in the background; this example does not queue multiple transactions, so a queue depth > 1 is enough */
        .gpio_num = RMT_TX_PIN,                                                                                         /* RMT transmit channel pin */
    };
    rmt_channel_handle_t tx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));                                                  /* Create an RMT transmit channel */

    /* Configure the carrier and duty cycle */
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = 38000,                                                                                          /* Carrier frequency; 0 disables the carrier */
        .duty_cycle = 0.33,                                                                                             /* Carrier duty cycle */
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_channel, &carrier_cfg));                                                       /* Apply a modulation function to the transmit channel */

    /* Do not send the NEC frame in a loop */
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,                                                                                                /* 0 = no loop, -1 = infinite loop */
    };

    /* Configure the encoder */
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = RMT_RESOLUTION_HZ,                                                                                /* Encoder resolution */
    };
    rmt_encoder_handle_t nec_encoder = NULL;
    ESP_ERROR_CHECK(rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));                                            /* Configure the encoder */

    /* Enable the transmit and receive channels */
    ESP_ERROR_CHECK(rmt_enable(tx_channel));                                                                            /* Enable the transmit channel */
    ESP_ERROR_CHECK(rmt_enable(rx_channel));                                                                            /* Enable receive channel */

    /* Buffer for received RMT symbols */
    rmt_symbol_word_t raw_symbols[64];                                                                                  /* 64 symbols is sufficient for a standard NEC frame */
    rmt_rx_done_event_data_t rx_data;

    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));                        /* ready to receive */

    while (1)
    {
        if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS)                                      /* Wait for the RX-done signal */
        {
            example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);                                     /* Parse the received symbols and print the result */
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
            ESP_ERROR_CHECK(rmt_transmit(tx_channel, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));    /* Transmit data through the RMT TX channel */
        }
    }

    rmt_disable(tx_channel);
    rmt_disable(rx_channel);
    vQueueDelete(receive_queue);
}

/**
 * @brief       Check whether the signal duration is within the NEC timing tolerance (spec_duration +/- RMT_NEC_DECODE_MARGIN)
 * @param       none
 * @retval      none
 */
inline bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + RMT_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - RMT_NEC_DECODE_MARGIN));
}

/**
 * @brief       Compare the timing length to determine whether it is logic 0
 * @param       none
 * @retval      none
 */
bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief       Compare the timing length to determine whether it is logic 1
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

    /* Save the data address and command to detect repeated keys */
    s_nec_code_address = address;
    s_nec_code_command = command;

    return true;
}

/**
 * @brief       Check whether the data frame is a repeat frame: the same key is held down
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
    switch (symbol_num) /* Decode the RMT received data */
    {
        case 34:        /* Normal NEC data frame */
        {
            if (nec_parse_frame(rmt_nec_symbols) )
            {
                rmt_flag = 1;
                printf("RX CMD = %d\n", s_nec_code_command);
            }
            break;
        }
        
        case 2:         /* Repeat NEC data frame */
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
 * @brief       RMT data reception completion callback
 * @param       none
 * @retval      none
 */
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;

    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup); /* Send the received RMT data to the parsing task via the message queue */
    return high_task_wakeup == pdTRUE;
}