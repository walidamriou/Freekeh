/**
 * @file main.cpp
 * @brief main cpp file of freekeh firmware
 *
 * @version 1.0.0
 * 
 * @author Walid Amriou
 * @date 08/07/2021
 * 
 */


/* Includes */
#include <Arduino.h>
#include "firmware_config.h"

#include <esp_now.h>
#include <WiFi.h>

#ifdef freekeh_esp32_pico_m5stickc
#include <M5StickC.h>
#endif

#define freekeh_serial_speed 115200
#define freekeh_gateway_check_time 8000        // Every 8 s check (without the time of waiting the thing to response)
#define freekeh_thing_check_time 200        // Every 0.2 s check (without the time of waiting the thing to response)
#define freekeh_gateway_wait_thing_data_max 50 // Wait the thing to response 5 s

// To know with thing will the gateway connect
uint8_t freekeh_things_index=1;
// thing address buffer
uint8_t * freekeh_thing_adderss;
// To know if the gateway send again or pass when there is error
uint8_t freekeh_gateway_send_again = 1;
// The gateway send again when there is error with the think
uint8_t freekeh_gateway_send_again_index = 1;

uint8_t freekeh_gateway_wait_thing_data = 0;
uint8_t freekeh_gateway_flag_received_data=0;
uint8_t freekeh_gateway_received_data = 0; 


uint8_t freekeh_thing_flag_received_data=0;
uint8_t freekeh_thing_received_data = 0; 

// Flag when the data was sent successfully (from gateway or thing)
uint8_t freekeh_send_success;

// Flag set 1 when a data was received successfully
uint8_t flag_received_data=0;

uint8_t Send_Data;

// return the things MAC address depending on a index
uint8_t * freekehiot_gateway_thing_mac_address(uint8_t things_index){
  uint8_t * address_return;
  if(things_index==0){
    address_return = freekeh_thing_1_mac_address;
    return address_return;
  }
  else if(things_index==1){
    address_return = freekeh_thing_2_mac_address;
    return address_return;
  }
  return 0;
}

// Callback when a new data is sent
// This function prints if the message was successfully delivered or not
// If the message is delivered successfully, the status variable returns 0
void freekeh_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0){
    freekeh_send_success = 1;
  }
  else{
    // There are many reasons of fail: the destination device doesn???t exist; 
    //                                 the channels of the devices are not the same;
    //                                 the action frame is lost when transmitting on the air, etc.
    freekeh_send_success = 0;
  }
}

// Callback when new data is received
void freekheh_received_callback(const uint8_t * mac, const uint8_t *received_data_from_espnow, int len) {
  #ifdef freekeh_gateway
  memcpy(&freekeh_gateway_received_data, received_data_from_espnow, sizeof(freekeh_gateway_received_data));
  // set the flag to indicate that data has been received
  freekeh_gateway_flag_received_data=1;
  #endif

  #ifdef freekeh_thing
  memcpy(&freekeh_thing_received_data, received_data_from_espnow, sizeof(freekeh_thing_received_data));
  // set the flag to indicate that data has been received
  freekeh_thing_flag_received_data=1;
  #endif


}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(freekeh_serial_speed);
  
  #ifdef freekeh_esp32_pico_m5stickc
  // this to init power manager, serial, and LCD of M5STickc
  M5.begin();
  #endif
  
  #ifdef freekeh_gateway
  Serial.println("Start freekeh gateway.");
  #endif

  #ifdef freekeh_thing
  Serial.println("Start freekeh thing.");
  #endif

  #ifdef freekeh_thing
  Serial.println("Start freekeh thing config.");
  #endif

  // Set the device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW protocol
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    // go out from setup()
    return;
  }

  // Register sending callback function
  esp_now_register_send_cb(freekeh_send_callback);
  // register peers for gateway
  #ifdef freekeh_gateway
  for(int i=0;i<freekeh_things_number;i++){
  // In order to send data to another boards/board, system pair it as a peer, so need to register a peer
  // create a peerinfo object 
  esp_now_peer_info_t peerInfo;
  // Copy a mac address of the peer from broadcastAddress to peerInfo.peer_addr
  memcpy(peerInfo.peer_addr, freekehiot_gateway_thing_mac_address(i), 6); 
  Serial.println("Test..................................");
  // Set Wi-Fi channel that peer uses to send/receive ESPNOW data
  // The range of the channel of paired devices is from 0 to 14.
  // If the channel is set to 0, data will be sent on the current channel. 
  // Otherwise, the channel must be set as the channel that the local device is on.
  peerInfo.channel = 0;  
  // ESPNOW data that this peer sends/receives is encrypted or not
  peerInfo.encrypt = false;
  // Add peer   
  // add the device to the paired device list before send data to this device (max peers is 20)
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  }
  #endif

  // register peers for thing
  #ifdef freekeh_thing
  // In order to send data to another boards/board, system pair it as a peer, so need to register a peer
  // create a peerinfo object 
  esp_now_peer_info_t peerInfo;
  // Copy a mac address of the peer from broadcastAddress to peerInfo.peer_addr
  memcpy(peerInfo.peer_addr, freekeh_gateway_mac_address, 6); 
  // Set Wi-Fi channel that peer uses to send/receive ESPNOW data
  // The range of the channel of paired devices is from 0 to 14.
  // If the channel is set to 0, data will be sent on the current channel. 
  // Otherwise, the channel must be set as the channel that the local device is on.
  peerInfo.channel = 0;  
  // ESPNOW data that this peer sends/receives is encrypted or not
  peerInfo.encrypt = false;
  // Add peer   
  // add the device to the paired device list before send data to this device (max peers is 20)
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  #endif

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(freekheh_received_callback);

  #ifdef freekeh_thing
  Serial.println("End freekeh thing config.");
  #endif

}

void loop() {

  // Get the data from the things from Gatewat via ESP-NOW
  #ifdef freekeh_gateway
  // get thing address
  freekeh_thing_adderss=freekehiot_gateway_thing_mac_address(freekeh_things_index);
  // Go out from the while when the gateway send correctly to the thing or send twice 
  while(freekeh_gateway_send_again==1){
    Send_Data=freekeh_thing_get_data;
    esp_err_t result = esp_now_send(freekeh_thing_adderss, (uint8_t *) &Send_Data, sizeof(Send_Data));
    if (result == ESP_OK) {
      Serial.println("Sent with success, waiting the Thing to send the data.");
      freekeh_gateway_send_again=0;
    }
    else {
      // There are many reasons of fail: the destination device doesn???t exist; 
      //                                 the channels of the devices are not the same;
      //                                 the action frame is lost when transmitting on the air, etc.
      Serial.print("Error sending when sent, problem with Thing");
      Serial.print(freekeh_things_index);
      Serial.println(" .");
      freekeh_gateway_send_again_index++;
      // Send two time only 
      if(freekeh_gateway_send_again_index==3){
        freekeh_gateway_send_again=0;
      }
    }
  }
  // variables to default for next send
  freekeh_gateway_send_again=1;
  freekeh_gateway_send_again_index=1;
  // pass next thing for next send 
  freekeh_things_index++;
  if(freekeh_things_index>freekeh_things_number){
    freekeh_things_index=1;
  }
  // pass to next thing if thing did not send the response withing x time (default 10s )
  while((freekeh_gateway_wait_thing_data<freekeh_gateway_wait_thing_data_max) && (freekeh_gateway_flag_received_data==0) ){
    freekeh_gateway_wait_thing_data++;
    delay(100); // 100 ms
  }
  // if receive the data
  if(freekeh_gateway_flag_received_data==1){
    freekeh_gateway_flag_received_data=0;
    Serial.print("The data received: ");
    Serial.println(freekeh_gateway_received_data);
    freekeh_gateway_flag_received_data=0;
  }
  // if the time out waiting the thing to response
  else if(freekeh_gateway_wait_thing_data<freekeh_gateway_wait_thing_data_max){
    Serial.println("The time is out and the thing did not respone.");
  }
  freekeh_gateway_wait_thing_data=0;

  delay(freekeh_gateway_check_time);
  #endif

  // Send data from thing to gateway
  #ifdef freekeh_thing
  if(freekeh_thing_flag_received_data==1){
    freekeh_thing_flag_received_data=0;
    // send the data to gateway
    //if(freekeh_gateway_received_data==freekeh_thing_get_data){
      freekeh_gateway_received_data=0;
      
      Send_Data=222; // Example Data send from Thing
      esp_err_t result = esp_now_send(freekeh_gateway_mac_address, (uint8_t *) &Send_Data, sizeof(Send_Data));
    if (result == ESP_OK) {
      Serial.println("Sent Data to gateway with success.");
    }
    else {
      // There are many reasons of fail: the destination device doesn???t exist; 
      //                                 the channels of the devices are not the same;
      //                                 the action frame is lost when transmitting on the air, etc.
      Serial.println("Error when sent.");
    }
  
    //}

  }
  delay(freekeh_thing_check_time);
  #endif
}


