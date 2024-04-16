# ESP-IDF component Crossfire (CRSF) protocol decoder for Crossfire and ELRS receivers

This component connects to an RX module via UART and decodes incoming data based on the CRSF protocol. Currently in early development.

## Functions
- Reading data from channels 1-16
- more (telemetry, different data types) to be added

## How to use
First you need to call `CRSF_init` in which you have to specify rx and tx pins on ESP32 and an uart controller to be used to communicate with the RX module (default is `UART_NUM_1`). This should be done by passing a `crsf_config_t` type structure. Then, in order to get the channel values, call `CRSF_receive_channels` with an address to a `crsf_channels_t` type structure in which the data is meant to be saved.

## Usage example
```
crsf_config_t config = {
    .uart_num = UART_NUM_1,
    .tx_pin = 17,
    .rx_pin = 16
};

CRSF_init(&config);

crsf_channels_t channels = {0};
while (1)
{
    
    CRSF_receive_channels(&channels);
    printf(">Channel 1: %d\n", channels.ch1); //reads throttle value
}
```
