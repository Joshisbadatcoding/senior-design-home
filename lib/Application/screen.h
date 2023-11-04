#include "FS.h"

#include <SPI.h>
#include <TFT_eSPI.h> 
#include "password.h"

#define NUM_LEN 12

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b    // Key label font 2

// Keypad start position, key sizes and spacing
#define KEY_X 40 // Centre of key
#define KEY_Y 96
#define KEY_W 64 // Width and height
#define KEY_H 32
#define KEY_SPACING_X 16 // X and Y gap
#define KEY_SPACING_Y 18
#define KEY_TEXTSIZE 1   // Font size multiplier

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData1"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

// Status line for messages
#define STATUS_X 120 // Centered on this
#define STATUS_Y 65

// Numeric display box size and location
#define DISP_X 1
#define DISP_Y 10
#define DISP_W 238
#define DISP_H 50
#define DISP_TSIZE 3
#define DISP_TCOLOR TFT_CYAN


bool print_default(TFT_eSPI* tft, char numberBuffer[]);
void drawKeypad(TFT_eSPI* tft, TFT_eSPI_Button key[], uint16_t keyColor[], char keyLabel[][6]);
void touch_calibrate(TFT_eSPI* tft);
void status(const char *msg, TFT_eSPI* tft);
void checkPress(TFT_eSPI* tft, TFT_eSPI_Button key[], char keyLabel[][6], uint16_t t_x, uint16_t t_y, bool pressed, char numberBuffer[], const char password[]);
void updateDisplayField(TFT_eSPI* tft, char numberBuffer[]);