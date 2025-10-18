/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-09 15:02:18
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-10-12 18:29:59
 * @FilePath     : \esp-idf\pro\S3_LVGL\components\read_nvs\updata.c
 */

#include "cJSON.h"
#include "updata.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
static const char *TAG="NVS";
int save_nvs(const char * str,char * data)
{
    nvs_handle my_handle;//nvshandle
    esp_err_t err = nvs_open("info", NVS_READWRITE, &my_handle);//Open wifipass space
    err = nvs_set_str(my_handle,str,data);
    err =nvs_commit(my_handle);
    if(err==ESP_OK)
        ESP_LOGI(TAG,"save%s:%s",str,data);
    else
    {
        ESP_LOGI(TAG,"save失败");  
        return 0;
    }
    nvs_close(my_handle);
    return 1;
}
int read_nvs(const char * str,char * data)
{
    /*Openwifipassworkspace*/
    size_t len =100;
    nvs_handle my_handle;//nvshandle
    esp_err_t err = nvs_open("info", NVS_READWRITE, &my_handle);//Openwifipassspace
    /*fromnvsobtained fromssidand password*/
    err =nvs_get_str(my_handle,str,data,&len);
    if(err==ESP_OK)
        ESP_LOGI(TAG,"read%s:%s",str,data);
    else
    {
        ESP_LOGI(TAG,"Read failed"); 
        return 0;
    }  
    nvs_close(my_handle);
    return 1; 
}
void save_web_data(char *data)
{
    cJSON *root,*which;
    root = cJSON_Parse(data);
    if(root!=NULL)
    {
        which = cJSON_GetObjectItem(root, "n");
        if(which!=NULL)
        {
            if(which->valueint==1)//Required items
            {
                cJSON *uid=cJSON_GetObjectItem(root,"uid");//Get Bilibiliuid
                save_nvs("uid",uid->valuestring);
                cJSON *city=cJSON_GetObjectItem(root,"city");//Get the city
                save_nvs("city",city->valuestring);
            }
            else if(which->valueint==0)
            {
                cJSON *pass=cJSON_GetObjectItem(root,"pass");//Get the weather key
                save_nvs("pass",pass->valuestring);

            }else if(which->valueint==2)
            {
				cJSON *img_client_id=cJSON_GetObjectItem(root,"img_client_id");//Get the weather key
                save_nvs("img_client_id",img_client_id->valuestring);
				cJSON *img_client_secret=cJSON_GetObjectItem(root,"img_client_secret");//Get the weather key
                save_nvs("img_secret",img_client_secret->valuestring);
				cJSON *img_token=cJSON_GetObjectItem(root,"img_token");//Get the weather key
                save_nvs("img_token",img_token->valuestring);
				cJSON *ocr_client_id=cJSON_GetObjectItem(root,"ocr_client_id");//Get the weather key
                save_nvs("ocr_client_id",ocr_client_id->valuestring);
				cJSON *ocr_client_secret=cJSON_GetObjectItem(root,"ocr_client_secret");//Get the weather key
                save_nvs("ocr_secret",ocr_client_secret->valuestring);
				cJSON *ocr_token=cJSON_GetObjectItem(root,"ocr_token");//Get the weather key
                save_nvs("ocr_token",ocr_token->valuestring);

                // FILE* fp = fopen("/spiffs/mqtt.txt", "w");
                // fputs( (const char *)data, fp );
                // fputc( '\0',fp );
                // fclose(fp);
            }
        }
    cJSON_Delete(root);        
    }

}