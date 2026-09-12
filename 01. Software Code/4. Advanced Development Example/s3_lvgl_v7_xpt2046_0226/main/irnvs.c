#include "irnvs.h"
// #include "mywifi.h"
static const char *TAG = "MY_NVS";


/*
 * Write ac_handle data to NVS
 * ac_handle : the ac_handle to save
 * ac_size: length of ac_handle
 * key: key name
 */
esp_err_t nvs_save_ac_code(uint8_t code, const char *key)
{
    esp_err_t err;
    nvs_handle handle;
    size_t size = 0;

    printf("open the nvs:%s\n", AC_CODE_NAMESPACE);
    err = nvs_open(AC_CODE_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        printf("nvs open fail\n");
        return err;
    }
    // Write the item collection entry
    printf("write to blob\n");
    err = nvs_set_u8(handle, key, code);
    if (err != ESP_OK)
    {
        printf("set blob fial");
        return err;
    }
    printf( "save in nvs success,key = %s,item_size = %d", key, 1);

    // Commit
    
    err = nvs_commit(handle);
    if (err != ESP_OK)
    {
        printf("commit blob fial");
        return err;
    }

    // Remember to close
    nvs_close(handle);

    return ESP_OK;
}

/*
 * Read ac_handle from NVS
 * key: key name
 * len: number of bytes of ac_handle
 * Returns the ac_handle pointer
*/
uint8_t *nvs_get_ac_lib(const char *key)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    size_t len = 0;
    nvs_handle handle;
    esp_err_t err;
    uint8_t *code;
    

    err = nvs_open(AC_CODE_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "nvs open fail");
        return NULL;
    }
    code = (uint8_t *)malloc(sizeof(uint8_t));
    // Check existence
    err = nvs_get_u8(handle, key,code);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "get u8 error,err = %d,key = %s",err,key);
        free(code);
        return NULL;
    }
    printf("get code = %u from nvs", *code);
    nvs_close(handle);

    return code;
}
/*
 * Write the item data to NVS
 * item: pointer to the item read from the ring buffer
 * items_size: total byte length of all items (one item is 32 bits)
 * name: key name
 */
esp_err_t nvs_save_items(rmt_item32_t *item, size_t items_size, const char *key)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    nvs_handle items_handle;
    esp_err_t err;
    size_t size = 0;

    // Open the IR receive storage in read/write mode
    err = nvs_open(IR_STORAGE_NAMESPACE, NVS_READWRITE, &items_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "nvs open fail");
        return err;
    }

    // Check whether the entry already exists
    err = nvs_get_blob(items_handle, key, NULL, &size);

    if (size != 0)
    {
        // Entry already exists; the previous data will be overwritten
        ESP_LOGI(TAG, "key has alreadey exist! will cover it");
    }

    // Write the item collection entry
    err = nvs_set_blob(items_handle, key, item, items_size);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "set blob fial");
        return err;
    }
    ESP_LOGI(TAG, "save in nvs success,key = %s,item_size = %d", key, items_size);

    // Commit
    err = nvs_commit(items_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "commit blob fial");
        return err;
    }

    // Remember to close
    nvs_close(items_handle);

    return ESP_OK;
}

/*
 * Read item from NVS
 * key: key name
 * item_size: returns the number of bytes of the item
 * Returns the item pointer
 * Note: call free(item) after use to release the memory
*/
rmt_item32_t *nvs_get_items(size_t *item_size, const char *key)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);
    nvs_handle items_handle;
    esp_err_t err;
    rmt_item32_t *items;

    err = nvs_open(IR_STORAGE_NAMESPACE, NVS_READWRITE, &items_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "nvs open fail");
        return NULL;
    }

    // Check existence
    err = nvs_get_blob(items_handle, key, NULL, item_size);
    if (err != ESP_OK || err == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGI(TAG, "get blob error,item may be delete");
        return NULL;
    }
    if (*item_size == 0)
    {
        ESP_LOGI(TAG, "key=%s do not exist!", key);
        return NULL;
    }
    items = (rmt_item32_t *)malloc(*item_size);
    if (items == NULL)
    {
        return NULL;
    }
    // Read item
    err = nvs_get_blob(items_handle, key, items, item_size);
    if (err != ESP_OK)
    {
        return NULL;
    }

    nvs_close(items_handle);

    return items;
}

/*
 * brief: Delete the item with the specified key from NVS
 * Returns the err status code
 */
esp_err_t nvs_delete_items(const char *key)
{
    nvs_handle items_handle;
    esp_err_t err;
    esp_log_level_set(TAG, ESP_LOG_INFO);
    size_t size = 0;
    err = nvs_open(IR_STORAGE_NAMESPACE, NVS_READWRITE, &items_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "open nvs fail");
        return err;
    }

    // Check existence
    err = nvs_get_blob(items_handle, key, NULL, &size);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "item don't exist");
        return err;
    }
    if (size == 0)
    {
        ESP_LOGI(TAG, "nvs do not exist");
        return err;
    }
    // Erase
    nvs_erase_key(items_handle, key);

    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, " %s erase fail", key);
        return err;
    }
    ESP_LOGI(TAG, "key=%s erase success!", key);

    nvs_close(items_handle);
    return ESP_OK;
}

