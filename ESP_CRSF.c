#include <stdio.h>
#include "ESP_CRSF.h"

#define RX_BUF_SIZE 1024    //UART buffer size


SemaphoreHandle_t xMutex;

static int uart_num = 1;
static QueueHandle_t uart_queue;
crsf_channels_t received_channels = {0};

static void rx_task(void *arg)
{
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(RX_BUF_SIZE);
    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, RX_BUF_SIZE);
            if (event.type == UART_DATA ) {
                //ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(uart_num, dtmp, event.size, portMAX_DELAY);

                //extract length and type
                uint8_t type = dtmp[2];
                uint8_t length = dtmp[1];
                uint8_t dest = dtmp[0];

                //read the rest of the frame
                uint8_t payload_length = length - 2;
                uint8_t payload[payload_length];

                for (int i = 0; i < payload_length; i++) {
                    payload[i] = dtmp[i+3];
                }

                //todo CRC

                if (type == 22) {
                    
                    xSemaphoreTake(xMutex, portMAX_DELAY);
                    received_channels = *(crsf_channels_t*)payload;
                    xSemaphoreGive(xMutex);

                    //printf(">Channel1: %d\n", received_channels.ch1);
                    // *received_channels = *data;
                }
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void CRSF_init(crsf_config_t *config)
{
    uart_num = config->uart_num;

    //begin uart communication with RX
    uart_config_t uart_config = {
        .baud_rate = 420000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
        
    };
    uart_param_config(config->uart_num, &uart_config);
    uart_set_pin(uart_num, config->tx_pin, config->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(uart_num, RX_BUF_SIZE, RX_BUF_SIZE, 10, &uart_queue, 0));

    //create semaphore
    xMutex = xSemaphoreCreateMutex();
    //create task
    xTaskCreate(rx_task, "uart_rx_task", 1024*4, NULL, configMAX_PRIORITIES-1, NULL);
    
}

//receive uart data frame
void CRSF_receive_channels(crsf_channels_t *channels)
{
    *channels = received_channels;
}


