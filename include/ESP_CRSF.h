#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include <string.h>
#include "freertos/queue.h"
#include "freertos/semphr.h"



#define TAG "CRSF"

typedef struct
{
    uint8_t uart_num;
    uint8_t tx_pin;
    uint8_t rx_pin;
} crsf_config_t;


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


typedef struct __attribute__((packed))
{
    unsigned voltage : 16;  // V * 10 big endian
    unsigned current : 16;  // A * 10 big endian
    unsigned capacity : 24; // mah big endian
    unsigned remaining : 8; // %
} crsf_battery_t;

typedef enum
{
    CRSF_TYPE_CHANNELS = 22,
    CRSF_TYPE_BATTERY = 8
} crsf_type_t;

typedef enum
{
    CRSF_DEST_FC = 0xC8,
    CRSF_DEST_RADIO = 0xEA
} crsf_dest_t;

void CRSF_init(crsf_config_t *config);
void CRSF_receive_channels(crsf_channels_t *channels);
void CRSF_send(crsf_dest_t dest, crsf_type_t type, const void* payload, uint8_t payload_length);