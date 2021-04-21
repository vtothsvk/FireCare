#include "apCare.h"

RTC_DATA_ATTR uint8_t dataBuffer[BUFFER_SIZE * BUFFER_BLOCK];
RTC_DATA_ATTR circular_buf_t cBuffer;
RTC_DATA_ATTR int bootcount = 0;
RTC_DATA_ATTR int lastAdv = 1;
RTC_DATA_ATTR bool needSyncFlag = true;
time_t epoch = 0;

const char* TAG = "apCare";

//jwt generator
authHandler auth;

void boot() {
    bootcount++;
    
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            printf("Blink Motherfucker!\r\n");
            //Start Wifi Provisioning
            wifiProvisioning();
        break;

        default:
            printf("Timer boot\r\n");
        break;
    }
}//boot

bool firstBoot() {
    return (bootcount == 1);
}//firstBoot

bool needSync() {
    return needSyncFlag;
}//needSynch

void disableSync() {
    needSyncFlag = false;
}

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }//if (output_buffer == NULL) { ESP_LOGE...
                    }//if (output_buffer == NULL)
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }//if (evt->user_data)
                output_len += evt->data_len;
            }//if (!esp_http_client_is_chunked_response(evt->client))

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }//if (output_buffer != NULL)
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }//if (output_buffer != NULL)
            output_len = 0;
            break;
    }//switch(evt->event_id)
    return ESP_OK;
}//http_event_handler

esp_err_t dataUpdate() {
    gpio_reset_pin((gpio_num_t)SENSOR_PIN);
    gpio_set_direction((gpio_num_t)SENSOR_PIN, GPIO_MODE_INPUT);
    data_buffer_t hallData = {
        .motion = 99
    };
    
    hallData.motion = gpio_get_level((gpio_num_t)SENSOR_PIN);
    
    time(&epoch);
    printf("epoch %ld, motion %d\r\n", (long)epoch, hallData.motion);

    hallData.timestamp = epoch;

    printf("cbuff write ret: %d\r\n", circularBufferWrite(&cBuffer, hallData.buffer));
    //circularBufferWrite(&cBuffer, hallData.buffer);

    return ESP_OK;
}//dataUpdate

esp_err_t dataAdvertisement() {
    cJSON *root;
	root = cJSON_CreateObject();

    cJSON *data[bootcount - lastAdv];
    for(uint8_t i = 0; i < bootcount - lastAdv; i++){
        data_buffer_t advData;
        cJSON *event[1];
        cJSON *array;

        array = Create_array_of_anything(event, 1);

        //flashBufferRead(cBuffer, &advData.buffer[0]);
        circularBufferRead(&cBuffer, advData.buffer);
        printf("motion: %d\r\n", advData.buffer[4]);

        event[0] = cJSON_CreateObject();

        cJSON_AddStringToObject(event[0], "Name", "motion");
        cJSON_AddNumberToObject(event[0], "Value", advData.motion);
        
        data[i] = cJSON_CreateObject();

        cJSON_AddStringToObject(data[i], "LoggerName", "Hall");
        cJSON_AddNumberToObject(data[i], "Timestamp", advData.timestamp);

        array = Create_array_of_anything(event, 1);

        cJSON_AddItemToObject(data[i], "MeasuredData", array);
        cJSON_AddArrayToObject(data[i], "ServiceData");
        cJSON_AddArrayToObject(data[i], "DebugData");

        cJSON_AddStringToObject(data[i], "DeviceId", myId);
    }//for(uint8_t i = 0; i < bootcount - lastAdv; i++)

    //cJSON *root;
    root = Create_array_of_anything(data, bootcount - lastAdv);

    //cJSON_AddItemToObject(root, "Data", array);
    const char *my_json_string = cJSON_Print(root);
	//ESP_LOGI(TAG, "my_json_string\n%s", my_json_string);
    printf("\r\n%s\r\n", my_json_string);
    cJSON_Delete(root);

    esp_http_client_config_t config = {
        .url = POST_ENDPOINT,
        .event_handler = http_event_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    char jwt[500] = "Bearer ";
    size_t jlen = strlen(jwt);
    auth.createJWT((uint8_t*)jwt + jlen, sizeof(jwt) - jlen, &jlen, (long)epoch);

    printf("jwt:\r\n%s\r\n", jwt);

    esp_http_client_set_header(client, "Authorization", jwt);

    esp_http_client_set_post_field(client, my_json_string, strlen(my_json_string));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
    ESP_LOGI(TAG, "Status = %d, content_length = %d",
           esp_http_client_get_status_code(client),
           esp_http_client_get_content_length(client));
    }//if (err == ESP_OK)
    esp_http_client_cleanup(client);

    lastAdv = bootcount;
    return err;
}//data advertisement

void goToSleep() {
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_timer_wakeup(ADV_INTERVAL * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}

void initBuffer(){
    circularBufferInit(dataBuffer, sizeof(dataBuffer), &cBuffer);
}//buffer init

esp_err_t initButton() {
    esp_err_t ret = touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_pad_config(TOUCHPAD, TOUCH_THRESHOLD);

    esp_sleep_enable_touchpad_wakeup();

    return ret;
}//initButton

void initLed(void) {
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

void blink(void) {
    for (uint8_t i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, true);
        vTaskDelay(100 / portTICK_RATE_MS);
        gpio_set_level(LED_PIN, false);
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}