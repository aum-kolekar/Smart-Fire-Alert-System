#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "Aum";           // Replace with your Wi-Fi SSID
const char* password = "12345678";  // Replace with your Wi-Fi password
const char* googleApiKey = "AIzaSyCGTb-Zmf_wOC2yXb7eUEdXQaUezUEGvEw";  // Replace with your API key

WiFiServer server(80);
String lastAddress = "Fetching...";  // Store the retrieved address

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\nConnecting to WiFi...");
    WiFi.begin(ssid, password);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        server.begin();
    } else {
        Serial.println("\nFailed to connect to WiFi. Check credentials.");
        return;
    }

    getAddress(37.7749, -122.4194);  // Test with San Francisco coordinates
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client Connected.");
        String request = client.readStringUntil('\r');
        client.flush();

        // Send HTTP response
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("<html><body><h2>Location Address</h2>");
        client.println("<p>" + lastAddress + "</p>");
        client.println("</body></html>");
        delay(10);
        client.stop();
    }
}

void getAddress(float latitude, float longitude) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        HTTPClient http;
        client.setInsecure();  // Ignore SSL verification (only for testing)

        String url = "https://maps.googleapis.com/maps/api/geocode/json?latlng=" 
                     + String(latitude, 6) + "," + String(longitude, 6) 
                     + "&key=" + googleApiKey;

        Serial.println("\nRequesting: " + url);
        http.begin(client, url);
        
        int httpCode = http.GET();
        Serial.print("HTTP Response Code: ");
        Serial.println(httpCode);

        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Response length: " + String(payload.length()));
            Serial.println("Response:");
            Serial.println(payload);

            // Extract the address from JSON response
            int startIndex = payload.indexOf("\"formatted_address\" : \"") + 23;
            int endIndex = payload.indexOf("\",", startIndex);
            if (startIndex > 22 && endIndex > startIndex) {
                lastAddress = payload.substring(startIndex, endIndex);
            } else {
                lastAddress = "Address Not Found";
            }
        } else {
            Serial.println("Failed to get response. Possible reasons:");
            Serial.println("- Incorrect API key");
            Serial.println("- Network issues");
            Serial.println("- Reached API request limit");
            lastAddress = "Error Fetching Address";
        }

        http.end();
    } else {
        Serial.println("WiFi Not Connected! Unable to fetch address.");
        lastAddress = "No WiFi Connection";
    }
}
