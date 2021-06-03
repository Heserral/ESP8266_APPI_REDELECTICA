#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "ESP8266HTTPClient.h"
#include <Arduino_JSON.h>

#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ) )

const char *ssid     = "XXXXXXXXXXX";
const char *password = "XXXXXXXXXXXXXXXX";

const String serverName = "https://api.esios.ree.es/indicators/10391/";
const String token ="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200); //7200=+2h España

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

String getFormattedDate(byte plushour) {
  unsigned long rawTime =  timeClient.getEpochTime() / 86400L;  // in days
  unsigned long days = 0, year = 1970;
  uint8_t month;
  static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

  while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
    year++;
  rawTime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
  days=0;
  for (month=0; month<12; month++) {
    uint8_t monthLength;
    if (month==1) { // february
      monthLength = LEAP_YEAR(year) ? 29 : 28;
    } else {
      monthLength = monthDays[month];
    }
    if (rawTime < monthLength) break;
    rawTime -= monthLength;
  }
  String monthStr = ++month < 10 ? "0" + String(month) : String(month); // jan is month 1  
  String dayStr = ++rawTime < 10 ? "0" + String(rawTime) : String(rawTime); // day of month  
  return String(year) + "-" + monthStr + "-" + dayStr + "T" + (timeClient.getHours()+plushour) + "%3A00%3A00Z";
}

void loop() {
  timeClient.update();

  //2021-06-03T03:00:00Z
  //2021-06-03T03%3A00%3A00Z
  String now_time = getFormattedDate(0);
  String next_time = getFormattedDate(1);

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  //curl 
  //-H "Authorization: Token token=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" 
  //-H "Accept: application/json; application/vnd.esios-api-v2+json" 
  //-v -L "http://api.esios.ree.es/indicators/10391/?start_date=2021-06-03T03%3A00%3A00Z&end_date=2021-06-03T04%3A00%3A00Z&geo_ids[]=8741"
  
  String serverPath = serverName + "?start_date="+now_time+"&end_date="+next_time+"&geo_ids[]=8741";

  //Serial.println(serverPath);
  
  if (http.begin(client,serverPath.c_str())){
    delay(1000);
    http.setAuthorization("");
    http.addHeader("Authorization","Token token=" + token);
    http.addHeader("Accept","application/json; application/vnd.esios-api-v2+json");
    //http.addHeader("Content-Length", "219");
    //http.addHeader("User-Agent","Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_1) AppleWebKit/537.36 (K HTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36");
     
    //Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
       //Serial.printf("[HTTP] GET... code: %d\n", httpCode);
       if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          //String payload = http.getString();   // Obtener respuesta
          
          JSONVar myObject = JSON.parse(http.getString());
          if (JSON.typeof(myObject) == "undefined") {
            Serial.println("Parsing input failed!");
            return;
          }

          JSONVar pvpc_data = myObject["indicator"]["values"];

          /*
            for(int i=0; i< pvpc_data.length();i++){
              JSONVar keys = pvpc_data[i].keys();
              for (int n = 0; n < keys.length(); n++) {
                Serial.print(keys[n]);
                Serial.println( pvpc_data[i][keys[n]]);
              }
             }
              "value": 109.58,
              "datetime": "2021-06-03T00:00:00.000+02:00",
              "datetime_utc": "2021-06-02T22:00:00Z",
              "tz_time": "2021-06-02T22:00:00.000Z",
              "geo_id": 8741,
              "geo_name": "Península"
           */
           
          for(int i=0; i< pvpc_data.length();i++){
            JSONVar keys = pvpc_data[i].keys();
            Serial.print(pvpc_data[i][keys[1]]);
            Serial.print("   ");
            Serial.println( (double)(pvpc_data[i][keys[0]])/1000,4);
          }
       }
    }
    else {
       Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
   


  
  delay(10000);
}
