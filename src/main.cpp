/*
  The TFT_eSPI library incorporates an Adafruit_GFX compatible
  button handling class, this sketch is based on the Arduin-o-phone
  example.

  This example diplays a keypad where numbers can be entered and
  send to the Serial Monitor window.

  The sketch has been tested on the ESP8266 (which supports SPIFFS)

  The minimum screen size is 320 x 240 as that is the keypad size.
*/

// The SPIFFS (FLASH filing system) is used to hold touch screen
// calibration data

#include "FS.h"
#include <string.h>
#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
#include "screen.h"
#include "password.h"

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

// Number length, buffer for storing it and character index
#define NUM_LEN 12
char numberBuffer[NUM_LEN + 1] = "";
const char password[] = "1234";

// Create 15 keys for the keypad
char keyLabel[15][6] = {"DEL", "", "ENTER", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "#" };
uint16_t keyColor[15] = {TFT_RED, TFT_DARKGREY, TFT_DARKGREEN,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE
                        };

// Invoke the TFT_eSPI button class and create all the button objects
TFT_eSPI_Button key[15];

bool idle_state = true;
unsigned long last_press;
//unsigned long runtime;

//------------------------------------------------------------------------------------------

void setup() {
  // Use serial port
  Serial.begin(9600);

  // Initialise the TFT screen
  tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(0);

  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate(&tft);
  
  //print default message
  print_default(&tft, numberBuffer);
}

//------------------------------------------------------------------------------------------
bool printed = false;

void loop(void) {
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  //runtime = millis();
  if((millis() - last_press) >= 15000) //testing with 15 seconds
  {
    idle_state = true;
    if(!printed)
      printed = print_default(&tft, numberBuffer);
  }

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = tft.getTouch(&t_x, &t_y);

  if(pressed) last_press = millis(); //save time stamp of most current screen press

  if(pressed && idle_state)
  {
    // Draw keypad
    drawKeypad(&tft, key, keyColor, keyLabel);
    //Serial.println("drew keypad");
    idle_state = false;
    printed = false;
  }

  checkPress(&tft, key, keyLabel, t_x, t_y, pressed, numberBuffer, password);
  updateDisplayField(&tft, numberBuffer);

  //add code here
}