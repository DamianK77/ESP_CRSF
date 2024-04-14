#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"


#define TAG "CRSF"

#define LSB3BITMASK 0b00000111

typedef struct __attribute__((packed))
{
    unsigned ch1 : 11;
    unsigned ch2 : 11;
    unsigned ch3 : 11;
    unsigned ch4 : 11;
    unsigned ch5 : 11;
    unsigned ch6 : 11;
    unsigned ch7 : 11;
    unsigned ch8 : 11;
    unsigned ch9 : 11;
    unsigned ch10 : 11;
    unsigned ch11 : 11;
    unsigned ch12 : 11;
    unsigned ch13 : 11;
    unsigned ch14 : 11;
    unsigned ch15 : 11;
    unsigned ch16 : 11;
} crsf_channels_t;

typedef struct
{
    uint8_t uart_num;
    uint8_t tx_pin;
    uint8_t rx_pin;
} crsf_config_t;

void CRSF_init(crsf_config_t *config);
void CRSF_receive(crsf_channels_t *channels);