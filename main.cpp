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


#include "ventilation.h"
#include "i2c_display.h"
#include "interupt_handler.h"
#include "tftdisplay.h"
//#include "MqttWifiManager.h"

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
#define STOP_BITS 1 // for simulator
//#define STOP_BITS 2 // for real system

//#define USE_MODBUS
#define USE_MQTT
//#define USE_SSD1306


#ifdef USE_SSD1306
static const uint8_t raspberry26x32[] =
        {0x0, 0x0, 0xe, 0x7e, 0xfe, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xfc, 0xfe,
         0xfe, 0xff, 0xff,0xff, 0xff, 0xff, 0xfe, 0x7e,
         0x1e, 0x0, 0x0, 0x0, 0x80, 0xe0, 0xf8, 0xfd,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd,
         0xf8, 0xe0, 0x80, 0x0, 0x0, 0x1e, 0x7f, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
         0xff, 0xff, 0xff, 0xff, 0x7f, 0x1e, 0x0, 0x0,
         0x0, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x3f, 0x3f,
         0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f,
         0x3f, 0x1f, 0x1f, 0xf, 0x7, 0x3, 0x0, 0x0 };
#endif
static const uint8_t wifi_signal[] =
        {// font edit begin : monovlsb : 10 : 7
                0x08, 0x04, 0x12, 0x09, 0x65, 0x65, 0x09, 0x12,
                0x04, 0x08
// font edit end
        };

static const uint8_t wifi_signal_broken[] =
        {
// font edit begin : monovlsb : 10 : 7
                0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00,
                0x00, 0x00
// font edit end
        };



static const char *topicSUB = "controller/settings";
static const char *topicPUB = "controller/status";

int main() {

    uint8_t rotA = 10;
    uint8_t rotB = 11;
    uint8_t rotP = 12; //
    uint8_t SW0 = 9;
    uint8_t  SW1 = 8;
    uint8_t  SW2 = 7;
    InterruptHandler rothandlerA(rotA);
    //InterruptHandler rothandlerB(rotB);
    InterruptHandler rothandlerP(rotP);
    InterruptHandler _button1(SW0);
    InterruptHandler _button2(SW1);
    InterruptHandler _button3(SW2);

    const uint led_pin = 22;
    const uint led_pin2 = 21;


    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(led_pin2);
    gpio_set_dir(led_pin2, GPIO_OUT);


    stdio_init_all();
    // I2C_Display tft(14, 15, i2c1);
    Ventilation vent(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE);
    vent.addSensor("fan", 1, 0);
    vent.addSensor("co2", 240, 256);
    vent.addSensor("t", 241, 257);
    vent.addSensor("rh", 241, 256);
    //vent.registers["fan"]->write(0);
    vent.writeSensor("fan", 0);
    sleep_ms(100);

    i2c_init(i2c1, 400*1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    TFTDisplay tft( 14, 15, i2c1);
    //tft.displayControlPressureLevel(50);
    //tft.displayMenu("Auto", "Manual");
    // tft.show();

/*    MqttWifiManager mqttManager("TP-Link_A2FC", "nadimahmed", "192.168.0.210", 1883);

    // Connect to WiFi
    if (mqttManager.connectWiFi()) {
        printf("Connected to WiFi\n");
    } else {
        printf("Failed to connect to WiFi\n");
    }

    // Connect to MQTT broker
    if (mqttManager.connectMQTT()) {
        printf("Connected to MQTT broker\n");
    } else {
        printf("Failed to connect to MQTT broker\n");
    }

    // Subscribe to a topic
    if (mqttManager.subscribe(topicSUB, MQTT::QOS1)) {
        printf("Subscribed to topic: %s\n", topicSUB);
    } else {
        printf("Failed to subscribe to topic: %s\n", topicSUB);
    }*/
    enum scrollmenu{
        start,
        menu,
        automode,
        menualmode,
        connect,
        read,
        write,
        display,
        send
    } scrollmenu = start;

    int count = 0;
    int count2 = 0;
    int speed = 0;
    //tft.welcomeScreen();
    uint8_t  data[1] = {0xF1};
    uint8_t  values[2] = {0};
    int pressure = 0;
    int co2 = 0;
    int rh = 0;
    int t = 0;
    int fan = 0;
    auto modbus_poll = make_timeout_time_ms(4000);
    bool autoMode = false;

    while(true) {
        switch (scrollmenu) {
            case start:
                tft.welcomeScreen();
                sleep_ms(2000);
                tft.autoOrManual();
                tft.selectmenu(1);
                scrollmenu = menu;
                break;
            case menu:
                if(rothandlerA.rotaryturned){
                    rothandlerA.rotaryturned = false;
                    if(rothandlerA.getCount() == 1){
                        autoMode = true;
                        tft.selectmenu(1);
                    } else if(rothandlerA.getCount()== -1){
                        tft.selectmenu(0);
                        autoMode = false;
                    }
                }
                if(rothandlerP.buttonPressed && autoMode){
                    rothandlerP.buttonPressed = false;
                    tft.fill(0);
                    tft.displayControlSpeed(speed);
                    scrollmenu = automode;
                } else if(rothandlerP.buttonPressed && !autoMode){
                    rothandlerP.buttonPressed = false;
                    tft.fill(0);
                    tft.displayControlSpeed(speed);
                    scrollmenu = menualmode;
                }
                break;
            case menualmode:
                if(rothandlerA.rotaryturned){
                    rothandlerA.rotaryturned = false;
                    speed+=rothandlerA.getCount();
                    if(speed > 100){
                        speed = 100;
                    } else if(speed < 0){
                        speed = 0;
                    }
                    vent.writeSensor("fan", speed*10);
                    sleep_ms(100);
                    tft.displayControlSpeed(speed);

                }
                if(rothandlerP.buttonPressed){
                    rothandlerP.buttonPressed = false;
                    tft.displayStatus(co2, t, rh, speed, pressure, wifi_signal);
                    scrollmenu = read;
                }
                break;
            case automode:
                if(rothandlerA.rotaryturned){
                    rothandlerA.rotaryturned = false;
                    speed+=rothandlerA.getCount();
                    if(speed > 100){
                        speed = 100;
                    } else if(speed < 0){
                        speed = 0;
                    }
                    vent.writeSensor("fan", speed*10);
                    sleep_ms(100);
                    tft.displayControlSpeed(speed);
                }

            case read:
                if(time_reached(modbus_poll)){
                    modbus_poll = make_timeout_time_ms(4000);
                    co2 = vent.readSensor("co2");
                    rh = vent.readSensor("rh");
                    t = vent.readSensor("t");
                    fan = vent.readSensor("fan");
                    i2c_write_blocking(i2c1, 64, data, 1, false);  // Send address
                    sleep_ms(10);
                    i2c_read_blocking(i2c1, 64, values, 2, false);  // Read values
                    sleep_ms(100);
                   tft.displayStatus(co2, t, rh, fan, pressure, wifi_signal);
                   std::cout<< "co2: " << co2 << " t: " << t << " rh: " << rh << " fan: " << fan << " pressure: " << pressure << std::endl;
                }
                if(rothandlerP.buttonPressed){
                    rothandlerP.buttonPressed = false;
                    tft.fill(0);
                    tft.autoOrManual();
                    tft.selectmenu(1);
                    scrollmenu = menu;
                }
                break;

             default:
                break;
        }
    }


    return 0;
}


/*
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
 */