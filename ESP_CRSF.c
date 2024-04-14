#include <stdio.h>
#include "ESP_CRSF.h"

static int uart_num = 1;

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
    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));
    
}

//receive uart data frame
void CRSF_receive(uint8_t *data)
{

    //read first 3 bytes to check destination length and type
    uint8_t header[8];
    uart_flush_input(uart_num);
    uart_read_bytes(uart_num, header, 8, 100 / portTICK_PERIOD_MS);

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
        printf("Correct type\n");
        for (int i = 0; i < payload_length; i++) {
            printf("Payload[%d]: %d\n", i, payload[i]);
        }
    }

    printf("Type: %d\n", type);

    printf("\n");

    //read CRC
    uint8_t crc;
    uart_read_bytes(uart_num, &crc, 1, 100 / portTICK_PERIOD_MS);
    

}


