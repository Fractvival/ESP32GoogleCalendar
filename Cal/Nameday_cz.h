#ifndef NAMEDAY_CZ_H
#define NAMEDAY_CZ_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

String getNameDayCZ(int day, int month, int year) 
{
  HTTPClient http;
  String url = "https://svatkyapi.cz/api/day/";
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode != 200) 
  {
    http.end();
    return "Chyba API";
  }
  String payload = http.getString();
  http.end();
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) 
  {
    return "JSON chyba";
  }
  if (doc.containsKey("name")) 
  {
    return String((const char*)doc["name"]);
  }
  if (doc.containsKey("svatek")) 
  {
    return String((const char*)doc["svatek"]);
  }
  return "Neznámý";
}

#endif