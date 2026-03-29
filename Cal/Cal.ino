#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <SPI.h>
#include <GxEPD2_3C.h>
#include <WiFiClientSecure.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>

#include "Moon.h"
#include "Nameday_cz.h"
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

// ===== WIFI =====
const char* ssid = "Darina";
const char* password = "bublina22";

// ===== ICS =====
String ICS_URL = "https://calendar.google.com/calendar/ical/fractvival%40gmail.com/private-2f207af2f9c073193186f1d706bfbd29/basic.ics";

// ===== BRNO =====
double LAT = 49.195;
double LON = 16.607;

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

const unsigned char clear_day[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x0c, 0x18, 0x30, 0x0e, 
	0x00, 0x70, 0x07, 0x7e, 0xe0, 0x02, 0xff, 0x40, 0x01, 0xe7, 0x80, 0x03, 0x81, 0xc0, 0x03, 0x81, 
	0xc0, 0x7b, 0x00, 0xde, 0x7b, 0x00, 0xde, 0x03, 0x81, 0xc0, 0x03, 0x81, 0xc0, 0x01, 0xe7, 0x80, 
	0x02, 0xff, 0x40, 0x07, 0x7e, 0xe0, 0x0e, 0x00, 0x70, 0x1c, 0x18, 0x30, 0x08, 0x18, 0x00, 0x00, 
	0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00
};

const unsigned char cloudy_day[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x10, 0x0c, 0x18, 0x38, 0x0e, 
	0x00, 0x70, 0x07, 0x7e, 0xe0, 0x02, 0xff, 0x40, 0x01, 0xe7, 0x80, 0x03, 0x81, 0xc0, 0x03, 0x81, 
	0xc0, 0x1f, 0x80, 0xde, 0x3f, 0xc0, 0xde, 0x79, 0xe1, 0xc0, 0x70, 0xf9, 0xc0, 0x60, 0x7f, 0x80, 
	0x60, 0x1f, 0x40, 0x70, 0x0e, 0xe0, 0x78, 0x1c, 0x70, 0x3f, 0xfc, 0x30, 0x1f, 0xf8, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char cloud[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x01, 
	0xff, 0x80, 0x03, 0xc7, 0xc0, 0x03, 0x81, 0xc0, 0x07, 0x00, 0xe0, 0x0f, 0x00, 0xe0, 0x3e, 0x00, 
	0x60, 0x38, 0x00, 0x78, 0x70, 0x00, 0x7c, 0x60, 0x00, 0x1e, 0x60, 0x00, 0x0e, 0x60, 0x00, 0x06, 
	0x70, 0x00, 0x0e, 0x78, 0x00, 0x1e, 0x3f, 0xff, 0xfc, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char rainy[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x07, 
	0x81, 0xc0, 0x1f, 0x81, 0xc0, 0x1c, 0x00, 0xf0, 0x38, 0x00, 0xf8, 0x30, 0x00, 0x3c, 0x30, 0x00, 
	0x1c, 0x30, 0x00, 0x0c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x3c, 0x1f, 0xff, 0xf8, 0x07, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x06, 0x18, 0x60, 0x07, 0x1c, 0x70, 0x07, 0x1c, 0x70, 0x03, 0x8e, 0x38, 0x03, 
	0x8e, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char rainy_heavy[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 
	0x39, 0x80, 0x77, 0x39, 0xc0, 0x73, 0x9d, 0xc0, 0x3b, 0x9c, 0xe0, 0x39, 0xce, 0xe0, 0x1d, 0xce, 
	0x70, 0x1c, 0xe7, 0x70, 0x0e, 0xe7, 0x38, 0x0e, 0x73, 0xb8, 0x07, 0x73, 0x9c, 0x07, 0x39, 0xdc, 
	0x03, 0xb9, 0xce, 0x03, 0x9c, 0xee, 0x01, 0x9c, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char snowing[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 
	0x18, 0x60, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0xc3, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x18, 0x60, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char thunderstorm[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x07, 
	0x81, 0xc0, 0x1f, 0x81, 0xc0, 0x1c, 0x00, 0xf0, 0x38, 0x00, 0xf8, 0x30, 0x00, 0x3c, 0x30, 0x00, 
	0x1c, 0x30, 0x00, 0x0c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x3c, 0x1f, 0xff, 0xf8, 0x07, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x01, 0xc3, 0x80, 0x01, 0xc7, 0x00, 0x01, 0xe7, 0xc0, 0x03, 0xe1, 0x80, 0x00, 
	0xc3, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00
};

const unsigned char foggy[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xe7, 0x80, 0x07, 
	0x81, 0xc0, 0x1f, 0x81, 0xc0, 0x1c, 0x00, 0xf0, 0x38, 0x00, 0xf8, 0x30, 0x00, 0x3c, 0x30, 0x00, 
	0x1c, 0x30, 0x00, 0x0c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x3c, 0x1f, 0xff, 0xf8, 0x07, 0xff, 0xf0, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0x60, 0x07, 0xff, 0x60, 0x00, 0x00, 0x00, 0x03, 0x7f, 0xc0, 0x03, 
	0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct tm timeinfo;


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



#define ICONWIDTH 24

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
    display.drawBitmap(display.width()-24, yPosIcons, moonIcon, ICONWIDTH, ICONWIDTH, GxEPD_BLACK);    

    int y=timeinfo.tm_year+1900;
    int m=timeinfo.tm_mon+1;
    int d=timeinfo.tm_mday;
    int hour = timeinfo.tm_hour;
    int min = timeinfo.tm_min;

    display.setFont(&FreeSans9pt7b);

    String srStr, ssStr;
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

    String rise, set;
    MoonCalc::compute(LAT, LON, rise, set);
    int xPosMoonText = display.width() - ICONWIDTH - spacing;
    display.setTextColor(GxEPD_RED);
    display.setCursor(xPosMoonText-_w2, yPosIcons + _h2);
    display.print(rise);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(xPosMoonText-_w2, yPosIcons + (_h2 * 2) + (_h2 * 40 / 100));
    display.print(set);

    int yPosNamedayText = yPosIcons + (_h2 * 2) + (_h2 * 40 / 100) + ((display.height()*5)/100);

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&CZSpaceMonoRegular12);
    int widthNamedayText = getTextWidthCZ(getNameDayCZ(timeinfo.tm_mday, timeinfo.tm_mon, timeinfo.tm_year).c_str(),&CZSpaceMonoRegular12);
    int heightNamedayText = getTextHeightCZ(getNameDayCZ(timeinfo.tm_mday, timeinfo.tm_mon, timeinfo.tm_year).c_str(),&CZSpaceMonoRegular12);
    int xPosNamedayText = (display.width() - widthNamedayText) / 2;
    display.setCursor(xPosNamedayText, yPosNamedayText);
    printCZ(getNameDayCZ(timeinfo.tm_mday, timeinfo.tm_mon, timeinfo.tm_year).c_str());

    int yPosHeadLine = yPosNamedayText + ((display.height()*5)/100);

    display.drawLine(0, yPosHeadLine - 3, display.width(), yPosHeadLine -3, GxEPD_BLACK);
    display.drawLine(0, yPosHeadLine, display.width(), yPosHeadLine, GxEPD_BLACK);

    String lastSyncText = "";
    lastSyncText += String(hour);
    lastSyncText += ":";
    lastSyncText += String(min);
    lastSyncText += " - ";
    lastSyncText += String(d);
    lastSyncText += "/";
    lastSyncText += String(m);
    lastSyncText += "/";
    lastSyncText += String(y);

    display.setFont(&CZSpaceMonoRegular5);
    int heightSyncText = getTextHeightCZ(lastSyncText.c_str(),&CZSpaceMonoRegular5);
    display.setCursor(0, display.height()-1);
    printCZ(lastSyncText.c_str());

    int yPosBottomLine = display.height() - 1 - heightSyncText - 5;

    display.drawLine(0, yPosBottomLine + 3, display.width(), yPosBottomLine + 3, GxEPD_BLACK);
    display.drawLine(0, yPosBottomLine, display.width(), yPosBottomLine, GxEPD_BLACK);

    Serial.print("Width: ");
    Serial.println(display.width());

    Serial.print("Area height: ");
    Serial.println(yPosBottomLine - yPosHeadLine);

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
  display.setRotation(0);
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
  MoonCalc::CalcSun(LAT, LON, sr, ss);

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
  String ics=downloadICS();

  Serial.print("ICS length: ");
  Serial.println(ics.length());

  if(ics.length()>50){
    Serial.println("Parsing ICS...");
    parseICS(ics,y,m);
    Serial.print("Events: ");
    Serial.println(eventCount);
  } else {
    Serial.println("ICS FAIL -> fallback");
    eventCount=1;
    events[0]={d,"NO DATA"};
  }

  Serial.println("Drawing...");
  drawAll(timeinfo);
  Serial.println("DONE");
  display.hibernate();
}


void loop()
{
}