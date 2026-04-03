//
// Pred kompilaci nainstalovat ESP32 v manazeru desek od Espressif Systems

#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <SPI.h>
#include <GxEPD2_3C.h> // nutno doinstalovat
#include <ArduinoJson.h> // nutno doinstalovat
#include <WiFiClientSecure.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>

#include "Moon.h"
#include "Nameday_cz.h"
#include "Ics.h"
#include "CZUbuntuBold20.h"
#include "CZUbuntuBold18.h"
#include "CZUbuntuBold12.h"
#include "CZUbuntuBold9.h"
#include "CZPTSansBold20.h"
#include "CZPTSansBold22.h"
#include "CZPTSansRegular18.h"
#include "CZPTSansRegular12.h"
#include "CZPTSansRegular9.h"
#include "CZSpaceMonoRegular18.h"
#include "CZSpaceMonoRegular12.h"
#include "CZSpaceMonoRegular9.h"
#include "CZSpaceMonoRegular5.h"
#include "CZProstoOneRegular18.h"
#include "CZProstoOneRegular12.h"
#include "CZProstoOneRegular9.h"
#include "CZProstoOneRegular5.h"
#include "CZSigmarRegular20.h"

///////////////////////////////
//
//  +------+----------------------------
//  |      | [wealth] [wind] temp
//  |  day | [ icon ] [ocpn]  min max
//  +      +----------------------------
//  |  nr. | birthday or first event
//  +      +----------------------------
//  |      | second event
//  +------+----------------------------
//
///////////////////////////////

// ===== WIFI =====
//const char* ssid = "Darina";
//const char* password = "bublina22";

// ===== WIFI =====
const char* ssid = "Cisco";
const char* password = "Posta128";


// ===== ICS =====
String ICS_URL = "https://calendar.google.com/calendar/ical/fractvival%40gmail.com/private-2f207af2f9c073193186f1d706bfbd29/basic.ics";

/*
// ===== BRNO =====
double LAT = 49.195;
double LON = 16.607;
*/

// ===== HOSTERADICE =====
double LAT = 48.9500557;
double LON = 16.259316;


// ===== DISPLAY =====
#define CS_PIN   10
#define DC_PIN   9
#define RES_PIN  46
#define BUSY_PIN 3

GxEPD2_3C<GxEPD2_420c_GDEY042Z98, GxEPD2_420c_GDEY042Z98::HEIGHT> display(
  GxEPD2_420c_GDEY042Z98(CS_PIN, DC_PIN, RES_PIN, BUSY_PIN)
);


const unsigned char sunIcon[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x0c, 0x18, 0x30, 0x0e, 
	0x00, 0x70, 0x07, 0x7e, 0xe0, 0x02, 0xff, 0x40, 0x01, 0xe7, 0x80, 0x03, 0x81, 0xc0, 0x03, 0x81, 
	0xc0, 0x7b, 0x00, 0xde, 0x7b, 0x00, 0xde, 0x03, 0x81, 0xc0, 0x03, 0x81, 0xc0, 0x01, 0xe7, 0x80, 
	0x02, 0xff, 0x40, 0x07, 0x7e, 0xe0, 0x0e, 0x00, 0x70, 0x1c, 0x18, 0x30, 0x08, 0x18, 0x00, 0x00, 
	0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00
};

const unsigned char moonIcon[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x0f, 0xfe, 0x00, 0x03, 0x9f, 0x80, 0x01, 
	0xc3, 0x80, 0x01, 0xc1, 0xc0, 0x00, 0xe0, 0xe0, 0x00, 0x60, 0xe0, 0x00, 0x70, 0x60, 0x00, 0x70, 
	0x70, 0x00, 0x70, 0x70, 0x00, 0x70, 0x70, 0x00, 0x70, 0x70, 0x00, 0x60, 0x60, 0x00, 0x60, 0xe0, 
	0x00, 0xe0, 0xe0, 0x01, 0xc1, 0xc0, 0x01, 0xc3, 0x80, 0x03, 0x9f, 0x00, 0x0f, 0xfe, 0x00, 0x03, 
	0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char isMoon[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x07, 0xc0, 0x00, 0x0f, 
	0xc0, 0x00, 0x1c, 0xe0, 0x00, 0x1c, 0xe0, 0x00, 0x38, 0x70, 0x00, 0x38, 0x70, 0x00, 0x30, 0x38, 
	0x00, 0x30, 0x3c, 0x00, 0x30, 0x1f, 0x00, 0x30, 0x0f, 0xc0, 0x38, 0x03, 0xfc, 0x38, 0x00, 0xfc, 
	0x1c, 0x00, 0x38, 0x1e, 0x00, 0x38, 0x0f, 0x00, 0xf0, 0x07, 0xc3, 0xe0, 0x03, 0xff, 0xc0, 0x00, 
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char noMoon[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x47, 0xc0, 0x00, 0xe7, 
	0xc0, 0x00, 0xf0, 0xe0, 0x00, 0x78, 0xe0, 0x00, 0x3c, 0x70, 0x00, 0x3e, 0x70, 0x00, 0x3f, 0x38, 
	0x00, 0x37, 0x9c, 0x00, 0x33, 0xcf, 0x00, 0x31, 0xe7, 0xc0, 0x38, 0xf3, 0xfc, 0x38, 0x78, 0xfc, 
	0x1c, 0x3c, 0x38, 0x1e, 0x1e, 0x38, 0x0f, 0x0f, 0x30, 0x07, 0xc7, 0x80, 0x03, 0xff, 0xc0, 0x00, 
	0xff, 0xe0, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x60
};



const unsigned char sunny[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x0c, 0x18, 0x30, 0x0e, 
	0x00, 0x70, 0x07, 0x7e, 0xe0, 0x02, 0xff, 0x40, 0x01, 0xe7, 0x80, 0x03, 0x81, 0xc0, 0x03, 0x81, 
	0xc0, 0x7b, 0x00, 0xde, 0x7b, 0x00, 0xde, 0x03, 0x81, 0xc0, 0x03, 0x81, 0xc0, 0x01, 0xe7, 0x80, 
	0x02, 0xff, 0x40, 0x07, 0x7e, 0xe0, 0x0e, 0x00, 0x70, 0x1c, 0x18, 0x30, 0x08, 0x18, 0x00, 0x00, 
	0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00
};

const unsigned char partly[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x10, 0x0c, 0x18, 0x38, 0x0e, 
	0x00, 0x70, 0x07, 0x7e, 0xe0, 0x02, 0xff, 0x40, 0x01, 0xe7, 0x80, 0x03, 0x81, 0xc0, 0x03, 0x81, 
	0xc0, 0x1f, 0x80, 0xde, 0x3f, 0xc0, 0xde, 0x79, 0xe1, 0xc0, 0x70, 0xf9, 0xc0, 0x60, 0x7f, 0x80, 
	0x60, 0x1f, 0x40, 0x70, 0x0e, 0xe0, 0x78, 0x1c, 0x70, 0x3f, 0xfc, 0x30, 0x1f, 0xf8, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char cloudy[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x01, 
	0xff, 0x80, 0x03, 0xc7, 0xc0, 0x03, 0x81, 0xc0, 0x07, 0x00, 0xe0, 0x0f, 0x00, 0xe0, 0x3e, 0x00, 
	0x60, 0x38, 0x00, 0x78, 0x70, 0x00, 0x7c, 0x60, 0x00, 0x1e, 0x60, 0x00, 0x0e, 0x60, 0x00, 0x06, 
	0x70, 0x00, 0x0e, 0x78, 0x00, 0x1e, 0x3f, 0xff, 0xfc, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char rain[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x07, 
	0x81, 0xc0, 0x1f, 0x81, 0xc0, 0x1c, 0x00, 0xf0, 0x38, 0x00, 0xf8, 0x30, 0x00, 0x3c, 0x30, 0x00, 
	0x1c, 0x30, 0x00, 0x0c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x3c, 0x1f, 0xff, 0xf8, 0x07, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x06, 0x18, 0x60, 0x07, 0x1c, 0x70, 0x07, 0x1c, 0x70, 0x03, 0x8e, 0x38, 0x03, 
	0x8e, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char showers[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 
	0x39, 0x80, 0x77, 0x39, 0xc0, 0x73, 0x9d, 0xc0, 0x3b, 0x9c, 0xe0, 0x39, 0xce, 0xe0, 0x1d, 0xce, 
	0x70, 0x1c, 0xe7, 0x70, 0x0e, 0xe7, 0x38, 0x0e, 0x73, 0xb8, 0x07, 0x73, 0x9c, 0x07, 0x39, 0xdc, 
	0x03, 0xb9, 0xce, 0x03, 0x9c, 0xee, 0x01, 0x9c, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char snow[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x18, 0x60, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0xc3, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x18, 0x60, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char storm[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x07, 
	0x81, 0xc0, 0x1f, 0x81, 0xc0, 0x1c, 0x00, 0xf0, 0x38, 0x00, 0xf8, 0x30, 0x00, 0x3c, 0x30, 0x00, 
	0x1c, 0x30, 0x00, 0x0c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x3c, 0x1f, 0xff, 0xf8, 0x07, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x01, 0xc3, 0x80, 0x01, 0xc7, 0x00, 0x01, 0xe7, 0xc0, 0x03, 0xe1, 0x80, 0x00, 
	0xc3, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00
};

const unsigned char fog[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x07, 
	0x81, 0xc0, 0x1f, 0x81, 0xc0, 0x1c, 0x00, 0xf0, 0x38, 0x00, 0xf8, 0x30, 0x00, 0x3c, 0x30, 0x00, 
	0x1c, 0x30, 0x00, 0x0c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x3c, 0x1f, 0xff, 0xf8, 0x07, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0x60, 0x07, 0xff, 0x60, 0x00, 0x00, 0x00, 0x03, 0x7f, 0xc0, 0x03, 
	0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char unknown[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 
	0xff, 0x80, 0x03, 0xc3, 0xc0, 0x01, 0xc3, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0x80, 0x00, 0x07, 
	0x80, 0x00, 0x0f, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 
	0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char nonewind[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x03, 0xc3, 
	0xc0, 0x07, 0x81, 0xe0, 0x07, 0x81, 0xe0, 0x03, 0xc3, 0xc0, 0x01, 0xe7, 0x80, 0x00, 0xff, 0x00, 
	0x00, 0x7e, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char lowwind[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x7e, 
	0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x03, 0xc3, 0xc0, 0x03, 0x81, 0xc0, 0x01, 0x00, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char mediumwind[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x03, 0xc3, 
	0xc0, 0x03, 0x99, 0xc0, 0x01, 0x3c, 0x80, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 
	0x03, 0xc3, 0xc0, 0x03, 0x81, 0xc0, 0x01, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char highwind[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x7e, 0x00, 0x00, 
	0xff, 0x00, 0x01, 0xe7, 0x80, 0x03, 0xc3, 0xc0, 0x03, 0x99, 0xc0, 0x01, 0x3c, 0x80, 0x00, 0x7e, 
	0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x03, 0xc3, 0xc0, 0x03, 0x99, 0xc0, 0x01, 0x3c, 0x80, 
	0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x03, 0xc3, 0xc0, 0x03, 0x81, 0xc0, 0x01, 
	0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct tm timeinfo;


struct Rect 
{
    int x;
    int y;
    int w;
    int h;
    int right() const { return x + w; }
    int bottom() const { return y + h; }
};


struct DayResult 
{
  float tMin;
  float tMax;
  uint8_t icon;  // číslo
  uint8_t wind;  // číslo
  bool valid;
};


#define ICON_SUNNY          0
#define ICON_PARTLY         1
#define ICON_CLOUDY         2
#define ICON_RAIN           3
#define ICON_SHOWERS        4
#define ICON_SNOW           5
#define ICON_STORM          6
#define ICON_FOG            7
#define ICON_UNKNOWN        255


#define WIND_NONE    0
#define WIND_LOW     1
#define WIND_MEDIUM  2
#define WIND_HIGH    3


#define ICONWIDTH 24

ICSCalendar ical;


// ===================

String downloadICS() 
{
  WiFiClientSecure client;
  client.setInsecure();
  const char* host = "calendar.google.com";
  Serial.println("Connecting to Google...");
  if (!client.connect(host, 443)) 
  {
    Serial.println("Connection failed");
    return "";
  }
  Serial.println("Connected!");
  client.print(String("GET ") + ICS_URL + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP32\r\n" +
               "Connection: close\r\n\r\n");
  String payload = "";
  bool body = false;
  while (client.connected() || client.available()) 
  {
    String line = client.readStringUntil('\n');
    if (line == "\r") 
    {
      body = true;
      continue;
    }
    if (body) 
    {
      payload += line;
    }
  }
  Serial.print("Downloaded bytes: ");
  Serial.println(payload.length());
  return payload;
}

// ===== EVENT =====
struct Event {
  int day;
  String text;
};

#define MAX_EVENTS 32
Event events[MAX_EVENTS];
int eventCount = 0;


void parseICS(String data,int year,int month)
{
  eventCount=0;
  int i=0;

  while((i=data.indexOf("BEGIN:VEVENT",i))!=-1)
  {
    int end=data.indexOf("END:VEVENT",i);
    String ev=data.substring(i,end);

    int dI=ev.indexOf("DTSTART:");
    int sI=ev.indexOf("SUMMARY:");

    if(dI!=-1 && sI!=-1)
    {
      String date=ev.substring(dI+8,dI+16);
      String txt=ev.substring(sI+8,ev.indexOf("\n",sI));

      int y=date.substring(0,4).toInt();
      int m=date.substring(4,6).toInt();
      int d=date.substring(6,8).toInt();

      if(y==year && m==month && eventCount<MAX_EVENTS)
      {
        events[eventCount++]={d,txt};
      }
    }
    i=end;
  }
}

void printCZ(const char* text) {
  while (*text) {
    uint8_t c = (uint8_t)*text;

    // ASCII (funguje přímo)
    if (c >= 32 && c <= 126) {
      display.write(c - 32);
    }

    // UTF-8 2-byte znaky
    else if (c == 0xC3) {
      text++;
      switch ((uint8_t)*text) {
        // malé
        case 0xA1: display.write(95); break;  // á
        case 0xA9: display.write(98); break;  // é
        case 0xAD: display.write(100); break; // í
        case 0xB3: display.write(102); break; // ó
        case 0xBA: display.write(106); break; // ú
        case 0xBD: display.write(108); break; // ý

        // velké
        case 0x81: display.write(110); break; // Á
        case 0x89: display.write(113); break; // É
        case 0x8D: display.write(115); break; // Í
        case 0x93: display.write(117); break; // Ó
        case 0x9A: display.write(121); break; // Ú
        case 0x9D: display.write(123); break; // Ý
      }
    }

    else if (c == 0xC4) {
      text++;
      switch ((uint8_t)*text) {
        // malé
        case 0x8D: display.write(96); break;  // č
        case 0x8F: display.write(97); break;  // ď
        case 0x9B: display.write(99); break;  // ě
        case 0x88: display.write(101); break; // ň

        // velké
        case 0x8C: display.write(111); break; // Č
        case 0x8E: display.write(112); break; // Ď
        case 0x9A: display.write(114); break; // Ě
        case 0x87: display.write(116); break; // Ň
      }
    }

    else if (c == 0xC5) {
      text++;
      switch ((uint8_t)*text) {
        // malé
        case 0x99: display.write(103); break; // ř
        case 0xA1: display.write(104); break; // š
        case 0xA5: display.write(105); break; // ť
        case 0xBE: display.write(109); break; // ž
        case 0xAF: display.write(107); break; // ů

        // velké
        case 0x98: display.write(118); break; // Ř
        case 0xA0: display.write(119); break; // Š
        case 0xA4: display.write(120); break; // Ť
        case 0xBD: display.write(124); break; // Ž
        case 0xAE: display.write(122); break; // Ů
      }
    }

    text++;
  }
}


int getTextHeightCZ(const char* text, const GFXfont* font)
{
  int maxAbove = 0; // nad baseline
  int maxBelow = 0; // pod baseline
  while (*text)
  {
    uint8_t c = (uint8_t)*text;
    uint8_t index = 0;

    // --- mapování (stejné jako máš ve width) ---

    if (c >= 32 && c <= 126) {
      index = c - 32;
    }

    else if (c == 0xC3) {
      text++;
      switch ((uint8_t)*text) {
        case 0xA1: index = 95; break;
        case 0xA9: index = 98; break;
        case 0xAD: index = 100; break;
        case 0xB3: index = 102; break;
        case 0xBA: index = 106; break;
        case 0xBD: index = 108; break;

        case 0x81: index = 110; break;
        case 0x89: index = 113; break;
        case 0x8D: index = 115; break;
        case 0x93: index = 117; break;
        case 0x9A: index = 121; break;
        case 0x9D: index = 123; break;
      }
    }

    else if (c == 0xC4) {
      text++;
      switch ((uint8_t)*text) {
        case 0x8D: index = 96; break;
        case 0x8F: index = 97; break;
        case 0x9B: index = 99; break;
        case 0x88: index = 101; break;

        case 0x8C: index = 111; break;
        case 0x8E: index = 112; break;
        case 0x9A: index = 114; break;
        case 0x87: index = 116; break;
      }
    }

    else if (c == 0xC5) {
      text++;
      switch ((uint8_t)*text) {
        case 0x99: index = 103; break;
        case 0xA1: index = 104; break;
        case 0xA5: index = 105; break;
        case 0xBE: index = 109; break;
        case 0xAF: index = 107; break;

        case 0x98: index = 118; break;
        case 0xA0: index = 119; break;
        case 0xA4: index = 120; break;
        case 0xBD: index = 124; break;
        case 0xAE: index = 122; break;
      }
    }
    const GFXglyph *glyph = &font->glyph[index];
    int yOffset = glyph->yOffset;
    int h = glyph->height;
    // nad baseline
    int above = -yOffset;
    // pod baseline
    int below = h + yOffset;
    if (above > maxAbove) maxAbove = above;
    if (below > maxBelow) maxBelow = below;
    text++;
  }
  return maxAbove + maxBelow;
}

int getTextWidthCZ(const char* text, const GFXfont* font) 
{
  int width = 0;

  while (*text) {
    uint8_t c = (uint8_t)*text;
    uint8_t index = 0;

    if (c >= 32 && c <= 126) {
      index = c - 32;
    }

    else if (c == 0xC3) {
      text++;
      switch ((uint8_t)*text) {
        case 0xA1: index = 95; break;
        case 0xA9: index = 98; break;
        case 0xAD: index = 100; break;
        case 0xB3: index = 102; break;
        case 0xBA: index = 106; break;
        case 0xBD: index = 108; break;

        case 0x81: index = 110; break;
        case 0x89: index = 113; break;
        case 0x8D: index = 115; break;
        case 0x93: index = 117; break;
        case 0x9A: index = 121; break;
        case 0x9D: index = 123; break;
      }
    }

    else if (c == 0xC4) {
      text++;
      switch ((uint8_t)*text) {
        case 0x8D: index = 96; break;
        case 0x8F: index = 97; break;
        case 0x9B: index = 99; break;
        case 0x88: index = 101; break;

        case 0x8C: index = 111; break;
        case 0x8E: index = 112; break;
        case 0x9A: index = 114; break;
        case 0x87: index = 116; break;
      }
    }

    else if (c == 0xC5) {
      text++;
      switch ((uint8_t)*text) {
        case 0x99: index = 103; break;
        case 0xA1: index = 104; break;
        case 0xA5: index = 105; break;
        case 0xBE: index = 109; break;
        case 0xAF: index = 107; break;

        case 0x98: index = 118; break;
        case 0xA0: index = 119; break;
        case 0xA4: index = 120; break;
        case 0xBD: index = 124; break;
        case 0xAE: index = 122; break;
      }
    }
    const GFXglyph *glyph = &font->glyph[index];
    width += glyph->xAdvance;
    text++;
  }
  return width;
}


bool CalcSunFromAPI(double lat, double lon,
                          String &sunriseStr, String &sunsetStr)
{
  HTTPClient http;
  String url = "https://api.open-meteo.com/v1/forecast?";
  url += "latitude=" + String(lat,6);
  url += "&longitude=" + String(lon,6);
  url += "&daily=sunrise,sunset";
  url += "&timezone=Europe%2FPrague";
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode != 200) {
    http.end();
    return false;
  }
  String payload = http.getString();
  http.end();
  StaticJsonDocument<2048> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) return false;
  String sunrise = doc["daily"]["sunrise"][0];
  String sunset  = doc["daily"]["sunset"][0];
  // formát: 2026-03-29T06:44
  sunriseStr = sunrise.substring(11,16);
  sunsetStr  = sunset.substring(11,16);
  return true;
}  

static String toHHMM(double t)
{
  int h = (int)t;
  int m = (int)((t - h) * 60 + 0.5);
  if (m >= 60) { m -= 60; h++; }
  if (h >= 24) h -= 24;
  char buf[6];
  sprintf(buf, "%02d:%02d", h, m);
  return String(buf);
}


String formatTime(String input)
{
  int colon = input.indexOf(':');
  if (colon == -1) return input; // když tam není :, vrátí původní

  int hour = input.substring(0, colon).toInt();
  int minute = input.substring(colon + 1).toInt();

  char buffer[6];
  sprintf(buffer, "%02d:%02d", hour, minute);
  return String(buffer);
}


String twoDigitsFromText(String value)
{
  return (value.length() < 2) ? "0" + value : value;
}


String twoDigits(int value)
{
  return (value < 10) ? "0" + String(value) : String(value);
}

void drawTextCentered(
    const Rect& r,
    const char* text,
    int textW,
    int textH
)
{
    int x = r.x + (r.w - textW) / 2;
    int y = r.y + (r.h - textH) / 2 + textH; // baseline!

    display.setCursor(x, y);
    printCZ(text);
}


bool getWeatherForDay(float lat, float lon, tm timeinfo, DayResult &out) {

  out.valid = false;

  char targetDate[11];
  sprintf(targetDate, "%04d-%02d-%02d",
          timeinfo.tm_year + 1900,
          timeinfo.tm_mon + 1,
          timeinfo.tm_mday);

  String url = "https://api.open-meteo.com/v1/forecast?";
  url += "latitude=" + String(lat, 6);
  url += "&longitude=" + String(lon, 6);
  url += "&daily=temperature_2m_max,temperature_2m_min,weathercode,precipitation_sum,windspeed_10m_max";
  url += "&forecast_days=7&timezone=auto";

  HTTPClient http;
  http.begin(url);

  if (http.GET() != 200) {
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  StaticJsonDocument<512> filter;
  filter["daily"]["time"] = true;
  filter["daily"]["temperature_2m_max"] = true;
  filter["daily"]["temperature_2m_min"] = true;
  filter["daily"]["weathercode"] = true;
  filter["daily"]["precipitation_sum"] = true;
  filter["daily"]["windspeed_10m_max"] = true;

  StaticJsonDocument<2048> doc;

  if (deserializeJson(doc, payload, DeserializationOption::Filter(filter))) {
    return false;
  }

  JsonArray time = doc["daily"]["time"];
  JsonArray tMax = doc["daily"]["temperature_2m_max"];
  JsonArray tMin = doc["daily"]["temperature_2m_min"];
  JsonArray wCode = doc["daily"]["weathercode"];
  JsonArray rain = doc["daily"]["precipitation_sum"];
  JsonArray wind = doc["daily"]["windspeed_10m_max"];

  for (int i = 0; i < time.size(); i++) {

    if (strcmp(time[i], targetDate) == 0) {

      out.tMin = tMin[i];
      out.tMax = tMax[i];

      int code = wCode[i];
      float r = rain[i];
      float w_kmh = wind[i];

      // ===== ICON (číslo) =====
      if (code == 0) out.icon = ICON_SUNNY;

      else if (code <= 2)
        out.icon = (r > 0.5) ? ICON_SHOWERS : ICON_PARTLY;

      else if (code == 3)
        out.icon = (r > 0.5) ? ICON_SHOWERS : ICON_CLOUDY;

      else if (code == 45 || code == 48)
        out.icon = ICON_FOG;

      else if (code >= 51 && code <= 67)
        out.icon = ICON_RAIN;

      else if (code >= 71 && code <= 77)
        out.icon = ICON_SNOW;

      else if (code >= 80 && code <= 82)
        out.icon = ICON_SHOWERS;

      else if (code == 85 || code == 86)
        out.icon = ICON_SNOW;

      else if (code >= 95)
        out.icon = ICON_STORM;

      else
        out.icon = ICON_UNKNOWN;

      // ===== WIND (číslo) =====
      float w_ms = w_kmh / 3.6;

      if (w_ms < 2.0) out.wind = WIND_NONE;
      else if (w_ms < 5.0) out.wind = WIND_LOW;
      else if (w_ms < 10.0) out.wind = WIND_MEDIUM;
      else out.wind = WIND_HIGH;

      Serial.print("Wind raw: ");
      Serial.println(w_ms);

      out.valid = true;
      return true;
    }
  }

  return true;
}


void addDays(struct tm &date, int days)
{
    time_t t = mktime(&date);   // převede na timestamp
    t += days * 86400;          // + dny (v sekundách)
    localtime_r(&t, &date);     // zpět do struct tm
}



void drawRow(const Rect& rect, const struct tm& date, const char* event1, const char* event2)
{
  Rect partRect[4];
  const char* daysCZ[] = {
    "Ne", "Po", "Út", "St", "Čt", "Pá", "So"
  };  

  // ===== 1️⃣ LEVÝ SLOUPEC (den) =====
  int leftW = (rect.w * 15) / 100;

  partRect[0] = { rect.x, rect.y, leftW, rect.h };

  Rect topRect;
  Rect bottomRect;

  int split = (partRect[0].h * 60) / 100;

  // horní část (70 %)
  topRect.x = partRect[0].x;
  topRect.y = partRect[0].y;
  topRect.w = partRect[0].w;
  topRect.h = split;

  // spodní část (30 %)
  bottomRect.x = partRect[0].x;
  bottomRect.y = partRect[0].y + split;
  bottomRect.w = partRect[0].w;
  bottomRect.h = partRect[0].h - split;

  display.setTextColor(GxEPD_BLACK);
  display.setFont(&CZUbuntuBold12);
  char dayStr[4];
  snprintf(dayStr, sizeof(dayStr), "%d", date.tm_mday);  
  int dayHeightFont = getTextHeightCZ("A", &CZUbuntuBold12);
  int dayWidthFont = getTextWidthCZ(dayStr, &CZUbuntuBold12);
  drawTextCentered(topRect, dayStr, dayWidthFont, dayHeightFont);  
  //int dayHeightFont = getTextHeightCZ(String(date.tm_mday).c_str(), &CZUbuntuBold9);
  //int dayWidthFont = getTextWidthCZ(String(date.tm_mday).c_str(), &CZUbuntuBold9);
  //drawTextCentered(topRect, String(date.tm_mday).c_str(), dayWidthFont, dayHeightFont);

  display.setTextColor(GxEPD_BLACK);
  display.setFont(&CZUbuntuBold9);
  int daynameHeightFont = getTextHeightCZ("A", &CZUbuntuBold9);
  int daynameWidthFont = getTextWidthCZ(daysCZ[date.tm_wday], &CZUbuntuBold9);
  drawTextCentered(bottomRect, daysCZ[date.tm_wday], daynameWidthFont, daynameHeightFont);



  // ===== 2️⃣ PRAVÝ SLOUPEC =====
  Rect right;
  right.x = rect.x + leftW;
  right.y = rect.y;
  right.w = rect.w - leftW;
  right.h = rect.h;

  // ===== 3️⃣ ROZDĚLENÍ NA 3 ŘÁDKY =====

  int topH = ICONWIDTH + 2;  // ikony + padding
  int remainingH = right.h - topH;

  int midH = remainingH / 2;
  int botH = remainingH - midH;

  // TOP (ikony + teplota)
  partRect[1] = {
    right.x,
    right.y,
    right.w,
    topH
  };

  // MIDDLE (birthday / důležité)
  partRect[2] = {
    right.x,
    right.y + topH,
    right.w,
    midH
  };

  // BOTTOM (další event)
  partRect[3] = {
    right.x,
    right.y + topH + midH,
    right.w,
    botH
  };

  // ===== DEBUG DRAW =====

  display.drawRect(rect.x, rect.y, rect.w, rect.h, GxEPD_RED);
  display.drawRect(partRect[0].x, partRect[0].y,
                   partRect[0].w, partRect[0].h,
                   GxEPD_RED);

/*
  for (int i = 0; i < 4; i++) {
    display.drawRect(partRect[i].x, partRect[i].y,
                     partRect[i].w, partRect[i].h,
                     GxEPD_BLACK);
  }
*/

  display.setTextColor(GxEPD_BLACK);
  display.setFont(&CZPTSansRegular9);

  int event1HeightFont = getTextHeightCZ("A", &CZPTSansRegular9);
  int event1WidthFont = getTextWidthCZ(event1, &CZPTSansRegular9);
  // první řádek
  display.setCursor(
      partRect[2].x + 2,
      partRect[2].y + event1HeightFont
  );
  printCZ(event1);
  //drawTextCentered(partRect[2], event1, event1WidthFont, event1HeightFont);

  // druhý řádek
  int event2HeightFont = getTextHeightCZ("A", &CZPTSansRegular9);
  int event2WidthFont = getTextWidthCZ(event2, &CZPTSansRegular9);
  display.setCursor(
      partRect[3].x + 2,
      partRect[3].y + event2HeightFont
  );
  printCZ(event2);
  //drawTextCentered(partRect[3], event2, event2WidthFont, event2HeightFont);


  DayResult dayRes;
  if (getWeatherForDay(LAT, LON, date, dayRes))
  {
    switch (dayRes.icon) 
    {
      case ICON_SUNNY:
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, sunny, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_PARTLY: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, partly, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_CLOUDY: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, cloudy, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_RAIN: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, rain, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_SHOWERS: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, showers, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_SNOW: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, snow, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_STORM: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, storm, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_FOG: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, fog, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case ICON_UNKNOWN: 
      {
        display.drawBitmap(partRect[1].x+1, partRect[1].y+1, unknown, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
    }    


    switch (dayRes.wind) 
    {
      case WIND_NONE:
      {
        display.drawBitmap(partRect[1].x + ICONWIDTH + 2, partRect[1].y+1, nonewind, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case WIND_LOW:
      {
        display.drawBitmap(partRect[1].x + ICONWIDTH + 2, partRect[1].y+1, lowwind, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case WIND_MEDIUM:
      {
        display.drawBitmap(partRect[1].x + ICONWIDTH + 2, partRect[1].y+1, mediumwind, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
      case WIND_HIGH:
      {
        display.drawBitmap(partRect[1].x + ICONWIDTH + 2, partRect[1].y+1, highwind, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
        break;
      }
    }


    display.setTextColor(GxEPD_BLACK);
    display.setFont(&CZUbuntuBold9);
    int tempHeightFont = getTextHeightCZ("A", &CZUbuntuBold9);
    String tempText = String(dayRes.tMin, 1) + " / " + String(dayRes.tMax, 1) + "°C";
    int tempWidthFont = getTextWidthCZ(tempText.c_str(), &CZUbuntuBold9);
    display.setCursor(
        partRect[1].x + (ICONWIDTH * 2) + 10,
        partRect[1].y + ((partRect[1].h/2)+(tempHeightFont/2))
    );
    printCZ(tempText.c_str());


  }
  




}





void drawAll(struct tm timeinfo)
{
  const char* namemonth[12] = {
    "LEDEN","ÚNOR","BŘEZEN","DUBEN","KVĚTEN","ČERVEN","ČERVENEC","SRPEN","ZÁŘÍ","ŘÍJEN","LISTOPAD","PROSINEC"
  };

  display.firstPage();
  do
  {

    display.setTextColor(GxEPD_RED);
    display.setFont(&CZSigmarRegular20);
    int widthMonthText = getTextWidthCZ(namemonth[timeinfo.tm_mon],&CZSigmarRegular20);
    int xPosMonthText = (display.width() - widthMonthText) / 2;
    int yPosMonthText = 0+((display.height()/100)*11);
    display.setCursor(xPosMonthText+1, yPosMonthText+1);
    printCZ(namemonth[timeinfo.tm_mon]);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(xPosMonthText, yPosMonthText);
    printCZ(namemonth[timeinfo.tm_mon]);

    display.setFont(&FreeMonoBold18pt7b);
    int16_t _x1, _y1;
    uint16_t _w, _h;
    display.getTextBounds(String(timeinfo.tm_mday), 0, 0, &_x1, &_y1, &_w, &_h);        
    int xPosDayText = (display.width() - _w) / 2;
    int yPosDayText = yPosMonthText+_h+((display.height()/100)*4);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(xPosDayText+1, yPosDayText+1);
    display.print(String(timeinfo.tm_mday));
    display.setTextColor(GxEPD_RED);
    display.setCursor(xPosDayText, yPosDayText);
    display.print(String(timeinfo.tm_mday));

    int yPosIcons = yPosDayText-_h;//(display.height()/100)*20;

    display.drawBitmap(0, yPosIcons, sunIcon, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);
    if (MoonCalc::getMoonStatus(LAT, LON))
    {
      display.drawBitmap(display.width()-24, yPosIcons, isMoon, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
    }
    else
    {
      display.drawBitmap(display.width()-24, yPosIcons, noMoon, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    
    }

    int y=timeinfo.tm_year+1900;
    int m=timeinfo.tm_mon+1;
    int d=timeinfo.tm_mday;
    int hour = timeinfo.tm_hour;
    int min = timeinfo.tm_min;

    display.setFont(&FreeSans9pt7b);

    String srStr, ssStr;
    //CalcSunFromAPI(LAT, LON, srStr, ssStr);
    MoonCalc::CalcSun(LAT, LON, srStr, ssStr);
    int16_t _x2, _y2;
    uint16_t _w2, _h2;
    display.getTextBounds(srStr, 0, 0, &_x2, &_y2, &_w2, &_h2);
    int spacing = ICONWIDTH * 30 / 100;
    int xPosSunText = ICONWIDTH + spacing;
    display.setTextColor(GxEPD_RED);
    display.setCursor(xPosSunText, yPosIcons + _h2);
    display.print(srStr);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(xPosSunText, yPosIcons + (_h2 * 2) + (_h2 * 40 / 100));
    display.print(ssStr);

    String riseStr, setStr;
    MoonCalc::compute(LAT, LON, riseStr, setStr);

    bool isUp = MoonCalc::getMoonStatus(LAT, LON);
    // 🔥 jen když ještě nevyšel
    if (!isUp)
    {
      // získej "dnešní" východ tak, že to spočítáš v minulosti
      time_t now = time(nullptr) - 12 * 3600;
      struct tm t;
      localtime_r(&now, &t);
      int y = t.tm_year + 1900;
      int m = t.tm_mon + 1;
      int d = t.tm_mday;
      // 🔥 ručně spočítáš jen dnešní východ
      double prev = MoonCalc::moonAltitude(
          MoonCalc::julianDate(y,m,d,0), LAT, LON);
      double todayRise = -1;
      for(double h=0.05; h<=24; h+=0.05)
      {
        double alt = MoonCalc::moonAltitude(
            MoonCalc::julianDate(y,m,d,h), LAT, LON);
        if(prev < -0.3 && alt >= -0.3)
        {
          double h0 = h - 0.05;
          double frac = prev / (prev - alt);
          todayRise = h0 + frac * 0.05;
          break;
        }

        prev = alt;
      }
      if (todayRise >= 0)
      {
        int tz = t.tm_isdst ? 2 : 1;
        double rise = fmod(todayRise + tz, 24);
        riseStr = MoonCalc::toHHMM(rise);
      }
    }

    int xPosMoonText = display.width() - ICONWIDTH - spacing;
    display.setTextColor(GxEPD_RED);
    display.setCursor(xPosMoonText-_w2, yPosIcons + _h2);
    display.print(riseStr);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(xPosMoonText-_w2, yPosIcons + (_h2 * 2) + (_h2 * 40 / 100));
    display.print(setStr);

    int yPosNamedayText = yPosIcons + (_h2 * 2) + (_h2 * 40 / 100) + ((display.height()*5)/100);

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&CZSpaceMonoRegular12);
    String namedayStr = String(getNameDayCZ(timeinfo.tm_mday, timeinfo.tm_mon+1, timeinfo.tm_year+1900));
    int widthNamedayText = getTextWidthCZ(namedayStr.c_str(),&CZSpaceMonoRegular12);
    int heightNamedayText = getTextHeightCZ(namedayStr.c_str(),&CZSpaceMonoRegular12);
    int xPosNamedayText = (display.width() - widthNamedayText) / 2;
    display.setCursor(xPosNamedayText, yPosNamedayText);
    printCZ(namedayStr.c_str());

    int yPosHeadLine = yPosNamedayText + ((display.height()*5)/100);

    display.drawLine(0, yPosHeadLine - 3, display.width(), yPosHeadLine -3, GxEPD_BLACK);
    display.drawLine(0, yPosHeadLine, display.width(), yPosHeadLine, GxEPD_BLACK);

    String lastSyncText = "";
    lastSyncText += String(twoDigits(hour));
    lastSyncText += ":";
    lastSyncText += String(twoDigits(min));
    lastSyncText += " - ";
    lastSyncText += String(d);
    lastSyncText += ".";
    lastSyncText += String(m);
    lastSyncText += ".";
    lastSyncText += String(y);

    display.setFont(&CZProstoOneRegular5);
    int heightSyncText = getTextHeightCZ(lastSyncText.c_str(),&CZProstoOneRegular5);
    display.setCursor(0, display.height()-1);
    printCZ(lastSyncText.c_str());

    int yPosBottomLine = display.height() - 1 - heightSyncText - 5;

    display.drawLine(0, yPosBottomLine + 3, display.width(), yPosBottomLine + 3, GxEPD_BLACK);
    display.drawLine(0, yPosBottomLine, display.width(), yPosBottomLine, GxEPD_BLACK);

    Serial.print("Width: ");
    Serial.println(display.width());

    Serial.print("Area height: ");
    Serial.println(yPosBottomLine - yPosHeadLine);

    int areaHeight = yPosBottomLine - yPosHeadLine;

    display.setFont(&CZPTSansRegular9);
    int _thFont = getTextHeightCZ("A", &CZPTSansRegular9);
    int rowMin = ICONWIDTH + 2 + (_thFont * 2) + 4;
    // kolik řádků se vejde
    int rows = areaHeight / rowMin;
    if (rows < 1) rows = 1;
    // skutečné rozdělení
    int baseH = areaHeight / rows;
    int remainder = areaHeight % rows;
    Rect rectRow[20] = {0};
    int yy = yPosHeadLine;
    time_t now;
    time(&now);
    struct tm day;
    localtime_r(&now, &day);    
    String e1, e2;
    for (int i = 0; i < rows; i++)
    {
        int h = baseH + (i < remainder ? 1 : 0);
        rectRow[i].x = 0;
        rectRow[i].y = yy;
        rectRow[i].w = display.width();
        rectRow[i].h = h;
        struct tm tmp = day;
        addDays(tmp, i);
        ical.getEventsForDay(tmp, e1, e2);
        drawRow(rectRow[i], tmp, e1.c_str(), e2.c_str());
        // oddělovací čára (volitelně)
        display.drawLine(0, yy + h, display.width(), yy + h, GxEPD_BLACK);
        yy += h;
    }
  }
  while(display.nextPage());
}






// ===== SETUP =====
void setup()
{

  Serial.begin(115200);
  delay(1000);
  Serial.println("=== START ===");
  SPI.begin(12,-1,11,CS_PIN);
  Serial.println("SPI OK");
  display.init(115200);
  display.setRotation(1);
  Serial.println("Display OK");

  // WIFI
  Serial.println("Connecting WiFi...");
  WiFi.begin(ssid,password);
  int wifiTry=0;
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
    wifiTry++;
    if(wifiTry>20){
      Serial.println("\nWiFi FAIL");
      break;
    }
  }

  if(WiFi.status()==WL_CONNECTED){
    Serial.println("\nWiFi OK");
  }

  Serial.println("Sync time...");
  MoonCalc::initTime();
  while (!getLocalTime(&timeinfo)) delay(200);

  int y=timeinfo.tm_year+1900;
  int m=timeinfo.tm_mon+1;
  int d=timeinfo.tm_mday;

  int h=timeinfo.tm_hour;
  int min=timeinfo.tm_min;

  Serial.print("Date: ");
  Serial.print(d); Serial.print(".");
  Serial.print(m); Serial.print(".");
  Serial.println(y);

  Serial.print("Time: ");
  Serial.print(h); Serial.print(":");
  Serial.println(min);

  String sr, ss;
  CalcSunFromAPI(LAT, LON, sr, ss);

  Serial.print("Sunrise: "); Serial.println(sr);
  Serial.print("Sunset: "); Serial.println(ss);

  String rise, set;

  if (MoonCalc::compute(LAT, LON, rise, set)) {

    Serial.print("Moonrise: ");
    Serial.println(rise);

    Serial.print("Moonset: ");
    Serial.println(set);

  } else {
    Serial.println("No rise/set today");
  }  

  // ICS
  Serial.println("Downloading ICS...");
  if (ical.fetchICS(ICS_URL)) {
    Serial.println("ICS stažen");
  } else {
    Serial.println("Chyba stahování");
  }


  Serial.println("Drawing...");
  drawAll(timeinfo);
  Serial.println("DONE");
  display.hibernate();
}


void loop()
{
}