#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
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
#include "MqttWifiManager.h"

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
//#define USE_MQTT
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
    //i2c_init(i2c1, 100*1000);
    i2c_init(i2c1, 400*1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    TFTDisplay tft( 14, 15, i2c1);
    //tft.displayControlPressureLevel(50);
    //tft.displayMenu("Auto", "Manual");
    // tft.show();

    // MqttWifiManager mqttManager("SmartIotMQTT", "SmartIot", "192.168.1.142", 1883);

/*
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

    enum key{
        up,
        down,
        left,
        right,
        enter
    } key = up;

    int MainMenucount = 0;
    int SubMenucount = 0;
    int speed = 0;
    //tft.welcomeScreen();
    uint8_t  data[1] = {0xF1};
    uint8_t  values[2] = {0};
    int sensorPressure = 0;
    int co2 = 0;
    int rh = 0;
    int t = 0;
    int fan = 0;
    auto modbus_poll = make_timeout_time_ms(4000);
    auto pressure_poll = make_timeout_time_ms(1000);
    bool autoMode = false;
    int setPressure = 0;
    int currentPressure = 0;
    int tempPressure = 0;
    char buf[256];
    int msg_count=0;
    bool error = false;
    int MqttValue = 0;
    auto mqttSendTime = make_timeout_time_ms(2000);
    int scroll = 0;
    // Arry of capital letters
    std::array<char, 26> alphabet = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    std::array<char, 26> lowercase_alphabet = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    std::array<char, 36> special_chars_numbers = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
                                                  '-', '_', '=', '+', '[', ']', '{', '}', ';', ':',
                                                  '\'', '\"', ',', '.', '<', '>'};
    std::vector<char> ssid;
    std::vector<char> password;
    std::vector<char> ip;
    std::vector<char> port;
    std:: string SSID;
    char alepa[26];
    int smallchar = 97;
    int smallcharend = 122;
    int printSpy = 0;
    int positionX = 6;
    enum keyboard{
        capital = 0,
        small = 1,
        number = 2
    } keyboard = capital;
    int maxScroll = 0;
    char ssid2[25];
    int wifimenucount = 0;
    enum scrollmenu{
        start,
        menu,
        setpressureMode,
        /*automode,*/
        setspeedMode,
        controlautomode,
        /*manualmode,*/
        connect,
        /* read,*/
        statusMode,
        writeMode,
        /*display,*/
        MQTTsend,
        connectTowifi,
        enterCredentials,
        enterIDpass,
        CredentialsMenu
    } scrollmenu = start;
    while(true) {

        /*   switch (key) {
               case up:
                   tft.displayKeyboard(0);
                   maxScroll = alphabet.size() - 1;
                   key = down;
                   break;
               case down:
                   if (_button3.buttonPressed) {
                       _button3.buttonPressed = false;
                       if (keyboard == capital) {
                           maxScroll = lowercase_alphabet.size() - 1;
                           keyboard = small;
                       } else if (keyboard == small) {
                           maxScroll = special_chars_numbers.size() - 1;
                           keyboard = number;
                       } else {
                           keyboard = capital;
                       }
                       tft.displayKeyboard(keyboard);
                   }
                   if (rothandlerA.rotaryturned) {
                       rothandlerA.rotaryturned = false;
                       scroll += rothandlerA.getCount();
                       if (scroll > maxScroll) {
                           scroll = 0;
                       } else if (scroll < 0) {
                           scroll = maxScroll;
                       }
                       tft.selectChar(scroll);
                   }
                   if (keyboard == capital && scroll >= alphabet.size()) {
                       scroll = alphabet.size() - 1;
                   } else if (keyboard == small && scroll >= lowercase_alphabet.size()) {
                       scroll = lowercase_alphabet.size() - 1;
                   } else if (keyboard == number && scroll >= special_chars_numbers.size()) {
                       scroll = special_chars_numbers.size() - 1;
                   }
                   tft.selectChar(scroll);
                   if (rothandlerP.buttonPressed) {
                       rothandlerP.buttonPressed = false;
                       if (keyboard == capital) {
                           ssid.push_back(alphabet[scroll]);
                       } else if (keyboard == small) {
                           ssid.push_back(lowercase_alphabet[scroll]);
                       } else {
                           ssid.push_back(special_chars_numbers[scroll]);
                       }
                       // ssid.push_back(alphabet[scroll]);
                       //tft.displaychar(alphabet[scroll][0], positionX,52);
                       SSID = std::string(ssid.begin(), ssid.end());
                       tft.displayString(SSID);
                       std::cout << SSID << std::endl;
                       positionX += 18;

                   }

                   if (_button1.buttonPressed) {
                       _button1.buttonPressed = false;
                       if (!SSID.empty()) {
                           SSID = std::string(ssid.begin(), ssid.end());
                           //std::cout << SSID << std::endl;
                           std::copy(ssid.begin(), ssid.end(), ssid2);
                           ssid2[SSID.size()] = '\0';
                           printf("SSID: %s\n", ssid2);
                           //ssid.clear();
                           key = left;
                       }
                   }
                   if (_button2.buttonPressed) {
                       _button2.buttonPressed = false;
                       if (!ssid.empty()) {
                           ssid.pop_back();
                           positionX -= 18;
                           //SSID = std::string(ssid.begin(), ssid.end());
                           //tft.displaychar(' ', positionX, 52);
                           tft.displayString(SSID);
                       }

                   }

                   break;
               case left: {
                   MqttWifiManager mqttManager(ssid2, "nadimahmed", "192.168.1.142", 1883);
                   if (mqttManager.connectWiFi()) {
                       printf("Connected to WiFi\n");
                   } else {
                       printf("Failed to connect to WiFi\n");
                   }

               }
                   break;
               case right:
                   tft.displayKeyboard(3);
                   break;
               case enter:
                   tft.displayKeyboard(4);
                   break;


           }*/
        /*if(mqttManager.isNotification()){
            mqttManager.setNotification(false);
            std::cout << "Automatic mode" << std::endl;
            autoMode = mqttManager.isAutomatic();
            if (autoMode) {
                setPressure = mqttManager.getvalue();
                scrollmenu = controlautomode;
            } else {
                speed = mqttManager.getvalue();
                vent.writeSensor("fan", speed*10);
                scrollmenu = read;
            }
            tft.displayStatus(co2, t, rh, fan, pressure, wifi_signal);
        }*/
        switch (scrollmenu) {
            case start: // start screen
                tft.welcomeScreen();
                sleep_ms(1000);
                tft.mainMenu();
                tft.selectmenu(0);
                scrollmenu = menu;
                break;
            case menu: // main menu
                if(rothandlerA.rotaryturned){
                    rothandlerA.rotaryturned = false;
                    MainMenucount+=rothandlerA.getCount();
                    if(MainMenucount > 3){
                        MainMenucount = 0;
                    } else if(MainMenucount < 0){
                        MainMenucount = 3;
                    }
                    std::cout << MainMenucount << std::endl;
                    tft.selectmenu(MainMenucount);
                }
                if(MainMenucount == 0 && rothandlerP.buttonPressed){
                    rothandlerP.buttonPressed = false;
                    tft.displayControlSpeed(speed);
                    scrollmenu = setspeedMode;
                } else if(MainMenucount == 1 && rothandlerP.buttonPressed){
                    rothandlerP.buttonPressed = false;
                    tft.displayControlPressureLevel(setPressure);
                    scrollmenu = setpressureMode;
                    rothandlerP.buttonPressed = false;
                } else if(MainMenucount == 2 && rothandlerP.buttonPressed){
                    rothandlerP.buttonPressed = false;
                    tft.connectTowifi();
                    tft.selectWifimenu(0);
                    scrollmenu = connect;
                } else if(MainMenucount == 3 && rothandlerP.buttonPressed){
                    rothandlerP.buttonPressed = false;
                    scrollmenu = statusMode;
                }
                break;
            case setspeedMode: // manual mode for setting fan speed
                if(rothandlerA.rotaryturned){
                    rothandlerA.rotaryturned = false;
                    speed+=rothandlerA.getCount();
                    if(speed >= 100){
                        speed = 100;
                    } else if(speed <= 0){
                        speed = 0;
                    }
                    vent.writeSensor("fan", speed*10);
                    tft.displayControlSpeed(speed);
                }
                if(rothandlerP.buttonPressed){
                    rothandlerP.buttonPressed = false;
                    tft.displayStatus(co2, t, rh, speed, setPressure, wifi_signal);
                    scrollmenu = statusMode;
                }

                break;
            case setpressureMode:
                if(rothandlerA.rotaryturned){
                    // std::cout << "turned" << std::endl;
                    rothandlerA.rotaryturned = false;
                    setPressure+=rothandlerA.getCount();
                    if(setPressure > 125){
                        setPressure = 125;
                    } else if(setPressure < 0){
                        setPressure = 0;
                    }
                    // vent.writeSensor("fan", speed*10);
                    //sleep_ms(100);
                    tft.displayControlPressureLevel(setPressure);
                }
                if(rothandlerP.buttonPressed ){
                    rothandlerP.buttonPressed = false;
                    //vent.writeSensor("fan", setPressure*0.8*10);
                    tft.displayStatus(co2, t, rh, speed, setPressure, wifi_signal);
                    scrollmenu = controlautomode;
                }

                break;

            case controlautomode:
                // currentPressure = 0;
                if(currentPressure > 125){
                    currentPressure = 125;
                }
                co2 = vent.readSensor("co2");
                rh = vent.readSensor("rh")/10;
                t = vent.readSensor("t")/10;
                fan = vent.readSensor("fan")/10;
                // currentPressure = (vent.readPressure() >= 125) ? 0 : vent.readPressure();
                if (currentPressure != setPressure) {
                    if (currentPressure >= setPressure) {
                        std::cout << "Pressure: " << currentPressure << " SetPressure: " << setPressure << std::endl;
                        speed -= 1;
                        if (speed <= 0) {
                            speed = 0;
                        }
                        vent.writeSensor("fan", (speed * 10));
                    } else if (currentPressure <= setPressure) {
                        std::cout << "Pressure: " << currentPressure << " SetPressure: " << setPressure << "  " << speed
                                  << std::endl;
                        speed += 1;
                        if (speed >= 100) {
                            speed = 100;
                        }
                        //tempPressure = setPressure - currentPressure;
                        vent.writeSensor("fan", (speed * 10));
                    }

                }
                /*if (time_reached(mqttSendTime)) {
                    mqttSendTime = delayed_by_ms(mqttSendTime, 2000);
                    sprintf(buf, R"({"nr": %d, "speed": %d, "setpoint": %d, "pressure": %d, "auto": %s, "error": %s, "co2": %d, "rh": %d, "temp": %d})",
                            ++msg_count, (int)fan, (int)setPressure, currentPressure, autoMode ? "true" : "false", error ? "true" : "false", co2, rh, t);
                    mqttManager.publish(topicPUB, buf, MQTT::QOS0);
                    std::cout << buf << std::endl;*/
        }
        if (time_reached(pressure_poll)){
            currentPressure = (vent.readPressure() >= 125) ? 0 : vent.readPressure();
            pressure_poll = delayed_by_ms(pressure_poll,50);
            tft.displayStatus(co2, t, rh, fan, currentPressure, wifi_signal);
        }
        if(rothandlerP.buttonPressed){
            rothandlerP.buttonPressed = false;
            tft.fill(0);
            tft.mainMenu();
            tft.selectmenu(1);
            scrollmenu = menu;
        }
        //scrollmenu = read;
        break;

        case statusMode:
            if(time_reached(modbus_poll)) {
                // std::cout << "Reading" << mqttManager.getvalue()<<std::endl;
                // vent.writeSensor("fan", 60*10);
                modbus_poll = make_timeout_time_ms(4000);
                co2 = vent.readSensor("co2");
                rh = vent.readSensor("rh") / 10;
                t = vent.readSensor("t") / 10;
                fan = vent.readSensor("fan") / 10;
                sensorPressure = vent.readPressure();
                tft.displayStatus(co2, t, rh, fan, sensorPressure, wifi_signal);
#ifdef USE_MQTT
                sprintf(buf, R"({"nr": %d, "speed": %d, "setpoint": %d, "pressure": %d, "auto": %s, "error": %s, "co2": %d, "rh": %d, "temp": %d})",
                         ++msg_count, (int)fan, (int)setPressure, pressure, autoMode ? "true" : "false", error ? "true" : "false", co2, rh, t);
                 //buf[strlen(buf)] = '\0';
                 mqttManager.publish(topicPUB,buf,MQTT::QOS0);
                 std::cout << buf << std::endl;
#endif
            }
        if(rothandlerP.buttonPressed){
            rothandlerP.buttonPressed = false;
            tft.fill(0);
            tft.mainMenu();
            tft.selectmenu(0);
            scrollmenu = menu;
        }
        break;
        /* case writeMode:
             if(rothandlerA.rotaryturned){
                 rothandlerA.rotaryturned = false;
                 MqttValue+=rothandlerA.getCount();
                 if(MqttValue > 100){
                     MqttValue = 100;
                 } else if(MqttValue < 0){
                     MqttValue = 0;
                 }
                 tft.displayControlSpeed(MqttValue);
             }
             if(rothandlerP.buttonPressed){
                 rothandlerP.buttonPressed = false;
                 tft.fill(0);
                 tft.autoOrManual();
                 tft.selectmenu(1);
                 scrollmenu = menu;
             }
             break;*/
        case MQTTsend:
#ifdef USE_MQTT
            if (time_reached(mqttSendTime)) {
                 mqttSendTime = delayed_by_ms(mqttSendTime, 2000);
                 sprintf(buf, R"({"nr": %d, "speed": %d, "setpoint": %d, "pressure": %d, "auto": %s, "error": %s, "co2": %d, "rh": %d, "temp": %d})",
                         ++msg_count, (int)fan, (int)setPressure, currentPressure, autoMode ? "true" : "false", error ? "true" : "false", co2, rh, t);
                 mqttManager.publish(topicPUB, buf, MQTT::QOS0);
                 std::cout << buf << std::endl;
             }
#endif
            break;
        case connect:
            if(rothandlerA.rotaryturned){
                rothandlerA.rotaryturned = false;
                SubMenucount+=rothandlerA.getCount();
                if(SubMenucount > 2){
                    SubMenucount = 0;
                } else if(SubMenucount < 0){
                    SubMenucount = 2;
                }
                tft.selectWifimenu(SubMenucount);
            }
        if(SubMenucount == 0 && rothandlerP.buttonPressed){
            rothandlerP.buttonPressed = false;
            tft.displayKeyboard(0);
            scrollmenu = connectTowifi;
        } else if(SubMenucount == 1 && rothandlerP.buttonPressed){
            rothandlerP.buttonPressed = false;
            SubMenucount = 0;
            //tft.displayKeyboard(1);
            tft.selctcredenttials(0);
            tft.credenttialsMenu();
            //maxScroll = alphabet.size() - 1;
            scrollmenu = enterCredentials;
        } else if(SubMenucount == 2 && rothandlerP.buttonPressed){
            rothandlerP.buttonPressed = false;
            tft.mainMenu();
            tft.selectmenu(0);
            scrollmenu = menu;
        }
        break;
        case connectTowifi:
            break;
        case enterIDpass:
            break;
        /*case enterCredentials:
            if(rothandlerA.rotaryturned){
                rothandlerA.rotaryturned = false;
                SubMenucount+=rothandlerA.getCount();
                if(SubMenucount > 2){
                    SubMenucount = 0;
                } else if(SubMenucount < 0){
                    SubMenucount = 2;
                }
                tft.selctcredenttials(SubMenucount);
            }
            if (SubMenucount == 0 && rothandlerP.buttonPressed) {
                rothandlerP.buttonPressed = false;
                //tft.displayKeyboard(0);
                scrollmenu = connectTowifi;
            } else if (SubMenucount == 1 && rothandlerP.buttonPressed) {
                rothandlerP.buttonPressed = false;
                SubMenucount = 0;
                //tft.displayKeyboard(1);
                tft.credenttialsMenu();
                tft.selctcredenttials(0);
                scrollmenu = CredentialsMenu;
            } else if (SubMenucount == 2 && rothandlerP.buttonPressed) {
                rothandlerP.buttonPressed = false;
                tft.mainMenu();
                tft.selectmenu(0);
                scrollmenu = menu;
            }
            break;
        case CredentialsMenu:
            tft.displayKeyboard(0);
            maxScroll = alphabet.size() - 1;
            if(rothandlerA.rotaryturned){
                rothandlerA.rotaryturned = false;
                wifimenucount+=rothandlerA.getCount();
                if(wifimenucount > 2){
                    wifimenucount = 0;
                } else if(wifimenucount < 0){
                    wifimenucount = 2;
                }
                tft.selctcredenttials(wifimenucount);
            }
            if(SubMenucount == 0 && rothandlerP.buttonPressed){
                rothandlerP.buttonPressed = false;
                tft.displayKeyboard(0);
                std::printf("ssid");
               // scrollmenu = connectTowifi;
            } else if (SubMenucount == 2 && rothandlerP.buttonPressed) {
                rothandlerP.buttonPressed = false;
                tft.mainMenu();
                tft.selectmenu(0);
                scrollmenu = enterCredentials;
            }
            break;*/

    }
    // cyw43_arch_poll(); // obsolete? - see below
    //mqttManager.yield(100); // socket that client uses calls cyw43_arch_poll()
}

return 0;
}