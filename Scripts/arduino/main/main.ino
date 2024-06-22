// load cell hx711
#include <HX711.h>
// oled 1.3" i2c SH1106 128*64 libs
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>

// Initialize the HX711 sensor with data pin (D3) and clock pin (D2)
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;
bool LOADCELL_CALIBRATED = true;
#define LOADCELL_VIN 4

HX711 scale;

// OLED init
#define i2c_Address 0x3c // initialize with the I2C addr 0x3C Typically eBay OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define XPOS 0
#define YPOS 1
#define DELTAY 2

const int TRIGGER_PIN = 5;
struct HomePageState
{
  int weight = 0;
  bool isCalibrated = false;
  bool isCalibrating = false;
  bool isTare = false;
  bool isTareing = false;
  bool blutoothConnected = false;
  bool blutoothAvailable = false;
  bool isSleeping = false;
  String units = "gms";
  String timee = "";
  String date = "";
};

// 'bluetooth', 48x48px
const unsigned char bitmap_bluetooth[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x08, 0x03, 0xfe,
    0x00, 0x00, 0x00, 0x1c, 0x03, 0xff, 0x00, 0x00, 0x00, 0x3e, 0x03, 0xdf, 0x80, 0x00, 0x00, 0x1f,
    0x03, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 0x83, 0xc7, 0xe0, 0x00, 0x00, 0x07, 0xc3, 0xc3, 0xe0, 0x00,
    0x00, 0x03, 0xe3, 0xc7, 0xc0, 0x00, 0x00, 0x01, 0xf3, 0xcf, 0x80, 0x00, 0x00, 0x00, 0xfb, 0xdf,
    0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00,
    0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8,
    0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0xfb, 0xdf, 0x00, 0x00, 0x00, 0x01, 0xf3, 0xcf, 0x80, 0x00, 0x00, 0x03, 0xe3, 0xc7, 0xc0, 0x00,
    0x00, 0x07, 0xc3, 0xc3, 0xe0, 0x00, 0x00, 0x0f, 0x83, 0xc7, 0xe0, 0x00, 0x00, 0x1f, 0x03, 0xcf,
    0xc0, 0x00, 0x00, 0x3e, 0x03, 0xdf, 0x80, 0x00, 0x00, 0x1c, 0x03, 0xff, 0x00, 0x00, 0x00, 0x08,
    0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 'bluetooth-connected', 48x48px
const unsigned char bitmap_bluetooth_connected[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x08, 0x03, 0xfe,
    0x00, 0x00, 0x00, 0x1c, 0x03, 0xff, 0x00, 0x00, 0x00, 0x3e, 0x03, 0xdf, 0x80, 0x00, 0x00, 0x1f,
    0x03, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 0x83, 0xc7, 0xe0, 0x00, 0x00, 0x07, 0xc3, 0xc3, 0xe0, 0x00,
    0x00, 0x03, 0xe3, 0xc7, 0xc0, 0x00, 0x00, 0x01, 0xf3, 0xcf, 0x80, 0x00, 0x00, 0x00, 0xfb, 0xdf,
    0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x60, 0x3f, 0xfc, 0x06, 0x00, 0x00, 0xf0,
    0x1f, 0xf8, 0x0f, 0x00, 0x01, 0xf8, 0x0f, 0xf0, 0x1f, 0x80, 0x03, 0xfc, 0x07, 0xe0, 0x3f, 0xc0,
    0x03, 0xfc, 0x07, 0xe0, 0x3f, 0xc0, 0x01, 0xf8, 0x0f, 0xf0, 0x1f, 0x80, 0x00, 0xf0, 0x1f, 0xf8,
    0x0f, 0x00, 0x00, 0x60, 0x3f, 0xfc, 0x06, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0xfb, 0xdf, 0x00, 0x00, 0x00, 0x01, 0xf3, 0xcf, 0x80, 0x00, 0x00, 0x03, 0xe3, 0xc7, 0xc0, 0x00,
    0x00, 0x07, 0xc3, 0xc3, 0xe0, 0x00, 0x00, 0x0f, 0x83, 0xc7, 0xe0, 0x00, 0x00, 0x1f, 0x03, 0xcf,
    0xc0, 0x00, 0x00, 0x3e, 0x03, 0xdf, 0x80, 0x00, 0x00, 0x1c, 0x03, 0xff, 0x00, 0x00, 0x00, 0x08,
    0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 'bluetooth-off', 48x48px
const unsigned char bitmap_bluetooth_off[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00,
    0x00, 0x20, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x70, 0x03, 0xfc, 0x00, 0x00, 0x00, 0xf8, 0x03, 0xfe,
    0x00, 0x00, 0x00, 0x7c, 0x03, 0xff, 0x00, 0x00, 0x00, 0x3e, 0x03, 0xdf, 0x80, 0x00, 0x00, 0x1f,
    0x03, 0xcf, 0xc0, 0x00, 0x00, 0x0f, 0x83, 0xc7, 0xe0, 0x00, 0x00, 0x07, 0xc1, 0xc3, 0xe0, 0x00,
    0x00, 0x03, 0xe0, 0xc7, 0xc0, 0x00, 0x00, 0x01, 0xf0, 0x4f, 0x80, 0x00, 0x00, 0x00, 0xf8, 0x1f,
    0x00, 0x00, 0x00, 0x00, 0x7c, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00,
    0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8,
    0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0xfb, 0xdf, 0x00, 0x00, 0x00, 0x01, 0xf3, 0xcf, 0x80, 0x00, 0x00, 0x03, 0xe3, 0xc7, 0xc0, 0x00,
    0x00, 0x07, 0xc3, 0xc3, 0xe0, 0x00, 0x00, 0x0f, 0x83, 0xc7, 0xf0, 0x00, 0x00, 0x1f, 0x03, 0xcf,
    0xf8, 0x00, 0x00, 0x3e, 0x03, 0xdf, 0xfc, 0x00, 0x00, 0x1c, 0x03, 0xff, 0x3e, 0x00, 0x00, 0x08,
    0x03, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x0e, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x04, 0x00,
    0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 'bluetooth-searching', 48x48px
const unsigned char bitmap_bluetooth_searching[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x80, 0x3f, 0xe0,
    0x00, 0x00, 0x01, 0xc0, 0x3f, 0xf0, 0x00, 0x00, 0x03, 0xe0, 0x3d, 0xf8, 0x00, 0x00, 0x01, 0xf0,
    0x3c, 0xfc, 0x00, 0x00, 0x00, 0xf8, 0x3c, 0x7e, 0x03, 0x00, 0x00, 0x7c, 0x3c, 0x3e, 0x07, 0x00,
    0x00, 0x3e, 0x3c, 0x7c, 0x07, 0x80, 0x00, 0x1f, 0x3c, 0xf8, 0x07, 0x80, 0x00, 0x0f, 0xbd, 0xf0,
    0x03, 0x80, 0x00, 0x07, 0xff, 0xe0, 0x03, 0xc0, 0x00, 0x03, 0xff, 0xc0, 0xc3, 0xc0, 0x00, 0x01,
    0xff, 0x81, 0xc3, 0xc0, 0x00, 0x00, 0xff, 0x03, 0xc3, 0xc0, 0x00, 0x00, 0x7e, 0x07, 0xc3, 0xc0,
    0x00, 0x00, 0x7e, 0x07, 0xc3, 0xc0, 0x00, 0x00, 0xff, 0x03, 0xc3, 0xc0, 0x00, 0x01, 0xff, 0x81,
    0xc3, 0xc0, 0x00, 0x03, 0xff, 0xc0, 0xc3, 0xc0, 0x00, 0x07, 0xff, 0xe0, 0x03, 0x80, 0x00, 0x0f,
    0xbd, 0xf0, 0x03, 0x80, 0x00, 0x1f, 0x3c, 0xf8, 0x07, 0x80, 0x00, 0x3e, 0x3c, 0x7c, 0x07, 0x00,
    0x00, 0x7c, 0x3c, 0x3e, 0x07, 0x00, 0x00, 0xf8, 0x3c, 0x7e, 0x02, 0x00, 0x01, 0xf0, 0x3c, 0xfc,
    0x00, 0x00, 0x03, 0xe0, 0x3d, 0xf8, 0x00, 0x00, 0x01, 0xc0, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x80,
    0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 'bluetooth-transfer', 48x48px
const unsigned char bitmap_bluetooth_transfer[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x06, 0x00, 0x02, 0x00, 0xff, 0x80,
    0x0f, 0x00, 0x07, 0x00, 0xff, 0xc0, 0x1f, 0x80, 0x0f, 0x80, 0xf7, 0xe0, 0x1f, 0x80, 0x07, 0xc0,
    0xf3, 0xf0, 0x3f, 0xc0, 0x03, 0xe0, 0xf1, 0xf8, 0x7f, 0xe0, 0x01, 0xf0, 0xf0, 0xf8, 0xff, 0xf0,
    0x00, 0xf8, 0xf1, 0xf0, 0x0f, 0x00, 0x00, 0x7c, 0xf3, 0xe0, 0x0f, 0x00, 0x00, 0x3e, 0xf7, 0xc0,
    0x0f, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x0f, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x0f, 0x00, 0x00, 0x07,
    0xfe, 0x00, 0x0f, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00,
    0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00,
    0x0f, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x0f, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x0f, 0x00, 0x00, 0x3e,
    0xf7, 0xc0, 0x0f, 0x00, 0x00, 0x7c, 0xf3, 0xe0, 0x0f, 0x00, 0x00, 0xf8, 0xf1, 0xf0, 0x0f, 0x00,
    0x01, 0xf0, 0xf0, 0xf8, 0xff, 0xf0, 0x03, 0xe0, 0xf1, 0xf8, 0x7f, 0xe0, 0x07, 0xc0, 0xf3, 0xf0,
    0x3f, 0xc0, 0x0f, 0x80, 0xf7, 0xe0, 0x1f, 0x80, 0x07, 0x00, 0xff, 0xc0, 0x1f, 0x80, 0x02, 0x00,
    0xff, 0x80, 0x0f, 0x00, 0x00, 0x00, 0xff, 0x00, 0x06, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 1520)
const int bitmap_allArray_LEN = 5;
const unsigned char *bitmap_allArray[5] = {
    bitmap_bluetooth,
    bitmap_bluetooth_connected,
    bitmap_bluetooth_off,
    bitmap_bluetooth_searching,
    bitmap_bluetooth_transfer};
String startMarker_Ard = "arduino:";
String endMarker_Ard = ":oniudra";
String startMarker_Esp = "esp32:";
String endMarker_Esp = ":23pse";
bool debug = true;
bool connected = false;
int weight = 0;
bool sendWeights = false;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600); // to esp32
  // turn on Load Cell
  turnOnLoadCell();
  // turn on display
  turnOnDisplay();
  delay(1);
  // calibrateLoadCell();
}

void loop()
{
  // put your main code here, to run repeatedly:
  readLoadCell(); // read from load cell
  readEsp();      // read from esp32
  readSerial();   // read from serial monitor
  updateScreen();
}

void turnOnDisplay()
{
  display.begin(i2c_Address, true); // Address 0x3C default
  // display.setContrast (0); // dim display

  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();
  // sensor.sleepMode(true);   // Turn off the sensor
  // sensor.sleepMode(false);  // Turn on the sensor
  //  display.drawBitmap(30, 16,  epd_bitmap_IMG_0607__Edited_, 32, 32, 1);
  display.display();
}
void turnOnLoadCell(void)
{
  // turn on sensor by setting pin 4 to high
  pinMode(LOADCELL_VIN, OUTPUT);
  digitalWrite(LOADCELL_VIN, HIGH);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  // you need to wait for the sensor to be ready and wait for the OLED to power up
  delay(500);
}
void calibrateLoadCell()
{
  if (scale.is_ready())
  {
    scale.set_scale();
    log("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    log("Tare done...");
    log("Place a known weight on the scale and enter the weight in grams:");

    // Wait for the user to enter the known weight
    while (!Serial.available())
    {
      // Do nothing, just wait
    }
    String weightStr = Serial.readStringUntil('\n');
    float knownWeight = weightStr.toFloat();

    Serial.print("Using known weight: ");
    log(knownWeight);
    delay(5000);

    long reading = scale.get_units(10);
    Serial.print("Result: ");
    log(reading);

    float calibrationFactor = reading / knownWeight;
    Serial.print("Calibration factor is: ");
    log(calibrationFactor);

    scale.set_scale(calibrationFactor); // Set the new calibration factor
    LOADCELL_CALIBRATED = true;
  }
  else
  {
    log("HX711 not found.");
  }
  delay(1000);
}

void readLoadCell()
{
  // Read only when data is available!
  if (scale.is_ready() && LOADCELL_CALIBRATED)
  {
    // Read the data from the sensor
    const int sensorRead = scale.get_units(10);
    weight = sensorRead;
    String str = "Weight: " + String(sensorRead) + " g";
    log(str);
    if (connected && sendWeights)
    {
      sendToESP32(str);
    }
  }
}
void readEsp() // read from esp32
{
  String str = "";
  while (Serial1.available())
  {
    char c = Serial1.read();
    str += c;
  }
  handleEsp32Request(str);
}
void readSerial() // read from serial monitor
{
  if (Serial.available())
  {
    // read serial data and convert to string
    String data = Serial.readStringUntil('\n');
    // log(data);
    sendToESP32(data);
  }
}

void handleEsp32Request(String data)
{
  // log("esp32: " + data);

  if (isEspMessage(data)) // check if data contains the start and end marker
  {
    String request = extractEspMessage(data);
    // log("esp32: " + request);
    // handshake
    if (request == "Are you Smart-Scale")
    {
      log("Yes, I am Smart-Scale");
      sendToESP32("Yes, I am Smart-Scale");
      connected = true;
    }
    if (request == "connect")
    {
      connected = true;
    }
    if (request == "disconnect")
    {
      connected = false;
    }
    if (request == "getWeight")
    {
      readLoadCell();
    }
    if (request == "start sending weights")
    {
      sendWeights = true;
    }
    if (request == "tare")
    {
      scale.tare();
    }
    if (request == "calibrate")
    {
      calibrateLoadCell();
    }
  }
}
void sendToESP32(String data)
{
  Serial1.println(" " + startMarker_Ard + data + endMarker_Ard);
}
bool isEspMessage(String data)
{
  return data.indexOf(startMarker_Esp) >= 0 && data.indexOf(endMarker_Esp) >= 0;
}
String extractEspMessage(String data)
{
  // get string between $$# and #$$
  int startIndex = data.indexOf(startMarker_Esp) + startMarker_Esp.length();
  int endIndex = data.indexOf(endMarker_Esp);
  return data.substring(startIndex, endIndex);
}

void drawHomePage(const HomePageState &state)
{
  // Clear the display
  display.clearDisplay();

  // Draw top bar with Bluetooth, time, and date
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("Bluetooth: ");
  display.println(state.blutoothConnected ? "Connected" : "Disconnected");

  display.print("Time: ");
  display.println(state.timee);
  display.print("Date: ");
  display.println(state.date);

  // Draw calibration and tare information
  if (state.isCalibrating)
  {
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 20);
    display.println("Calibrating...");
  }
  else if (state.isTareing)
  {
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 20);
    display.println("Taring...");
  }

  // Draw weight information
  if (state.isCalibrated)
  {
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 40);
    display.print("Weight: ");
    display.print(state.weight);
    display.print(" ");
    display.println(state.units);
  }

  // Display the content on the OLED
  display.display();
}
void drawchar(void)
{
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++)
  {
    if (i == '\n')
      continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
  delay(1);
}
void displayText(const String &text, const int x, const int y)
{
  // text display tests
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(x, y);
  display.println(text);
  // display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  // display.println(3.141592);
  // display.setTextSize(2);
  // display.setTextColor(SH110X_WHITE);
  // display.print("0x"); display.println(0xDEADBEEF, HEX);
}
void updateScreen()
{
  // Clear the display
  display.clearDisplay();
  // Draw the bitmap
  // display.drawBitmap(0, 0, bitmap_bluetooth, 48, 48, SH110X_WHITE);
  displayText("Weight: " + String(weight) + " g", 0, 0);
  displayText("master: " + String(connected ? "connected" : "disconnected"), 0, 20);
  // Display the content on the OLED
  display.display();
}

void log(String data)
{
  if (debug)
  {
    Serial.println(data);
  }
}