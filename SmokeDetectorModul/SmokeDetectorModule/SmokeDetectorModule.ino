#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

#define RXPin 13   // GPS TX -> NodeMCU RX
#define TXPin 15   // Not used for GPS
#define MQ2Pin D2  // MQ2 Sensor connected to GPIO4

const char* ssid = "Aum";               // WiFi SSID
const char* password = "12345678";      // WiFi Password
const char* serverUrl = "http://192.168.178.204:5000/get_address";  // Flask Server URL

SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
    Serial.begin(9600);
    gpsSerial.begin(9600);
    
    pinMode(MQ2Pin, INPUT);

    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
}

void loop() {
    int mq2Value = digitalRead(MQ2Pin);
    Serial.print("MQ2 Sensor Value: ");
    Serial.println(mq2Value);

    if (mq2Value == LOW) {  // If smoke is detected
        Serial.println("WARNING: High gas/smoke detected!");
        String gpsData = readGPS();
        if (gpsData != "") {
            sendCoordinatesToServer(gpsData);
        }
        else {
          Serial.println("GPS data empty....");
        }
    }

    delay(2000);  // Read sensor every 2 seconds
}

String readGPS() {
    String nmea = "";
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        nmea += c;
        if (c == '\n') break;
    }

    if (nmea.startsWith("$GPGGA")) {  // Process only GPGGA data
        Serial.println("Raw GPS: " + nmea);
        return parseCoordinates(nmea);
    }
    return "";
}

String parseCoordinates(String nmea) {
    char* token = strtok((char*)nmea.c_str(), ",");
    int index = 0;
    String lat, lon;

    while (token != NULL) {
        index++;
        if (index == 3) lat = token;
        if (index == 4 && String(token) == "S") lat = "-" + lat;
        if (index == 5) lon = token;
        if (index == 6 && String(token) == "W") lon = "-" + lon;
        token = strtok(NULL, ",");
    }

    if (lat != "" && lon != "") {
        float latDecimal = convertToDecimal(lat.toFloat());
        float lonDecimal = convertToDecimal(lon.toFloat());
        return String(latDecimal, 6) + "," + String(lonDecimal, 6);
    }
    return "";
}

float convertToDecimal(float value) {
    int degrees = (int)(value / 100);
    float minutes = value - (degrees * 100);
    return degrees + (minutes / 60);
}

void sendCoordinatesToServer(String coords) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        String lat = coords.substring(0, coords.indexOf(","));
        String lon = coords.substring(coords.indexOf(",") + 1);

        String jsonPayload = "{\"latitude\":" + lat + ", \"longitude\":" + lon + "}";
        
        http.begin(client, serverUrl);
        http.addHeader("Content-Type", "application/json");
        
        Serial.println("Sending Data to Server: " + jsonPayload);
        int httpResponseCode = http.POST(jsonPayload);
        
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Server Response: " + response);
        } else {
            Serial.println("HTTP Request failed");
        }

        http.end();
    }
}
