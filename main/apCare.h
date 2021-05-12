#ifndef AP_CARE_H
#define AP_CARE_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_NUM_TO_MASK(PIN_NUM) ((uint64_t)((1 << (PIN_NUM - 31))) << 31)

#define uS_TO_S_FACTOR 1000000

//#ifndef ADV_INTERVAL
#define ADV_INTERVAL 300
//#endif
#define HTTP_HEADER_AUTHORISATION   "Authorization"
#define POST_ENDPOINT               "https://192.168.2.6:51415/api/v1/Auth"

#define BUFFER_SIZE     500

#define SDA_GPIO GPIO_NUM_21
#define SCL_GPIO GPIO_NUM_22
#define PORT 0
#define ADDR BME680_I2C_ADDR_1

#define TOUCH_THRESH_NO_USE 0
#define TOUCH_THRESHOLD     1000
#define TOUCHPAD            TOUCH_PAD_NUM7

//#ifndef SENSOR_PIN
#define SENSOR_PIN 39
//#endif

#define BUTTON_PIN_BITMASK 0x8000000000

#define LED_PIN GPIO_NUM_2

#define ADC_GPIO ADC1_CHANNEL_0

typedef struct data_buffer{
    union{
        struct{
            unsigned long timestamp;
            uint8_t motion;
        };
        uint8_t buffer[5];
    };
}data_buffer_t;

void boot(void);
bool firstBoot(void);
bool needSync(void);
bool needAdvertisement(void);
void disableSync(void);

esp_err_t loggerSync(void);
esp_err_t dataUpdate(void);
esp_err_t http_event_handler(esp_http_client_event_t *evt);
esp_err_t dataAdvertisement(void);
void goToSleep(void);
void initBuffer(void);
void sensorInit(void);

esp_err_t batteryInit(void);
int batteryRead(void);
esp_err_t initButton(void);

void initLed(void);
void blink(void);

#ifdef __cplusplus
}
#endif

#endif//AP_CARE_H