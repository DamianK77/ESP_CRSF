#include <stdio.h>
#include "ESP_CRSF.h"

#define RX_BUF_SIZE 1024    //UART buffer size

static int uart_num = 1;
static QueueHandle_t uart_queue;
crsf_channels_t channels;

static void rx_task(void *arg)
{
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(RX_BUF_SIZE);
    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, RX_BUF_SIZE);
            if (event.type == UART_DATA ) {
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(uart_num, dtmp, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "Data: %s", dtmp);
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
    //uart_enable_rx_intr(uart_num);
    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(uart_num, RX_BUF_SIZE, RX_BUF_SIZE, 10, &uart_queue, 0));

    //create task
    xTaskCreate(rx_task, "uart_rx_task", 1024*4, NULL, configMAX_PRIORITIES-1, NULL);
    
}

//receive uart data frame
void CRSF_receive(crsf_channels_t *channels)
{

    //read first 3 bytes to check destination length and type
    uint8_t header[3];
    uart_flush_input(uart_num);
    uart_flush(uart_num);
    uart_read_bytes(uart_num, header, 3, 100 / portTICK_PERIOD_MS);

    //extract length and type
    uint8_t type = header[2];
    uint8_t length = header[1];
    uint8_t dest = header[0];

    //read the rest of the frame
    uint8_t payload_length = length - 2;
    uint8_t payload[payload_length];

    uart_read_bytes(uart_num, payload, payload_length, 100 / portTICK_PERIOD_MS);

    
    //print payload
    if (type == 22) {
        // printf("Correct type\n");

        crsf_channels_t *data = (crsf_channels_t*)payload;
        *channels = *data;

    }

    //read CRC
    uint8_t crc;
    uart_read_bytes(uart_num, &crc, 1, 100 / portTICK_PERIOD_MS);
    // printf("CRC: %d\n", crc);

    

}


