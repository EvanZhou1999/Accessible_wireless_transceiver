/*
 * WIRELESS TRANSCEIVER FOR SWITCH ACCESSIBLE DEVICES ===============
 * Using ESPAsyncWebServer Library
 * HARDWARE CONFIGURATIONS ==============================================
 * - ESP32-WROOM-DA Module
 * - RGB LED Pixel Light
 *    - LED Pin 32
 *    - num-Pixels 7
 * - Software Switch Relay
 *    - Pin 27
 * =======================================================================
 * =======================================================================
 */

// Import WiFi Libraries
#include <WiFi.h>
#include <esp_wpa2.h>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>
// Import DNS Library
#include <ESPmDNS.h>

// Import FreeRTOS
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Other Libraries
#include <time.h>
#include <stdlib.h>

// Include html files
#include "index_html.h"

// Import wireless configurations
#include "wireless_config.h"
// Import LED tasks
#include "led_task.h"


// Software Switch Relay Setup
#define SOFT_RELAY_PIN 27

/*
 * =======================================================
 * Instantiate Global Objects/Devices
 * =======================================================
 */

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


/*
 * =======================================================
 * Global Variables
 * =======================================================
 */



/*
 * =======================================================
 * Callback Functions
 * =======================================================
 */
void handleRoot(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", INDEX_HTML);
}

void handleClick (AsyncWebServerRequest *request) {
    int paramValue; // url parameter named "interval" ranging between 0 to 6000ms
    if (request->hasParam("interval") ) {
        paramValue = request->getParam("interval")->value().toInt();
        Serial.println("Get Parameter: "+paramValue);
    }else{
        Serial.println("[ERROR] >>> handleClick: no url Param - interval");
    }
    
    if (paramValue > 6000 && paramValue < 0) {
        Serial.println("[ERROR] >>> handleClick: invalid parameter range (0-6000ms)");
        return;
    }
    
    // Display animation
    LED_Message_queue_send(LED_CIRCLE_IN, 0, 40, 40, false);
    
    // Engage Switch for defined periods
    digitalWrite(SOFT_RELAY_PIN, HIGH);
    delay(paramValue);
    digitalWrite(SOFT_RELAY_PIN, LOW);

    // Display animation
    LED_Message_queue_send(LED_LOAD_OUT, 0, 40, 40, false);

    // Acknowledge with 200 response
    request->send_P(200, "text/plain", "OK");

    // return to normal status indicator
    LED_Message_queue_send(LED_PERSIST_STATUS_2, 100, 20, 0, false);
}

void handleActivation (AsyncWebServerRequest *request) {
    // Display animation
    LED_Message_queue_send(LED_CIRCLE_IN, 0, 40, 40, false);
    // Engage Switch
    digitalWrite(SOFT_RELAY_PIN, HIGH);
    // Acknowledge with 200 response
    request->send_P(200, "text/plain", "OK");
}

void handleDeactivation (AsyncWebServerRequest *request) {
    // Display animation
    LED_Message_queue_send(LED_LOAD_OUT, 0, 40, 40, false);
    // Disengage Switch
    digitalWrite(SOFT_RELAY_PIN, LOW);
    // Acknowledge with 200 response
    request->send_P(200, "text/plain", "OK");
    // return to normal status indicator
    LED_Message_queue_send(LED_PERSIST_STATUS_2, 100, 20, 0, false);
}

/*
 * =======================================================
 * Functions
 * =======================================================
 */


bool connectWifi_WAP2_Personal() {
    /*
    //ESP32 As access point
      WiFi.mode(WIFI_AP); //Access Point mode
      WiFi.softAP(ssid, password);
    */
    WiFi.mode(WIFI_STA);
    //Serial.print("MAC address >> ");
    //Serial.println(WiFi.macAddress());
    WiFi.begin(ssid, password);
    // Make Sure Wifi is connected
    float timeout = 0.0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
        timeout += 0.5;
        if (timeout > 30){ // timeout is set to 30 seconds
            return false;
        }
    }
    Serial.println("Connected to the WiFi network");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);

    return true;
}

bool connectWifi_WAP2_Enterprise() {
    // Set NULL WiFi mode for hostname change
    WiFi.mode(WIFI_MODE_NULL);
    // Set host name
    WiFi.disconnect(true);
    delay(1000);
    //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(domainName); // have to do this everytime since new host name won't be remembered
    // Set ESP32 to sation mode
    WiFi.mode(WIFI_MODE_STA);
    // Change mac address if needed
    //esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
    // Configure enterprise network
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wifi_sta_wpa2_ent_enable();
    // Start connection
    WiFi.begin(WAP2_SSID);
    // Make Sure Wifi is connected
    float timeout = 0.0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
        timeout += 0.5;
        if (timeout > 30){ // timeout is set to 30 seconds
            return false;
        }
    }
    Serial.println("Connected to the WiFi network");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(500);
    Serial.print("Host Name >> ");
    Serial.println(WiFi.getHostname());

    return true;
}

// Function that pushes a led message to the queue
// return false if queue full
// ledmessage.useprevcolor set to false
bool LED_Message_queue_send(int pattern, int red, int green, int blue, bool replay) {
    struct LEDMessage msg;
    msg.pattern = pattern;
    msg.colors[0] = red;
    msg.colors[1] = green;
    msg.colors[2] = blue;
    msg.useprevcolor = false;
    msg.allowreplay = replay;

    if (xQueueSend(xLedQueue,( void * ) &msg,( TickType_t ) 0 ) == pdTRUE){
        // xQueueSend returns pdTRUE upon success
        return true;
    }else{
        // xQueueSend returns errQUEUE_FULL if the queue is full
        return false;
    }   
}

// Function that pushes a led message to the queue
// return false if queue full
// ledmessage.useprevcolor set to true
bool LED_Message_queue_send(int pattern, bool replay) {
    struct LEDMessage msg;
    msg.pattern = pattern;
    msg.useprevcolor = true;
    msg.allowreplay = replay;

    if (xQueueSend(xLedQueue,( void * ) &msg,( TickType_t ) 0 ) == pdTRUE){
        // xQueueSend returns pdTRUE upon success
        return true;
    }else{
        // xQueueSend returns errQUEUE_FULL if the queue is full
        return false;
    }  
}

/*
 * =======================================================
 * Setup Function
 * =======================================================
 */
void setup() {
    // Initialize Serial Communication
    Serial.begin(115200);
    Serial.println("\n"); // This is to format output so it does not start on the same line with the gibberish code

    // Create LED Message Queue
    xLedQueue = xQueueCreate(10, sizeof(LEDMessage));
    // Check if Queue was created successfully
    if (xLedQueue == NULL) {
        Serial.print("[ERROR] >>> xLedQueue failed to create");
    }

    // Create LED ring task
    xTaskCreate(
        LED_ring_task,  // Task Function.
        "LED_Ring",     // String name of the Task.
        10000,          // Stack Size in words.
        NULL,           // Parameter passed as input.
        1,              // Task Priority.
        NULL);          // Task Handle.

    // Device start up animation
    LED_Message_queue_send(LED_CIRCLE_IN, 100, 60, 0, false);
    
    //Serial.println("Main loop will now start sleep");
    //for(;;){}
    
    // Software Relay Pin Setup
    pinMode(SOFT_RELAY_PIN, OUTPUT);
    digitalWrite(SOFT_RELAY_PIN, LOW);

    
    // Loading animation
    LED_Message_queue_send(LED_LOADING, 100, 80, 0, true);
    // Connect to Wifi
    bool connectionSuccess;
    if (UseWAPEnterprise) {
        connectionSuccess = connectWifi_WAP2_Enterprise();
    }else{
        connectionSuccess = connectWifi_WAP2_Personal();
    }

    if (connectionSuccess){
        // Setup Local DNS - domain name
        if ( MDNS.begin(domainName) ) {
            Serial.println("mDNS responder started");
        } else {
            Serial.println("Error setting up mDNS responder!");
        }
    
        MDNS.addService("http", "tcp", 80);
    
        // Setup Web server callbacks:
        server.on("/", HTTP_GET, handleRoot);
        server.on("/control/click", handleClick);
        server.on("/control/activate", handleActivation);
        server.on("/control/deactivate", handleDeactivation);
    
        // Begin Server
        server.begin();
        Serial.println("HTTP server started \nOn Domain:\n" + String(domainName) + ".local");
    
        // setup complete animation
        LED_Message_queue_send(LED_LOAD_IN, 0, 80, 0, false);
        LED_Message_queue_send(LED_LOAD_OUT, 0, 80, 0, false);
        // Turn on normal status indicator
        LED_Message_queue_send(LED_PERSIST_STATUS_2, 100, 20, 0, false);
    }else{
        // connection unsuccessful, 
        Serial.println("[ERROR] >> Failed to establish wireless connection");
        // Display error animation
        LED_Message_queue_send(LED_FADEIN, 80, 0, 0, false);
        LED_Message_queue_send(LED_BREATHE, 80, 0, 0, true);
    }


}


/*
 * =======================================================
 * Loop Function
 * =======================================================
 */
void loop() {
    // nothing for server in the loop
    vTaskDelay(1 / portTICK_PERIOD_MS); // Delay 1 millisecond
}
