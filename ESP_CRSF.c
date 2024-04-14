#include <stdio.h>
#include "ESP_CRSF.h"

static int uart_num = 1;

#define MSBpos 1
#define LSBpos 0

uint8_t generate_mask(uint8_t number_of_ones, uint8_t position)
{
    if (position == MSBpos) {
        return (uint8_t)(0xFF << (8 - number_of_ones));
    } else if (position == LSBpos) {
        return (uint8_t)((1 << number_of_ones) - 1);
    }
    return 0;

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

    //uart_read_bytes(uart_num, payload, payload_length, 100 / portTICK_PERIOD_MS);
    uart_read_bytes(uart_num, payload, payload_length, 100 / portTICK_PERIOD_MS);

    
    uint16_t channel_values[16]; // Array to hold 16 11-bit numbers
    //print payload
    if (type == 22) {
        printf("Correct type\n");

        crsf_channels_t *channels = (crsf_channels_t *)payload;
        printf("Channel 0: %d\n", channels->ch0);
        printf("Channel 1: %d\n", channels->ch1);
        printf("Channel 2: %d\n", channels->ch2);
        printf("Channel 3: %d\n", channels->ch3);
        printf("\n");

    }

    //read CRC
    uint8_t crc;
    uart_read_bytes(uart_num, &crc, 1, 100 / portTICK_PERIOD_MS);
    

}


