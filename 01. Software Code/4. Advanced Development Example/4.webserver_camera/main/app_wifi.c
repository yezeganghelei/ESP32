/*
 * @Author: Kevincoooool
 * @Date: 2023-04-19 14:59:02
 * @Description:
 * @version:
 * @Filename: Do not Edit
 * @LastEditTime: 2024-03-19 20:50:19
 * @FilePath: \SP_V2_DEMO\4.webserver_camera\main\app_wifi.c
 */
/* ESPRESSIF MIT License

 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>

 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "app_wifi.h"

static const char *TAG = "app_wifi";
const int CONNECTED_BIT = BIT0;
const int ESPTOUCH_DONE_BIT = BIT1;
const int WIFI_SMART = BIT2;
const int WIFI_CONNET_BIT = BIT3; // Distribution network连接
const int MQTT_CONNET_BIT = BIT4;

char ip_adder[20];
char ssid[40];
EventGroupHandle_t s_wifi_event_group; // wifi event group
static bool wifi_connect_status = false;
static esp_ip6_addr_t s_ipv6_addr;

/* types of ipv6 addresses to be displayed on ipv6 events */
static const char *s_ipv6_addr_types[] = {
    "ESP_IP6_ADDR_IS_UNKNOWN",
    "ESP_IP6_ADDR_IS_GLOBAL",
    "ESP_IP6_ADDR_IS_LINK_LOCAL",
    "ESP_IP6_ADDR_IS_SITE_LOCAL",
    "ESP_IP6_ADDR_IS_UNIQUE_LOCAL",
    "ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6"
};
#define EXAMPLE_ESP_WIFI_MODE_AP 0 // TRUE:AP FALSE:STA
#define EXAMPLE_ESP_WIFI_SSID "CMCC-"
#define EXAMPLE_ESP_WIFI_PASS "99999999"
#define EXAMPLE_MAX_STA_CONN 4
#define EXAMPLE_IP_ADDR "192.168.4.1"

static void net_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    static int s_disconnected_handshake_count = 0;
    static int s_disconnected_unknown_count = 0;

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:

            break;
        case WIFI_EVENT_STA_CONNECTED:
            s_disconnected_unknown_count = 0;
            s_disconnected_handshake_count = 0;
            wifi_event_sta_connected_t *sta_connected = event_data;
            ESP_LOGI(TAG, "connected ssid: %s", sta_connected->ssid);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            uint8_t sta_conn_state = 0;
            wifi_event_sta_disconnected_t *disconnected = event_data;
            switch (disconnected->reason)
            {
            case WIFI_REASON_ASSOC_TOOMANY:
                ESP_LOGW(TAG, "WIFI_REASON_ASSOC_TOOMANY Disassociated because AP is unable to handle all currently associated STAs");
                ESP_LOGW(TAG, "The number of connected devices on the router exceeds the limit");

                sta_conn_state = 1;

                break;

            case WIFI_REASON_MIC_FAILURE:              /**< disconnected reason code 14 */
            case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:   /**< disconnected reason code 15 */
            case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT: /**< disconnected reason code 16 */
            case WIFI_REASON_IE_IN_4WAY_DIFFERS:       /**< disconnected reason code 17 */
            case WIFI_REASON_HANDSHAKE_TIMEOUT:        /**< disconnected reason code 204 */
                ESP_LOGW(TAG, "Wi-Fi 4-way handshake failed, count: %d", s_disconnected_handshake_count);

                if (++s_disconnected_handshake_count >= 3)
                {
                    ESP_LOGW(TAG, "Router password error");
                    sta_conn_state = 2;
                }
                break;

            default:
                if (++s_disconnected_unknown_count > 10)
                {
                    ESP_LOGW(TAG, "Router password error");
                    sta_conn_state = 3;
                }
                break;
            }

            if (sta_conn_state == 0)
            {
                ESP_ERROR_CHECK(esp_wifi_connect());
            }
            xEventGroupSetBits(s_wifi_event_group, LVGL_WIFI_CONFIG_CONNECT_FAIL);
            break;
        }
        case WIFI_EVENT_SCAN_DONE:
        {
            wifi_event_sta_scan_done_t *scan_done_data = event_data;
            uint16_t apCount = scan_done_data->number;
            if (apCount == 0)
            {
                ESP_LOGI(TAG, "[ * ] Nothing AP found");
                break;
            }

            wifi_ap_record_t *wifi_ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
            if (!wifi_ap_list)
            {
                ESP_LOGE(TAG, "[ * ] malloc error, wifi_ap_list is NULL");
                break;
            }
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, wifi_ap_list));

            // wifi_config_data.apCount = apCount;
            // wifi_config_data.ap_info_list = (ap_info_t *)malloc(apCount * sizeof(ap_info_t));
            // if (!wifi_config_data.ap_info_list)
            // {
            //     ESP_LOGE(TAG, "[ * ] realloc error, ap_info_list is NULL");
            //     break;
            // }
            // for (int i = 0; i < apCount; ++i)
            // {
            //     printf("ap_info_list[%d].ssid:%s \n", i, wifi_ap_list[i].ssid);
            //     wifi_config_data.ap_info_list[i].rssi = wifi_ap_list[i].rssi;
            //     wifi_config_data.ap_info_list[i].authmode = wifi_ap_list[i].authmode;
            //     memcpy(wifi_config_data.ap_info_list[i].ssid, wifi_ap_list[i].ssid, sizeof(wifi_ap_list[i].ssid));
            // }
            xEventGroupSetBits(s_wifi_event_group, LVGL_WIFI_CONFIG_SCAN_DONE);
            esp_wifi_scan_stop();
            free(wifi_ap_list);
            break;
        }
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        xEventGroupSetBits(s_wifi_event_group, LVGL_WIFI_CONFIG_CONNECTED);
    }
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        static uint32_t i = 0;
        if (i == 5)
        {
            i = 0;
            ESP_LOGI(TAG, "retry to connect to the AP");
            // smartconfig_set();
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        }

        i++;
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // uart_write_bytes(1, ip4addr_ntoa((ip4_addr_t *)&event->ip_info.ip), strlen((ip4_addr_t *)ip4addr_ntoa(&event->ip_info.ip)));
        sprintf(ip_adder, "%s", ip4addr_ntoa((ip4_addr_t *)&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        wifi_connect_status = true;
    }else if (event_id == IP_EVENT_GOT_IP6) {
        ESP_LOGI(TAG, "GOT IPv6 event!");

        ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
        ESP_LOGI(TAG, "Got IPv6 address " IPV6STR, IPV62STR(event->ip6_info.ip));
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
        ESP_LOGI(TAG, "Scan done");
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
        ESP_LOGI(TAG, "Found channel");
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    { // Get password
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        char password[65];

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true)
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);

        /*Openwifipassworkspace and save password*/
        // if (save_nvs("wifi_ssid", ssid) && save_nvs("wifi_pass", password))
        //     ESP_LOGI(TAG, "Password saved successfully");

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
        wifi_connect_status = true;
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        // wifi_connect_status = false;
        wifi_connect_status = false;
    }
}
// Distribution network任务
void smartconfig_example_task(void *parm)
{
    ESP_LOGI(TAG, "start smartconfig。。。。。。。1");
    EventBits_t uxBits;

    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS)); // chooseesptouchandairkissDistribution network
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    while (1)
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, false, false, portMAX_DELAY); // Waiting for distribution network event group
        // if(uxBits & CONNECTED_BIT)
        //     ESP_LOGI(TAG, "WiFi Connected to ap");
        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            xEventGroupSetBits(s_wifi_event_group, WIFI_SMART); //Send automatic network configuration flag
            vTaskDelete(NULL);
        }
    }
}

#if EXAMPLE_ESP_WIFI_MODE_AP
static void wifi_init_softap()
{
    tcpip_adapter_init();

    if (strcmp(EXAMPLE_IP_ADDR, "192.168.4.1"))
    {
        int a, b, c, d;
        sscanf(EXAMPLE_IP_ADDR, "%d.%d.%d.%d", &a, &b, &c, &d);
        tcpip_adapter_ip_info_t ip_info;
        IP4_ADDR(&ip_info.ip, a, b, c, d);
        IP4_ADDR(&ip_info.gw, a, b, c, d);
        IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
        ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(WIFI_IF_AP));
        ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(WIFI_IF_AP, &ip_info));
        ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(WIFI_IF_AP));
    }

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_AP, mac));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    snprintf((char *)wifi_config.ap.ssid, 32, "KSDIY_ESP32_CAM-%x%x", mac[4], mac[5]);
    wifi_config.ap.max_connection = 1;
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;

    esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s ", wifi_config.ap.ssid);
    char buf[80];
    sprintf(buf, "SSID:%s", wifi_config.ap.ssid);
    sprintf(buf, "PASSWORD:%s", wifi_config.ap.password);
}

#else

// static void wifi_config_event_task(void *args)
// {
//     while (1)
//     {
//         EventBits_t uxBits;
//         uxBits = xEventGroupWaitBits(s_wifi_event_group,
//                                      (LVGL_WIFI_CONFIG_SCAN | LVGL_WIFI_CONFIG_SCAN_DONE | LVGL_WIFI_CONFIG_CONNECTED | LVGL_WIFI_CONFIG_CONNECT_FAIL | LVGL_WIFI_CONFIG_TRY_CONNECT),
//                                      pdTRUE, pdFALSE, portMAX_DELAY);

//         switch (uxBits)
//         {
//         case LVGL_WIFI_CONFIG_SCAN:
//         {
//             wifi_scan_config_t scanConf = {
//                 .ssid = NULL,
//                 .bssid = NULL,
//                 .channel = 0,
//                 .show_hidden = false};
//             ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, false));
//             lvgl_acquire();
//             start_wifi_scan();
//             lvgl_release();
//             break;
//         }
//         case LVGL_WIFI_CONFIG_SCAN_DONE:
//         {
//             ESP_LOGI(TAG, "[ * ] running refresh wifi list：%d\n", wifi_config_data.apCount);
//             lvgl_acquire();
//             wifi_scan_done();
//             lvgl_release();
//             break;
//         }
//         case LVGL_WIFI_CONFIG_CONNECTED:
//             lvgl_acquire();
//             wifi_connect_success();
//             lvgl_release();
//             break;

//         case LVGL_WIFI_CONFIG_CONNECT_FAIL:
//             lvgl_acquire();
//             esp_wifi_disconnect();
//             wifi_connect_fail();
//             lvgl_release();
//             break;

//         case LVGL_WIFI_CONFIG_TRY_CONNECT:
//         {
//             wifi_config_t sta_config = {0};
//             strcpy((char *)sta_config.sta.ssid, wifi_config_data.ap_info_list[wifi_config_data.current_ap].ssid);
//             strcpy((char *)sta_config.sta.password, wifi_config_data.current_pwd);
//             ESP_LOGI(TAG, "[ * ] Select SSID：%s", sta_config.sta.ssid);
//             ESP_LOGI(TAG, "[ * ] Input Password：%s", sta_config.sta.password);
//             esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config);
//             esp_wifi_disconnect();
//             esp_wifi_connect();
//             break;
//         }

//         default:
//             break;
//         }
//     }
// }
static void on_got_ipv6(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
    // if (!is_our_netif(TAG, event->esp_netif)) {
    //     ESP_LOGW(TAG, "Got IPv6 from another netif: ignored");
    //     return;
    // }
    esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
    ESP_LOGI(TAG, "Got IPv6 event: Interface \"%s\" address: " IPV6STR ", type: %s", esp_netif_get_desc(event->esp_netif),
             IPV62STR(event->ip6_info.ip), s_ipv6_addr_types[ipv6_type]);
    // if (ipv6_type == EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE) {
    //     memcpy(&s_ipv6_addr, &event->ip6_info.ip, sizeof(s_ipv6_addr));
    // }
}

esp_err_t wifi_init_sta()
{
    char password[65];
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &on_got_ipv6, NULL));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &net_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &net_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_config_t wifi_config;
    bzero(&wifi_config, sizeof(wifi_config_t));
    // if (read_nvs("wifi_ssid", ssid) && read_nvs("wifi_pass", password)) //Read ssid
    // {
    //ESP_LOGI(TAG, "SSID:%s PASS:%s obtained successfully", ssid, password);
    //     ESP_LOGI(TAG, "get ssid %s", ssid);

    //     memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    //     memcpy(wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    //     memcpy(ssid, wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
    //     ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    //     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    //     ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    //     ESP_ERROR_CHECK(esp_wifi_start());
    // }
    // else
    // {
    ESP_LOGI(TAG, "SSID not obtained...use default");
    memcpy(wifi_config.sta.ssid, EXAMPLE_ESP_WIFI_SSID, strlen(EXAMPLE_ESP_WIFI_SSID));
    memcpy(wifi_config.sta.password, EXAMPLE_ESP_WIFI_PASS, strlen(EXAMPLE_ESP_WIFI_PASS));
    memcpy(ssid, wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    // xTaskCreate(smartconfig_example_task, "smartconfig_task", 1024 * 3, NULL, 4, NULL);
    // return ESP_FAIL;
    // }

    /*Enter blocking state waiting for connection*/
    EventBits_t uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, false, false, 100000 / portTICK_PERIOD_MS);
    if (uxBits & CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "WiFi Connected to ap");
        return ESP_OK;
    }
    return ESP_FAIL;
}
#endif

esp_err_t app_wifi_init()
{

    // #if EXAMPLE_ESP_WIFI_MODE_AP
    //     ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    //     wifi_init_softap();
    // #else
    s_wifi_event_group = xEventGroupCreate();
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    // xTaskCreate(wifi_config_event_task, "config_task", 4096, NULL, 4, NULL);

    return wifi_init_sta();
    // #endif
}
bool wifi_is_connected(void)
{
    return wifi_connect_status;
}

bool wifi_is_disconnected(void)
{
    return (wifi_connect_status == false) ? 1 : 0;
}
char *get_ip(void)
{
    return ip_adder;
}
char *get_ssid(void)
{
    return ssid;
}