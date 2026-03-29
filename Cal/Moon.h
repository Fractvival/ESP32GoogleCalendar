#pragma once
#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DEG_TO_RAD 0.017453292519943295
#define RAD_TO_DEG 57.29577951308232

class MoonCalc {

public:

  // ================= TIME =================
  static void initTime()
  {
    //setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    //tzset();
    configTime(3600, 3600, "cz.pool.ntp.org", "pool.ntp.org");

    struct tm t;
    while (!getLocalTime(&t)) delay(200);
  }

  // ================= JULIAN =================
  static double julianDate(int y, int m, int d, double hour)
  {
    if (m <= 2) { y--; m += 12; }
    int A = y / 100;
    int B = 2 - A + A / 4;

    return floor(365.25 * (y + 4716))
         + floor(30.6001 * (m + 1))
         + d + B - 1524.5 + hour / 24.0;
  }

  // ================= MEEUS POSITION =================
  static void moonPosition(double jd, double &ra, double &dec)
  {
    double T = (jd - 2451545.0) / 36525.0;

    double L0 = fmod(218.316 + 481267.881 * T, 360);
    double M  = fmod(134.963 + 477198.867 * T, 360);
    double F  = fmod(93.272  + 483202.017 * T, 360);

    double lon = L0
      + 6.289 * sin(M * DEG_TO_RAD)
      + 1.274 * sin((2*L0 - M) * DEG_TO_RAD)
      + 0.658 * sin(2*L0 * DEG_TO_RAD);

    double lat = 5.128 * sin(F * DEG_TO_RAD);

    double eps = 23.439 - 0.0000004 * (jd - 2451545.0);

    double x = cos(lon*DEG_TO_RAD)*cos(lat*DEG_TO_RAD);
    double y = sin(lon*DEG_TO_RAD)*cos(lat*DEG_TO_RAD);
    double z = sin(lat*DEG_TO_RAD);

    double y2 = y*cos(eps*DEG_TO_RAD) - z*sin(eps*DEG_TO_RAD);
    double z2 = y*sin(eps*DEG_TO_RAD) + z*cos(eps*DEG_TO_RAD);

    ra  = atan2(y2, x);
    dec = asin(z2);
  }

  // ================= ALTITUDE =================
  static double moonAltitude(double jd, double lat, double lon)
  {
    double ra, dec;
    moonPosition(jd, ra, dec);

    double D = jd - 2451545.0;
    double GMST = fmod(280.46061837 + 360.98564736629 * D, 360);

    double LST = (GMST + lon) * DEG_TO_RAD;
    double HA = LST - ra;

    double alt = asin(
      sin(lat*DEG_TO_RAD)*sin(dec) +
      cos(lat*DEG_TO_RAD)*cos(dec)*cos(HA)
    );

    return alt * RAD_TO_DEG;
  }

  // ================= FIND TIMES =================
  static bool compute(double lat, double lon,
                      String &riseStr, String &setStr)
  {
    struct tm t;
    if (!getLocalTime(&t)) return false;

    int y = t.tm_year + 1900;
    int m = t.tm_mon + 1;
    int d = t.tm_mday;

    int tz = t.tm_isdst ? 2 : 1;

    double riseUTC = -1, setUTC = -1;

    double prev = moonAltitude(julianDate(y,m,d,0), lat, lon);

    bool fr=false, fs=false;

    for(double h=0.05; h<=24; h+=0.05) // ~3 min krok
    {
      double alt = moonAltitude(julianDate(y,m,d,h), lat, lon);

      if(!fr && prev<0 && alt>=0)
      {
        riseUTC = h;
        fr=true;
      }

      if(!fs && prev>0 && alt<=0)
      {
        setUTC = h;
        fs=true;
      }

      prev = alt;
    }

    double rise = riseUTC + tz;
    double set  = setUTC  + tz;

    if (rise >= 24) rise -= 24;
    if (set  >= 24) set  -= 24;

    riseStr = toHHMM(rise);
    setStr  = toHHMM(set);

    return true;
  }

  static bool CalcSun(double lat, double lon,
                      String &sunriseStr, String &sunsetStr)
  {
    struct tm t;
    if (!getLocalTime(&t)) return false;

    int y = t.tm_year + 1900;
    int m = t.tm_mon + 1;
    int d = t.tm_mday;

    int tz = t.tm_isdst ? 2 : 1;

    // day of year
    int N1 = floor(275 * m / 9);
    int N2 = floor((m + 9) / 12);
    int N3 = (1 + floor((y - 4 * floor(y / 4) + 2) / 3));
    int N = N1 - (N2 * N3) + d - 30;

    double lngHour = lon / 15.0;

    auto calc = [&](bool sunrise)
    {
      double t0 = N + ((sunrise ? 6 : 18) - lngHour) / 24.0;
      double M = (0.9856 * t0) - 3.289;

      double L = M + 1.916*sin(M*DEG_TO_RAD)
                  + 0.020*sin(2*M*DEG_TO_RAD)
                  + 282.634;

      L = fmod(L, 360);

      double RA = atan(0.91764 * tan(L*DEG_TO_RAD)) * RAD_TO_DEG;
      RA = fmod(RA, 360);

      double sinDec = 0.39782 * sin(L*DEG_TO_RAD);
      double cosDec = cos(asin(sinDec));

      double cosH = (cos(90.833*DEG_TO_RAD) -
                    (sinDec * sin(lat*DEG_TO_RAD))) /
                    (cosDec * cos(lat*DEG_TO_RAD));

      if (cosH > 1 || cosH < -1) return -1.0;

      double H = sunrise ? (360 - acos(cosH)*RAD_TO_DEG)
                        : (acos(cosH)*RAD_TO_DEG);

      H /= 15.0;

      double T = H + RA/15 - (0.06571 * t0) - 6.622;
      double UT = T - lngHour;

      return UT;
    };

    double riseUTC = calc(true);
    double setUTC  = calc(false);

    double rise = riseUTC + tz;
    double set  = setUTC  + tz;

    if (rise < 0) rise += 24;
    if (set  < 0) set  += 24;
    if (rise >= 24) rise -= 24;
    if (set  >= 24) set  -= 24;

    sunriseStr = toHHMM(rise);
    sunsetStr  = toHHMM(set);

    return true;
  }  

  static bool CalcSunFromAPI(double lat, double lon,
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

  // ================= FORMAT =================
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
};