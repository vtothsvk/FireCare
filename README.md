# AP-NURSE Care Firebeetle

AP-NURSE Care FW for the ESP32 FireBeetle devkit based on the ESP-IDF SDK

# SDK setup

Project based on the [ESP-IDF SKD](https://github.com/espressif/esp-idf).

* [Environment setup](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation-step-by-step)

# Building and flashing

ESP-IDF uses a CMake-based build system.
Namely:

* CMake to configure project and its components
* A command line build tool (either Ninja build or GNU Make)
* The espressif comand line tool, [esptool.py](https://github.com/espressif/esptool/#readme), for flashing the target

ESP-IDF provides its own comand line tool front-end tool to manage these stages of project development - idf.py

More info: [ESP-IDF build system](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html)

## Building using cmake and ninja

1. Configure using cmake

```
mkdir build
cd build
cmake .. -DADV_INTERVAL=300 -DSENSOR_PIN=39 -G Ninja
```

available settigs:

* switch **PROV_BLE** changes wifi provisioning to BLE instead of a WiFi AP
* variable **ADV_INTERVAL** sets data advertisement interval (in seconds)
* variable **SENSOR_PIN** sets up Hall sensor pin (usable pins 32-39)

2. Enable the "Bluetooth" component of the ESP-IDF using menuconfig

```
ninja menuconfig
```

* enale bluetooth in **Component config > Bluetooth**

3. Build the project (can be skipped)

```
ninja
```

4. Flash the project using `flash` (automatically builds the poroject as well)

```
ninja flash
```

## Building using idf.py



1. Select target using `set-target`

```
idf.py set-target esp32
```

2. Enable the "Bluetooth" component of the ESP-IDF using menuconfig

```
idf.py menuconfig
```

* enale bluetooth in **Component config > Bluetooth**


3. Build the project using `build` (can be skipped)

```
idf.py build
```

4. Flash the project using `flash` (automatically builds the poroject as well)

*optionally u can specify the target port using -p*
```
idf.py flash -p /dev/ttyS0
```
