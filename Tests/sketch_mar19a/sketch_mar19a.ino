#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define RXpin 13
#define TXpin 15

double Latitude;
double Longitude;

SoftwareSerial gpsSerial(RXpin, TXpin);
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  Serial.println("wating for gps signal...");
}

void loop() {
  while(gpsSerial.available()){
    if(!gpsSerial.available()){
    Serial.println("No Signal...");
    }
    gps.encode(gpsSerial.read());
  }

  

  if(gps.location.isUpdated()){
      Latitude = gps.location.lat();
      Longitude = gps.location.lng();
      Serial.print("Latitude : ");
      Serial.println(Latitude, 6);
      Serial.print("Longitude : ");
      Serial.println(Longitude, 6);
  }

  delay(2000);
}
