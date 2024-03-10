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
#include "ssd1306.h"


#include "ventilation.h"
#include "i2c_display.h"
#include "interupt_handler.h"

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

#include "json.h"
//char mqtt_payload[100];
/*
void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;
    char payload_str[message.payloadlen + 1];
    memcpy(payload_str, message.payload, message.payloadlen);
    payload_str[message.payloadlen] = '\0';
    memcpy(mqtt_payload, payload_str, message.payloadlen);

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n",payload_str);
}
 */

int sami = 0;

char mqtt_payload[256]; // Assuming a maximum payload size of 256 characters
bool automatic = false;
void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;

    // Copy the payload into a null-terminated string
    char payload_str[message.payloadlen + 1];
    memcpy(payload_str, message.payload, message.payloadlen);
    payload_str[message.payloadlen] = '\0';

    // Print MQTT message details
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload: %s\n", payload_str);

    // check for true or false
    if (strstr(payload_str, "true") != nullptr) {
        automatic = true;
        printf("Automatic: true\n");
    } else if (strstr(payload_str, "false") != nullptr) {
        automatic = false;
        printf("Automatic: false\n");
    } else {
        printf("Payload does not contain 'true' or 'false'\n");
    }

    // Search for "pressure" in the payload
    const char* pressure_key = "\"pressure\":";
    char* pressure_pos = strstr(payload_str, pressure_key);

    if (pressure_pos != nullptr) {
        // Extract the value of "pressure"
        int pressure;
        sscanf(pressure_pos + strlen(pressure_key), "%d", &pressure);
        printf("Pressure: %d\n", pressure);
        sami = pressure;
    } else {
        printf("Payload does not contain 'pressure' field\n");
    }
}


static const char *topicSUB = "controller/settings";
static const char *topicPUB = "controller/status";

int main() {
    const uint led_pin = 22;
    const uint button = 9;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);

    stdio_init_all();

    printf("\nBoot\n");
    I2C_Display tft(14, 15, i2c1);
    auto uart{std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE)};
    auto rtu_client{std::make_shared<ModbusClient>(uart)};
    ModbusRegister relativeHumidity(rtu_client, 241, 256);
    ModbusRegister temperature(rtu_client, 241, 257);
    ModbusRegister co2(rtu_client, 240, 256);
    ModbusRegister fan(rtu_client, 1, 0);
    auto modbus_poll = make_timeout_time_ms(1000);
    sleep_ms((100));
    fan.write(0);
    sleep_ms((100));

    uint8_t  cmd[1] = {0xF1};
    uint8_t  values[2] = {0};
    uint16_t  show = 0;

#ifdef USE_MQTT
    //Connect to the wifi and mqtt
    // IPStack ipstack("Nadim", "nadimahmed");
    IPStack ipstack("TP-Link_A2FC", "nadimahmed"); // example
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);

    int rc = ipstack.connect("192.168.0.210", 1883);
    if (rc != 1) {
        printf("rc from TCP connect is %d\n", rc);
    }

    printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *) "PicoW-sample";
    rc = client.connect(data);
    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
        while (true) {
            tight_loop_contents();
        }
    }
    printf("MQTT connected\n");

    // We subscribe QoS2. Messages sent with lower QoS will be delivered using the QoS they were sent with
    rc = client.subscribe(topicSUB, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("rc from MQTT subscribe is %d\n", rc);
    }
    printf("MQTT subscribed\n");

    auto mqtt_send = make_timeout_time_ms(2000);
    int mqtt_qos = 0;
    int msg_count = 0;
#endif
    enum{
        connect_to_wifi,
        connect_to_mqtt,
        read_modbus,
        display_status,
        send_mqtt,
    } condition = read_modbus;
    InterruptHandler handler(10, 12);
    int pressure = 0;
    /*
     *{
"nr": 96,
"speed": 18,
"setpoint": 18,
"pressure": 5,
"auto": false,
"error": false,
"co2": 300,
"rh": 37,
"temp": 20
}
     */
    int nr = 0;
    int speed = 0;
    int setpoint = 0;
    int pressure1 = 0;
    bool auto1 = false;
    bool error = false;
    int co22 = 0;
    int rh = 0;
    int temp = 0;


    while (true){

        if (pressure != handler.count) {
            pressure = handler.count;
            if (pressure >= 100) {
                pressure = 100;
                handler.count = 100;
            }else if (pressure <=0) {
                pressure = 0;
                handler.count = 0;
            }
            fan.write(pressure * 10);
            sleep_ms(100);
            printf("Pressure: %d\n", pressure);
        }

        if (automatic) {
            fan.write(sami * 10);
            printf("Pressure: %d\n", sami);
            automatic = false;
            sleep_ms(400);
        }
        switch (condition) {
            case connect_to_wifi:
                // do something
                break;
            case connect_to_mqtt:
                // do something else
                break;
            case read_modbus:
                if (time_reached(modbus_poll)) {
                    gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
                    modbus_poll = delayed_by_ms(modbus_poll, 3000);
                    i2c_write_blocking(i2c1, 64, cmd, 1, false);  // Send address
                    sleep_ms(10);
                    i2c_read_blocking(i2c1, 64, values, 2, false);  // Read values
                    sleep_ms(10);
                    show =( (values[0] << 8) | values[1]) /240;
                    nr ++;
                    speed = fan.read()/10;
                    setpoint = sami;
                    pressure1 = show;
                    auto1 = automatic;
                    error = false;
                    co22 = co2.read();
                    rh = relativeHumidity.read();
                    temp = temperature.read();
                    condition = display_status;
                }
                break;
            case display_status:
                // do something else

                tft.displayStatus(co2.read(), temperature.read()/10, relativeHumidity.read()/10, fan.read()/10, show, wifi_signal);
                printf("CO2: %d ppm, Temperature: %d C, Humidity: %d %%RH, Pressure: %d Pa, Fan Speed: %d %%\n",co2.read(), temperature.read()/10, relativeHumidity.read()/10, show, fan.read()/10);
                condition = send_mqtt;
                break;
            case send_mqtt:
                // do something else
                // Create a JSON object
                /*if (time_reached(mqtt_send)) {
                    mqtt_send = delayed_by_ms(mqtt_send, 2000);
                    if (!client.isConnected()) {
                        printf("Not connected...\n");
                        rc = client.connect(data);
                        if (rc != 0) {
                            printf("rc from MQTT connect is %d\n", rc);
                        }
                    }*/
                char buf[350];
                sprintf(buf, "My name is khan and  i am not a terrorist , what is your name . ");
                // Publish the JSON string to the MQTT topic
                //char buf[256] =  R"({"nr": +nr+"speed": %d,"setpoint": %d,"pressure": %d,"auto": %s,"error": %s,"co2": %d,"rh": %d,"temp": %d})";
                //char buf[256] = "HEllo world";
                int rc = 0;
                MQTT::Message message;

                message.retained = false;
                message.dup = false;
                message.payload = (void *) buf;

                //sprintf(buf, "Msg nr: %d QoS 0 message", ++msg_count);
                printf("%s\n", buf);
                message.qos = MQTT::QOS2;
                message.payloadlen = strlen(buf) + 1;
                rc = client.publish(topicPUB, message);
                printf("Publish rc=%d\n", rc);
                condition = read_modbus;
                // }
                break;

        }

        cyw43_arch_poll(); // obsolete? - see below
        client.yield(100); // socket that client uses calls cyw43_arch_poll()

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
