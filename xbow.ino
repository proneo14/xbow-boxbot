#include <WiFi.h>
#include <Stepper.h>
#include <ESP32Servo.h>

//////////////////////////
// Wi-Fi settings
//////////////////////////
const char* ssid = "ESP32_Server";
const char* password = "12345678";
WiFiServer server(80);

//////////////////////////
// Stepper settings
//////////////////////////
const int stepsPerRevolution = 2048; // 28BYJ-48
Stepper motor1(stepsPerRevolution, 19, 5, 18, 21);
Stepper motor2(stepsPerRevolution, 22, 2, 4, 23);
Stepper motor3(stepsPerRevolution, 26, 25, 27, 33);


#define DEADZONE 20
#define STEP_DELAY 2 // ms between steps

//////////////////////////
// Global joystick value
//////////////////////////
int joystickX = 0;
int joystickY = 0;

int button = 0;

Servo myServo;
int servoPin = 15;   // PWM pin
bool servoActive = false;
unsigned long servoStartTime = 0;



WiFiClient client;

void setup() {
  Serial.begin(115200);

  motor1.setSpeed(10); // rpm
  motor2.setSpeed(10); // rpm
  motor3.setSpeed(10); // rpm


  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.begin();

  myServo.attach(servoPin);
  myServo.write(90); // start at 0 degrees
}

void loop() {
  // Accept new client if not connected
  if (!client || !client.connected()) {
    client = server.available();
  }

  // Non-blocking read from client
  if (client && client.connected() && client.available()) {
    String data = client.readStringUntil('\n');
    int comma1 = data.indexOf(',');
    int comma2 = data.lastIndexOf(',');

    if (comma1 > 0 && comma2 > comma1) {
      joystickX = data.substring(0, comma1).toInt();
      joystickY = data.substring(comma1 + 1, comma2).toInt();
      button = data.substring(comma2 + 1).toInt();
      // Y and button can be stored if needed
    }
    Serial.printf("Received X: %d, Y: %d, Button: %d\n", joystickX, joystickY, button);
  }

  // Continuous motor control
  if (joystickX > DEADZONE) {
    motor1.step(1); // step more than 1 for visible motion
    motor2.step(1);
    delay(STEP_DELAY);
  } else if (joystickX < -DEADZONE) {
    motor1.step(-1);
    motor2.step(-1);
    delay(STEP_DELAY);
  }

  if (joystickY > DEADZONE) {
    motor3.step(-1); // step more than 1 for visible motion
    delay(STEP_DELAY);
  } else if (joystickY < -DEADZONE) {
    motor3.step(1); // step more than 1 for visible motion
    delay(STEP_DELAY);
  }

// Fire logic for continuous servo
  if (button == 1 && !servoActive) {
    myServo.write(0);  // spin full speed
    servoActive = true;
    servoStartTime = millis();
    Serial.println("Servo firing (spin)...");
  }

  if (servoActive && millis() - servoStartTime > 775) {
    myServo.write(90);  // stop
    servoActive = false;
    Serial.println("Servo stopped");
  }

  // else motor stops
}
