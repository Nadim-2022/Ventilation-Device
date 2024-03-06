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
    i2c_init(i2c1, 100000);
    InterruptHandler handler(10, 12);
    I2C_Display display(14, 15, i2c1);
    //display.displayText("Hello World", raspberry26x32);
    //display.show();
    auto uart{std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE)};
    auto rtu_client{std::make_shared<ModbusClient>(uart)};
    ModbusRegister rh(rtu_client, 241, 256);
    ModbusRegister tm(rtu_client, 241, 257);
    ModbusRegister co2(rtu_client, 240, 256);
    auto modbus_poll = make_timeout_time_ms(3000);
    ModbusRegister produal(rtu_client, 1, 0);
    produal.write(0);
    sleep_ms((100));
    produal.write(0);

    int pressure = 0;
    int speed = 0;
    uint8_t  data[1] = {0xF1};
    uint8_t  values[2] = {0};
    uint16_t  show = 0;

    int count = 0;
    //int last_count = handler.count;
    while (true) {
        if(pressure !=handler.count){
            std::cout << "Last count: " << handler.count << std::endl;
           i2c_write_blocking(i2c1, 64, data, 1, false);  // Send address
            sleep_ms(10);
            std::cout << "It's my life " << handler.count << std::endl;
            i2c_read_blocking(i2c1, 64, values, 2, false);  // Read values
            sleep_ms(100);
            pressure =handler.count;
            produal.write(pressure*10);
            sleep_ms((100));
            show =( (values[0] << 8) | values[1]) /240;
            std::cout << "Pressure: " << show << std::endl;
        }
        //tight_loop_contents();
    }
    return 0;

}