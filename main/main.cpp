#include "main.h"

extern "C" {
    void app_main(void);
}

void app_main(void) {
    boot();
    
    uint64_t pin = PIN_NUM_TO_MASK(SENSOR_PIN);
    printf("pin num: %d", SENSOR_PIN);
    printf("\r\npin mask: %lld\r\n\r\n", pin);

    vTaskDelay(20000 / portTICK_RATE_MS);

    if (firstBoot()) {
        //wifiProvisioning();
        initBuffer();
    }//if (firstBoot())

    if (needSync()) {
        if (wifiConnect() == ESP_OK) {
            syncTime();
            disableSync();
            initButton();
            goToSleep();
        }//if (wifiConnect() == ESP_OK)
    }//if (needSync())

    dataUpdate();

    if (wifiConnect() == ESP_OK) {
        dataAdvertisement();
    }//if (wifiConnect() == ESP_OK)

    initButton();
    goToSleep();
}//app_main