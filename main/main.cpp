#include "main.h"

extern "C" {
    void app_main(void);
}

void app_main(void) {    
    boot();
    initLed();
    blink();

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