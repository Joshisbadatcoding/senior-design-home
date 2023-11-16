#include <esp_now.h>
#include <WiFi.h>

uint8_t remoteMac[] = {0x7C, 0xDF, 0xA1, 0x97, 0xCD, 0xFE};
const int ledPin = 13;  // Pin for the LED
int X = 2023;

// RECEIVER
void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  // Compare MAC addresses
  if (memcmp(mac, remoteMac, 6) == 0) {
    String Pass = String(X);
    
    if(X== 2027)
    {
      X = 2023;
    }
    if (memcmp(data, Pass.c_str(), Pass.length()) == 0) {
      Serial.print("Received from: ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", mac[i]);
        if (i < 5) Serial.print(":");
      }

      Serial.printf(", Data: %.*s\n", len, data);
      X++;
      // Check if the received data is "2023"
      // Toggle the LED on pin 13
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the data receive callback
  esp_now_register_recv_cb(OnDataRecv);

  // Initialize LED pin
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Your main code here
}
