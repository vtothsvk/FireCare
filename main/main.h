#ifndef SRC_MAIN_H
#define SRC_MAIN_H

//Standard Libraries
#include <string.h>
#include <time.h>
#include <sys/time.h>

//ESP-IDF components
//System
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sleep.h"
//FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
//Drivers
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"
#include "driver/touch_pad.h"
//WiFi & HTTP client
#include "esp_wifi.h"
#include "esp_http_client.h"

//Project components
#include "esp_json.h"
#include "circularBuffer.h"
#include "wifiManager.h"
#include "newAuth.h"

//apCare components
#include "apCare.h"

#endif//SRC_MAIN_H