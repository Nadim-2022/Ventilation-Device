#include <stdio.h>
#include <string.h>
#include <cmath>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
//#include "ssd1306.h"
#include "interupt_handler.h"
#include "i2c_display.h"

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#if 0
#define UART_NR 0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#else
#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#endif

#define BAUD_RATE 9600

#define USE_MODBUS

#define USE_MQTT
#define USE_SSD1306


#ifdef USE_SSD1306
static const uint8_t wifi_signal[] =
        {// font edit begin : monovlsb : 20 : 20
        0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x60, 0x60,
        0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0xC0, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x03,
        0x31, 0x18, 0x8C, 0xC6, 0x66, 0x66, 0x66, 0x66,
        0xC6, 0x8C, 0x18, 0x31, 0x03, 0x06, 0x0C, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x06, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
// font edit end
        };

#endif

int main(){
    stdio_init_all();
    InterruptHandler handler(10, 12);
    I2C_Display tft(14, 15, i2c1);
    int count = 0;
    tft.displayControlPressureLevel(56);
    //tft.displayText("Hello World", wifi_signal);
    //tft.displayStatus(85,33,77,88,120);
    while (true) {
        if (InterruptHandler::count != count) {
            count = InterruptHandler::count;
            printf("Interrupt count: %d\n", count);
        }
        tft.displayControlPressureLevel(count);
    }
    return 0;

}