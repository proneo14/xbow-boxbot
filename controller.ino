#define VRx 34   // ADC1 pin (X-axis)
#define VRy 35   // ADC1 pin (Y-axis)
#define Button 32

#include <WiFi.h>

const char* ssid = "ESP32_Server";
const char* password = "12345678";

const char* host = "192.168.4.1";  // Default IP of the AP
WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(Button, INPUT_PULLUP);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  if (client.connect(host, 80)) {
    Serial.println("Connected to server!");
  } else {
    Serial.println("Connection failed!");
  }
}

const int samples = 10; // Number of analog readings to average

//////////////////////////
// Helper function: average multiple reads
//////////////////////////
int readJoystick(int pin) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delay(1); // tiny delay between reads
  }
  return sum / samples;
}

void loop() {
  if (!client.connected()) {
    client.connect(host, 80);
  }

// Read joystick
  int rawX = readJoystick(VRx);
  int rawY = readJoystick(VRy);
  int launchButton = !digitalRead(Button);

  // Map to -100 to +100
  int x = map(rawX, 0, 4095, -100, 100);
  int y = map(rawY, 0, 4095, -100, 100);


  // Debug output
  Serial.printf("Joystick -> X: %d, Y: %d, Button: %d\n", x, y, launchButton);
  // send to client
  client.printf("%d,%d,%d\n", x, y, launchButton);
  delay(100);
}
