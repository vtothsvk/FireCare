#include "main.h"

extern "C" {
    void app_main(void);
}

char devName[16];

void app_main(void) {
    boot();
    initLed();
    blink();

    generateName(devName);
    printf("%s\r\n", devName);

    wifiInit();
    if (firstBoot()){
        wifiProvisioning();
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