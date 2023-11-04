#include "screen.h"

uint8_t numberIndex = 0;

bool print_default(TFT_eSPI* tft, char numberBuffer[])
{
    numberIndex = 0; // Reset index to 0
    numberBuffer[numberIndex] = 0; // Place null in buffer
    updateDisplayField(tft, numberBuffer); //clear any remaining numbers

     // Clear the screen
    tft->fillScreen(TFT_BLACK);
    tft->setCursor(4, 4);
    tft->setTextColor(TFT_WHITE, TFT_BLUE);
    
    uint16_t x = tft->width() / 2;
    uint16_t y = tft->height() / 2;
    // Set datum to Middle Left
    //tft.setTextDatum(MR_DATUM);

    // Draw text with left datum in font 6
    tft->drawString("SENIOR DESIGN", x-100, y-20, 4);

    return true;
}

//------------------------------------------------------------------------------------------

void drawKeypad(TFT_eSPI* tft, TFT_eSPI_Button key[], uint16_t keyColor[], char keyLabel[][6])
{
     // Draw keypad background
    tft->fillRect(0, 0, 240, 320, TFT_DARKGREY);

    // Draw number display area and frame
    tft->fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
    tft->drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);

    // Draw the keys
    for (uint8_t row = 0; row < 5; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            uint8_t b = col + row * 3;

            if (b < 3) tft->setFreeFont(&FreeSansBoldOblique9pt7b);
            else tft->setFreeFont(&FreeSansBold12pt7b);

            if(b == 0 || b == 2)
            {
                key[b].initButton(tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                                KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                                120, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                                keyLabel[b], KEY_TEXTSIZE);
                key[b].drawButton();
            }

            else if(b == 1)
            {
                //do nothing
            }

            else{
                key[b].initButton(tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                                KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                                KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                                keyLabel[b], KEY_TEXTSIZE);
                key[b].drawButton();
            }
        }
    }
}

//------------------------------------------------------------------------------------------

void touch_calibrate(TFT_eSPI* tft)
{
 #define CALIBRATION_FILE "/TouchCalData1"
 #define REPEAT_CAL false

    uint16_t calData[5];
    uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft->setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft->fillScreen(TFT_BLACK);
    tft->setCursor(20, 0);
    tft->setTextFont(2);
    tft->setTextSize(1);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);

    tft->println("Touch corners as indicated");

    tft->setTextFont(1);
    tft->println();

    if (REPEAT_CAL) {
      tft->setTextColor(TFT_RED, TFT_BLACK);
      tft->println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft->calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    tft->println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

//------------------------------------------------------------------------------------------

// Print something in the mini status bar
void status(const char *msg, TFT_eSPI* tft) 
{
   #define STATUS_X 120 // Centered on this
   #define STATUS_Y 65 

  tft->setTextPadding(240);
  tft->setCursor(STATUS_X, STATUS_Y);
  tft->setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft->setTextFont(0);
  tft->setTextDatum(TC_DATUM);
  tft->setTextSize(1);
  tft->drawString(msg, STATUS_X, STATUS_Y);
}

void checkPress(TFT_eSPI* tft, TFT_eSPI_Button key[], char keyLabel[][6], uint16_t t_x, uint16_t t_y, bool pressed, char numberBuffer[], const char password[])
{
  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < 15; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < 15; b++) {

    if (b < 3) tft->setFreeFont(LABEL1_FONT);
    else tft->setFreeFont(LABEL2_FONT);

    if (key[b].justReleased()) key[b].drawButton();     // draw normal

    if (key[b].justPressed()) {
      key[b].drawButton(true);  // draw invert

      // if a numberpad button, append the relevant # to the numberBuffer
      if (b >= 3) {
        if (numberIndex < NUM_LEN) {
          numberBuffer[numberIndex] = keyLabel[b][0];
          numberIndex++;
          numberBuffer[numberIndex] = 0; // zero terminate
        }
        status("", tft); // Clear the old status
      }

      // Del button, so delete last char
      if (b == 0) {
        numberBuffer[numberIndex] = 0;
        if (numberIndex > 0) {
          numberIndex--;
          numberBuffer[numberIndex] = 0;//' ';
        }
        status("", tft); // Clear the old status
      }

      if (b == 2) {
        Serial.println(numberBuffer);
        if(checkPass(numberBuffer, password)){
          status("Correct Password", tft);
          numberIndex = 0; // Reset index to 0
          numberBuffer[numberIndex] = 0; // Place null in buffer
        }
        else{
          status("Password Incorrect", tft);
        }
      }
      // we dont really check that the text field makes sense
      // just try to call
      if (b == 1) {
        // status("Value cleared", tft);
        // numberIndex = 0; // Reset index to 0
        // numberBuffer[numberIndex] = 0; // Place null in buffer
      }
    }
  }
}

void updateDisplayField(TFT_eSPI* tft, char numberBuffer[])
{
    // Update the number display field
  tft->setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
  tft->setFreeFont(&FreeSans18pt7b);  // Choose a nicefont that fits box
  tft->setTextColor(DISP_TCOLOR);     // Set the font colour

  // Draw the string, the value returned is the width in pixels
  int xwidth = tft->drawString(numberBuffer, DISP_X + 4, DISP_Y + 12);

  // Now cover up the rest of the line up by drawing a black rectangle.  No flicker this way
  // but it will not work with italic or oblique fonts due to character overlap.
  tft->fillRect(DISP_X + 4 + xwidth, DISP_Y + 1, DISP_W - xwidth - 5, DISP_H - 2, TFT_BLACK);

  delay(10); // UI debouncing
}