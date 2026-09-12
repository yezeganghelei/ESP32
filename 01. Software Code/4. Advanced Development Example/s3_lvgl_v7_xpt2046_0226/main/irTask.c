
#include "irTask.h"

#define GREE_CODE_2 0x0200a59
#define GREE_CODE_4 0x0200a59
#define MEIDI_CODE_1 0x6f9b24d
#define MEIDI_CODE_2 0x0200a59
#define MEIDI_CODE_4 0x96620df
#define MEIDI_CODE_5 0xa65d02f
#define HAIER_CODE_1 0x0003565
#define HAIER_CODE_2 0xcd28f70
#define HAIER_CODE_3 0x8565
#define HAIER_CODE_5 0x73565

static const char *TAG = "IR_task";

static const int rx_channel = RMT_RX_CHANNEL;
static const int tx_channel = RMT_TX_CHANNEL;

EXT_RAM_ATTR uint16_t decoded[1024] = {0}; // Data decoded from the IR binary file

static struct AC_Control ac_handle; // Air conditioner object (based on irext)

static uint8_t band, pro_code;

TaskHandle_t ir_tx_handle;
// Restrict IR operations: transmit and receive cannot happen simultaneously
static SemaphoreHandle_t IR_sem;

// Paths to the IR code library binary files
const char *ir_code_lib[] = {
    // Gree code library
    "/spiffs/gree/ac_gree_1.bin",
    "/spiffs/gree/ac_gree_2.bin",
    "/spiffs/gree/ac_gree_3.bin",
    "/spiffs/gree/ac_gree_4.bin",
    "/spiffs/gree/ac_gree_5.bin",

    // Midea code library
    "/spiffs/meidi/ac_meidi_1.bin",
    "/spiffs/meidi/ac_meidi_2.bin",
    "/spiffs/meidi/ac_meidi_3.bin",
    "/spiffs/meidi/ac_meidi_4.bin",
    "/spiffs/meidi/ac_meidi_5.bin",

    // Panasonic code library
    "/spiffs/songxia/ac_songxia_1.bin",
    "/spiffs/songxia/ac_songxia_2.bin",
    "/spiffs/songxia/ac_songxia_3.bin",
    "/spiffs/songxia/ac_songxia_4.bin",
    "/spiffs/songxia/ac_songxia_5.bin",

    // Daikin code library
    "/spiffs/dajin/ac_dajin_1.bin",
    "/spiffs/dajin/ac_dajin_2.bin",
    "/spiffs/dajin/ac_dajin_3.bin",
    "/spiffs/dajin/ac_dajin_4.bin",
    "/spiffs/dajin/ac_dajin_5.bin",

    // Haier code library
    "/spiffs/haier/ac_haier_1.bin",
    "/spiffs/haier/ac_haier_2.bin",
    "/spiffs/haier/ac_haier_3.bin",
    "/spiffs/haier/ac_haier_4.bin",
    "/spiffs/haier/ac_haier_5.bin",

    // Hisense code library
    "/spiffs/haixin/ac_haixin_1.bin",
    "/spiffs/haixin/ac_haixin_2.bin",
    "/spiffs/haixin/ac_haixin_3.bin",
    "/spiffs/haixin/ac_haixin_4.bin",
    "/spiffs/haixin/ac_haixin_5.bin",

    // AUX code library
    "/spiffs/aokesi/ac_aux_1.bin",
    "/spiffs/aokesi/ac_aux_2.bin",
    "/spiffs/aokesi/ac_aux_3.bin",
    "/spiffs/aokesi/ac_aux_4.bin",
    "/spiffs/aokesi/ac_aux_5.bin",
};
/*----------------------------------------------Air conditioner function settings----------------------------------------------------------------------*/
/*
 * Set the air conditioner code and save it to NVS
 */
uint8_t ac_set_code_lib(uint8_t band, uint8_t pro_code)
{
    ac_handle.code = band * 5 + pro_code;
    return nvs_save_ac_code(ac_handle.code, AC_DEFAULT);
}

/*
 * Transmit IR according to ac_handle
 */
void ac_control()
{

    xSemaphoreTake(IR_sem, portMAX_DELAY);
    xTaskNotifyGive(ir_tx_handle);
}
int ac_set_temp(int temp)
{
    if (temp > 28 || temp < 16)
    {
        return -1;
    }
    ac_handle.status.ac_temp = temp - 16;
    ac_control();
    return 0;
}

int ac_open(bool open)
{
    if (open)
    {
        ac_handle.status.ac_power = AC_POWER_ON;
    }
    else
    {
        ac_handle.status.ac_power = AC_POWER_OFF;
    }
    ac_control();
    return 0;
}

int ac_set_wind_speed(int speed)
{
    if (speed < 0 || speed > 3)
    {
        return -1;
    }
    switch (speed)
    {
    case 0:
        ac_handle.status.ac_wind_speed = AC_WS_AUTO;
        break;
    case 1:
        ac_handle.status.ac_wind_speed = AC_WS_LOW;
        break;
    case 2:
        ac_handle.status.ac_wind_speed = AC_WS_MEDIUM;
        break;
    case 3:
        ac_handle.status.ac_wind_speed = AC_WS_HIGH;
        break;
    default:
        return -1;
    }
    ac_control();
    return 0;
}

int ac_set_swing(bool open)
{
    if (open)
    {
        ac_handle.status.ac_wind_dir = AC_SWING_ON;
    }
    else
    {
        ac_handle.status.ac_wind_dir = AC_SWING_OFF;
    }
    ac_control();
    return 0;
}

/*
 * @brief Fill the item level and level duration; the time must be converted to counter ticks: /10*RMT_TICK_10_US
 */
static inline void nec_fill_item_level(rmt_item32_t *item, int high_us, int low_us)
{
    item->level0 = 1;
    item->duration0 = (high_us) / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = (low_us) / 10 * RMT_TICK_10_US;
}
/*
 * irext_build
 * brief: Build items via irext using global variables
 */
static void irext_build(rmt_item32_t *item, size_t item_num)
{
    int i = 0;

    nec_fill_item_level(item, decoded[0], decoded[1]);
    for (i = 1; i < item_num; i++)
    {
        item++;
        nec_fill_item_level(item, decoded[2 * i], decoded[2 * i + 1]);
    }
}

/*---------------------------------------------- IR receive item data parsing (no longer used) --------------------------------------------------*/

/*
 * @brief Check whether duration is around target_us
 * Check whether the duration in the item matches the target time
 */
inline bool check_in_duration(int duration_ticks, uint32_t target_us, int margin_us)
{
    //ESP_LOGI(TAG, "duration_ticks = %d,target_us=%d,margin_us=%d" ,NEC_ITEM_DURATION(duration_ticks), target_us, margin_us);
    if ((NEC_ITEM_DURATION(duration_ticks) < (target_us + margin_us)) && (NEC_ITEM_DURATION(duration_ticks) > (target_us - margin_us)))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*
 * Check whether the input item is 1
 * item: the item to check
 * sig: input signal structure
 */
static bool check_bit_one(rmt_item32_t *item, struct RX_signal *sig)
{
    //ESP_LOGI(TAG, "sig_low=%u,sig->highlevel_1=%u" ,sig->lowlevel,sig->highlevel_1);
    if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, sig->lowlevel, NEC_BIT_MARGIN) && check_in_duration(item->duration1, sig->highlevel_1, NEC_BIT_MARGIN))
    {
        return true;
    }
    return false;
}

/*
 * Check whether the input item is 0
 * item: the item to check
 * sig: input signal structure
 */
static bool check_bit_zero(rmt_item32_t *item, struct RX_signal *sig)
{

    if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, sig->lowlevel, NEC_BIT_MARGIN) && check_in_duration(item->duration1, sig->highlevel_0, NEC_BIT_MARGIN))
    {
        return true;
    }
    return false;
}

/*
 * Check the start frame of the input signal
 * item: the first item of the signal
 */
static bool check_header(rmt_item32_t *item)
{

    ESP_LOGI(TAG, "item head level0 = %u,duration0 = %u, level1 = %u, duration1=%u", NEC_ITEM_DURATION(item->level0), NEC_ITEM_DURATION(item->duration0), NEC_ITEM_DURATION(item->level1), NEC_ITEM_DURATION(item->duration1));

    if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, HEADER_HIGH_9000US, NEC_BIT_MARGIN) && check_in_duration(item->duration1, HEADER_LOW_4500US, NEC_BIT_MARGIN))
    {
        // All GREE models
        band = band_gree;
        return true;
    }
    else if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, HEADER_LOW_4500US, NEC_BIT_MARGIN) && check_in_duration(item->duration1, HEADER_LOW_4500US, NEC_BIT_MARGIN))
    {
        // Midea No.1
        band = band_meidi;
        return true;
    }
    else if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, HEADER_LOW_4300US, NEC_BIT_MARGIN) && check_in_duration(item->duration1, HEADER_HIGH_4300US, NEC_BIT_MARGIN))
    {
        // Midea No.2
        band = band_meidi;
        return true;
    }
    else if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, HEADER_LOW_5800US, NEC_BIT_MARGIN) && check_in_duration(item->duration1, HEADER_HIGH_7300US, NEC_BIT_MARGIN))
    {
        // Midea No.4,5
        band = band_meidi;
        return true;
    }
    else if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, HEADER_LOW_3000US, NEC_BIT_MARGIN) && check_in_duration(item->duration1, HEADER_HIGH_3000US, NEC_BIT_MARGIN))
    {
        item++; // Note
        if ((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL) && check_in_duration(item->duration0, HEADER_LOW_3000US, NEC_BIT_MARGIN) && check_in_duration(item->duration1, HEADER_HIGH_4500US, NEC_BIT_MARGIN))
        {
            // Haier
            band = band_haier;
            return true;
        }
        return false;
    }
    else
    {
        return false;
    }
}

/*
 * Parse the received IR signal
 * item: IR information read from the ring buffer
 * item_num: number of items, one item is 32 bits
 * sig: parsing result is stored in sig
 */
static int parse_items(rmt_item32_t *item, int item_num, struct RX_signal *sig)
{
    int i;
    uint64_t encode = 0;

    // Check the start bit
    if (!check_header(item))
    {
        ESP_LOGI(TAG, "header check err;");
        return -2;
    }
    ESP_LOGI(TAG, "band:%u", band);
    item++; // Point item to the code segment
    if (band == band_haier)
    {
        // Haier has two start segments
        item++;
    }
    // Find three values: low-level time and high-level times

    if (item->level0 == RMT_RX_ACTIVE_LEVEL)
    {
        sig->lowlevel = NEC_ITEM_DURATION(item->duration0);
        //ESP_LOGI(TAG, "lowlevel = %u",sig->lowlevel);
    }
    rmt_item32_t *t_item = item;
    // Iterate over items to determine highlevel_1 and highlevel_0
    for (; sig->highlevel_0 == 0 || sig->highlevel_1 == 0; t_item++)
    {
        uint32_t duration = NEC_ITEM_DURATION(t_item->duration1);

        if (duration > 1500)
        {
            sig->highlevel_1 = duration;
        }
        else
        {
            sig->highlevel_0 = duration;
        }
    }
    ESP_LOGI(TAG, "sig: lowlevel = %u highlevel_1 = %u  highlevel_0 = %u", sig->lowlevel, sig->highlevel_1, sig->highlevel_0);
    // Parse the encoded data; currently only the first 28 bits are checked
    // Check data bits
    for (i = 0; i < 28; i++)
    {
        //ESP_LOGI(TAG, "item->duration0 = %u,item->duration1 = %u", NEC_ITEM_DURATION(item->duration0), NEC_ITEM_DURATION(item->duration1));

        if (check_bit_one(item, sig))
        {
            encode |= (1 << i);
        }
        else if (check_bit_zero(item, sig))
        {

            encode |= (0 << i);
        }
        else
        {
            ESP_LOGI(TAG, "item->duration0 = %u,item->duration1 = %u", NEC_ITEM_DURATION(item->duration0), NEC_ITEM_DURATION(item->duration1));
            return -3;
        }
        item++;
    }
    sig->encode = encode;
    ESP_LOGI(TAG, "encode = %x", sig->encode);
    return 0;
}
/*---------------------------------------------------- Hardware initialization -------------------------------------------------*/
/*
 * @brief RMT transmitter initialization
 */
static void nec_tx_init()
{
    rmt_config_t rmt_tx;
    rmt_tx.channel = tx_channel;                     // RMT transmit channel
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;               // Pin that generates the RMT waveform
    rmt_tx.mem_block_num = 2;                        // Gree IR uses 70 items, so 2 memory blocks are used: 64x2=128 items
    rmt_tx.clk_div = RMT_CLK_DIV;                    // RMT clock divider
    rmt_tx.tx_config.loop_en = false;                // Disable loop transmission; transmit only once
    rmt_tx.tx_config.carrier_duty_percent = 50;      // Carrier duty cycle is 50%
    rmt_tx.tx_config.carrier_freq_hz = 38000;        // Carrier frequency 38 kHz (IR)
    rmt_tx.tx_config.carrier_level = 1;              // Carrier high level
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN; // Enable carrier
    rmt_tx.tx_config.idle_level = 0;                 // Idle level low
    rmt_tx.tx_config.idle_output_en = true;          // Output enable
    rmt_tx.rmt_mode = RMT_MODE_TX;                   // Transmit mode
    ESP_LOGI(TAG, "[ 1.1 ] config rmt");
    rmt_config(&rmt_tx); // Configure the RMT controller
    ESP_LOGI(TAG, "[ 1.2 ] install rmt driver");

    // Enable the IR transmit driver
    rmt_driver_install(rmt_tx.channel, 0, 0);
}

/*
 * @brief RMT receiver initialization
 */
static void nec_rx_init()
{
    rmt_config_t rmt_rx;
    rmt_rx.channel = rx_channel;
    rmt_rx.gpio_num = RMT_RX_GPIO_NUM;                                               // IR receive pin
    rmt_rx.clk_div = RMT_CLK_DIV;                                                    // Clock divider 100
    rmt_rx.mem_block_num = 2;                                                        // Gree IR uses 70 items, so 2 memory blocks are used: 64x2=128 items
    rmt_rx.rmt_mode = RMT_MODE_RX;                                                   // Receive mode
    rmt_rx.rx_config.filter_en = true;                                               // Enable the filter
    rmt_rx.rx_config.filter_ticks_thresh = 100;                                      // Filter signal width 100*80M = 12.5 us
    rmt_rx.rx_config.idle_threshold = rmt_item32_tIMEOUT_US / 10 * (RMT_TICK_10_US); // Set the receive exit time: if the input signal does not change within 21000 us, stop receiving
    rmt_config(&rmt_rx);                                                             // Configure RMT
    ESP_LOGI(TAG, "rmt rx config");
    // Enable the RMT driver and set up a 1000-byte ring buffer for received IR data
    rmt_driver_install(rmt_rx.channel, 1000, 0);
    ESP_LOGI(TAG, "rx driver initialization ok");
}

/*--------------------------------------- Interface functions ----------------------------------------------------*/

/*
 * Start IR reception
 * This function unblocks rmt_ir_rxTask() so it can receive data
 */
void ir_study()
{
    xSemaphoreTake(IR_sem, portMAX_DELAY);
    ESP_LOGI(TAG, "please send message");
    rmt_tx_stop(tx_channel);     // Stop transmitting to avoid affecting reception
    rmt_rx_start(rx_channel, 1); // Start receiving
}

/*
 * Determine which code library the signal belongs to and update ac_handle
 */
static int ir_code_lib_update(struct RX_signal *sig)
{
    switch (band)
    {
    case band_gree:
        if (sig->item_num == 70 && sig->encode == GREE_CODE_2)
        {
            ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_gree * 5 + code_2]);
            pro_code = code_2;
        }
        else if (sig->item_num == 36 && sig->encode == GREE_CODE_4)
        {
            //todo 36?
            ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_gree * 5 + code_4]);
            pro_code = code_4;
        }
        else
        {
            return -1;
        }
        break;
    case band_meidi:
        if (sig->lowlevel < 5000)
        {
            if (sig->encode == MEIDI_CODE_1)
            {
                ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_meidi * 5 + code_1]);
                pro_code = code_1;
            }
            else if (sig->encode == MEIDI_CODE_2)
            {
                ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_meidi * 5 + code_2]);
                pro_code = code_2;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            if (sig->encode == MEIDI_CODE_4)
            {
                ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_meidi * 5 + code_4]);
                pro_code = code_4;
            }
            else if (sig->encode == MEIDI_CODE_5)
            {
                ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_meidi * 5 + code_5]);
                pro_code = code_5;
            }
            else
            {
                return -1;
            }
        }
        break;
    case band_haier:
        if (sig->encode == HAIER_CODE_1)
        {
            ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_haier * 5 + code_1]);
            pro_code = code_1;
        }
        else if (sig->encode == HAIER_CODE_2)
        {
            ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_meidi * 5 + code_2]);
            pro_code = code_2;
        }
        else if (sig->encode == HAIER_CODE_3)
        {
            ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_meidi * 5 + code_3]);
            pro_code = code_3;
        }
        else if (sig->encode == HAIER_CODE_5)
        {
            ESP_LOGI(TAG, "update ir_code_lib:%s", ir_code_lib[band_meidi * 5 + code_5]);
            pro_code = code_5;
        }
        else
        {
            return -1;
        }
        break;
    default:
        return -1;
    }

    ac_set_code_lib(band, pro_code);

    return 0;
}
/*--------------------------------------- Receive/transmit task functions ----------------------------------------------------*/
/*
 * IR receive task; data is received only after rmt_rx_start() is called
 * The received data is stored in NVS using the item data structure
*/
void rmt_ir_rxTask(void *agr)
{
    size_t rx_size = 0;

    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(rx_channel, &rb); // Get the data received by the IR receiver, stored in the ring buffer

    rmt_rx_stop(rx_channel); // Stop receiving
    while (rb)
    {

        // Read items from the ring buffer; blocks until new data is available
        rmt_item32_t *item = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, portMAX_DELAY);
        if (item)
        {
            ESP_LOGI(TAG, "rx_size = %u", rx_size);

            //! The IR receiver has noise, so filtering is required
            if (rx_size > 30)
            {
                struct RX_signal sig;          // Received signal structure
                size_t item_num = rx_size / 4; // One item is 32 bits
                sig.item_num = item_num;
                sig.highlevel_1 = 0;
                sig.highlevel_0 = 0;
                sig.encode = 0;
                sig.lowlevel = 0;
                // Parse items
                parse_items(item, item_num, &sig);

                ir_code_lib_update(&sig); // Update ac_handle
                rmt_rx_stop(rx_channel);  // Stop receiving
                xSemaphoreGive(IR_sem);   // Release the semaphore
            }

            // Release the ring buffer space after parsing
            vRingbufferReturnItem(rb, (void *)item);
        }
    }
    vTaskDelete(NULL);
}

/**
 * IR transmit task 
 */
void rmt_ir_txTask(void *agr)
{
    rmt_item32_t *item; // Transmit item
    size_t size = 0;    // Memory required for items
    int item_num = 0;
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification

        ESP_LOGI(TAG, "ir_tx_irext:power=%d,temperature =%d", ac_handle.status.ac_power, ac_handle.status.ac_temp);
        ESP_LOGI(TAG, "using code lib:%s", ir_code_lib[ac_handle.code]);
        // Open the irext library binary file for ac_handle
        if (ir_file_open(1, 0, ir_code_lib[ac_handle.code]) != 0)
        {
            ESP_LOGI(TAG, "open file fail");
            goto tx_exit;
        }

        // Decode the specific IR sequence from ac_handle.status
        uint16_t decode_len = ir_decode(KEY_AC_POWER, decoded, &ac_handle.status, 0);

        // Check the sequence length
        if (decode_len > 200)
        {
            decode_len = (decode_len + 1) / 2;
        }
        // Close the irext library and free memory
        ir_close();

        // Build items from the IR sequence
        item_num = (decode_len / 2); // The decoded sequence is duplicated; take half

        size = (sizeof(rmt_item32_t) * item_num);
        item = (rmt_item32_t *)malloc(size);
        irext_build(item, item_num); // Build items from the sequence

        ESP_LOGI(TAG, "write item num = %d", item_num);

        rmt_write_items(tx_channel, item, item_num, true); // Write the item set to the transmit channel RAM

        rmt_wait_tx_done(tx_channel, portMAX_DELAY); // Transmit the IR carrier

        free(item); // Remember to free the dynamically allocated memory
    tx_exit:
        xSemaphoreGive(IR_sem); // Release the semaphore
    }
    vTaskDelete(NULL);
}
void ac_set_type(uint8_t ac)
{
    ESP_LOGI(TAG, "ac_set_type");
    // No code number provided; use the default
    band = ac;
    pro_code = code_2;

    if (ac_set_code_lib(band, pro_code) != ESP_OK) // Save to NVS
    {
        ESP_LOGI(TAG, "save ac code from nvs fail");
    }
    ESP_LOGI(TAG, "set ac lib to default:%u", ac_handle.code);
}
void ac_set_type1(uint8_t ac,uint8_t code)
{
    ESP_LOGI(TAG, "ac_set_type");
    // No code number provided; use the default
    band = ac;
    pro_code = code;

    if (ac_set_code_lib(band, pro_code) != ESP_OK) // Save to NVS
    {
        ESP_LOGI(TAG, "save ac code from nvs fail");
    }
    ESP_LOGI(TAG, "set ac lib to default:%u", ac_handle.code);
}
/*
 * IR initialization
 * brief: Initialize IR-related variables and peripherals, and create tasks
 * Returns: 1 on success
*/
int IR_init()
{
    esp_log_level_set(TAG, ESP_LOG_INFO);

    // Initialize the IR information in the air conditioner structure
    ac_handle.status.ac_mode = AC_MODE_COOL;
    ac_handle.status.ac_power = AC_POWER_ON;
    ac_handle.status.ac_temp = AC_TEMP_26;
    ac_handle.status.ac_wind_dir = AC_SWING_ON; // Enable swing
    ac_handle.status.ac_wind_speed = AC_WS_LOW;

    uint8_t *temp; // Buffer for the code library number
    // Read the code library number from NVS
    temp = nvs_get_ac_lib(AC_DEFAULT);

    // First use; no code library number saved in NVS
    if (temp == NULL)
    {
        ESP_LOGI(TAG, "use default code lib");
        // No code number provided; use the default
        band = band_aux;
        pro_code = code_1;

        if (ac_set_code_lib(band, pro_code) != ESP_OK) // Save to NVS
        {
            ESP_LOGI(TAG, "save ac code from nvs fail");
        }
        ESP_LOGI(TAG, "set ac lib to default:%u", ac_handle.code);
    }
    else 
    {
        ESP_LOGI(TAG, "nvs_get_ac_lib temp=%u", *temp);
        ac_handle.code = *temp; // Successfully read from NVS; save to ac_handle
        free(temp);
    }

    vSemaphoreCreateBinary(IR_sem); // Create a semaphore to synchronize transmit and receive

    nec_tx_init(); // Initialize the transmitter
    // nec_rx_init();

    return 1;
}
