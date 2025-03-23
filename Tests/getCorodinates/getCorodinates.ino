

#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Define GPS module pins
static const int RXPin = 13;  // GPS TX → NodeMCU RX (D7)
static const int TXPin = 15;  // GPS RX → NodeMCU TX (D8)
static const uint32_t GPSBaud = 9600;

double lat = 0.0, lon = 0.0;  // Store coordinates

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
    Serial.begin(9600);
    gpsSerial.begin(GPSBaud);
    Serial.println("Initializing GPS...");
}

void getCoordinates() {
    Serial.println("Fetching GPS Coordinates...");
    unsigned long start = millis(); // Timeout mechanism
    while (millis() - start < 10000) {  // Try for 10 seconds
        while (gpsSerial.available()) {
            gps.encode(gpsSerial.read());

            if (gps.location.isUpdated()) {
                lat = gps.location.lat();
                lon = gps.location.lng();
                Serial.println("Coordinates Fetched!");
                return;  // Exit function after getting coordinates
            }
        }
    }
    Serial.println("Failed to get coordinates. Retrying...");
}

void loop() {
    if (lat == 0.0 && lon == 0.0) {
        getCoordinates();  // Fetch only if not available
    } else {
        Serial.print("Stored Coordinates - Lat: ");
        Serial.print(lat, 6);
        Serial.print(" Lon: ");
        Serial.println(lon, 6);
    }
    
    delay(5000);  // Print stored coordinates every 5 seconds
}
