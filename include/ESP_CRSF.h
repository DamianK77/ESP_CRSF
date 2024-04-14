#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"


#define TAG "CRSF"

typedef struct
{
    uint8_t uart_num;
    uint8_t tx_pin;
    uint8_t rx_pin;
} crsf_config_t;

void CRSF_init(crsf_config_t *config);
void CRSF_receive(uint8_t *data);