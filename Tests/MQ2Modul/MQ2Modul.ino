// Define the digital pin for the MQ2 sensor
const int mq2 = D2;  // Connect DO of MQ2 to GPIO4 (D2 on NodeMCU)

void setup() {
    pinMode(mq2, INPUT);  // Set the digital pin as input
    Serial.begin(9600);           // Start serial communication at 19200 baud
    Serial.println("MQ2 Digital Output Test on ESP8266");
    delay(1000);                   // Allow the sensor to warm up
}

void loop() {
    // Read the digital output from the MQ2 sensor
    int smoke = digitalRead(mq2);

    Serial.println(smoke);
    
    if(smoke == LOW){
      //take gps coordinates and send to server.
      Serial.println("Smoke Detected , Coordinates send,");
    }

    // Delay for stability
    delay(1000);  // 1-second delay
}
