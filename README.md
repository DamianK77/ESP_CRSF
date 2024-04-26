# ESP-IDF component Crossfire (CRSF) protocol decoder for Crossfire and ELRS receivers

This component connects to an RX module via UART and decodes incoming data based on the CRSF protocol. Currently in early development.

This repo was heavily influenced by a repo by AlfredoSystems (https://github.com/AlfredoSystems/AlfredoCRSF). Thanks!

## Functions
- Reading data from channels 1-16
- Sending battery data back to transmitter
- more (telemetry, different data types) to be added

## How to use
First you need to call `CRSF_init` in which you have to specify rx and tx pins on ESP32 and an uart controller to be used to communicate with the RX module (default is `UART_NUM_1`). This should be done by passing a `crsf_config_t` type structure. Then, in order to get the channel values, call `CRSF_receive_channels` with an address to a `crsf_channels_t` type structure in which the data is meant to be saved.

To send telemetry you should use the `CRSF_send` function with attributes corresponding to the type of message you want to send and an appriopriate data structure and length. For some reason, if you want to send telemetry to the radio you still need to use the `CRSF_DEST_FC` destination flag.

## Usage example
```
crsf_config_t config = {
    .uart_num = UART_NUM_1,
    .tx_pin = 17,
    .rx_pin = 16
};
CRSF_init(&config);

crsf_channels_t channels = {0};
crsf_battery_t battery = {0};
crsf_gps_t gps = {0};
while (1)
{
    
    CRSF_receive_channels(&channels);
    printf(">Channel 1: %d\n", channels.ch1);

    battery.voltage = 120; //voltage 10*V
    battery.current = 100; //current 10*A
    battery.capacity = 1000; //capacity
    battery.remaining = 50; //remaining % of battery

    CRSF_send_battery_data(CRSF_DEST_FC, &battery);

    gps.latitude = 42.4242 * 10000000; //42.4242 deg
    gps.longitude = 56.5656 * 10000000; //56.5656 deg
    gps.altitude = 5 + 1000; //5m
    gps.groundspeed = 420; //42km/h
    gps.heading = 90 * 100; //90 deg NOT WORKING WELL NOW
    gps.satellites = 8; //8 satellites

    CRSF_send_gps_data(CRSF_DEST_FC, &gps);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
```
