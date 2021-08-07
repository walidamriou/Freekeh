/**
 * @file firmware_config.h
 * @brief config firmware such as: firmware target (thing or gateway), MAC address, board or microcontroller used. 
 *
 * @version 1.0.0
 * 
 * @author Walid Amriou
 * @date 08/05/2021
 * 
 */

#ifndef FIRMWARE_CONFIG_H_INCLUDED
#define FIRMWARE_CONFIG_H_INCLUDED



/* firmware target */

// To set the target uncomment and comment between the next two lines
#define freekeh_gateway // If uncomment this line, comment the next line
//#define freekeh_thing // If uncomment this line, comment the last line


/* board or microcontroller used */

#define freekeh_esp32_pico_m5stickc // based ESP32-PICO M5StickC module
//#define freekeh_esp32_dev           // based ESP32 dev board
//#define freehek_esp8266_e12n        // based ESP8266 E12n dev board


/* Gateway and Things config */

// Number of things (should be less then 20)
uint8_t number_of_things; 

// MAC address
#define freekeh_things_number 2
uint8_t freekeh_gateway_mac_address[] = {0x50, 0x02, 0x91, 0x89, 0x3D, 0xDC}; // Freekeh Gateway based ESP32-PICO M5StickC module
uint8_t freekeh_thing_1_mac_address[] = {0xDC, 0x4F, 0x22, 0x0A, 0x41, 0x13}; // Freekeh Thing based ESP32 dev board
uint8_t freekeh_thing_2_mac_address[] = {0x10, 0x52, 0x1C, 0x67, 0x79, 0x40}; // Freekeh Thing based ESP8266 E12n dev board

#endif