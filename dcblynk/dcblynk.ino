#define BLYNK_TEMPLATE_ID "TMPL3lvq-_QJV"
#define BLYNK_TEMPLATE_NAME "Simple Pendulum"
// #define BLYNK_AUTH_TOKEN "iczsda6Yg68zdyGPYHkLSEs3T40gXkzp"
#define BLYNK_FIRMWARE_VERSION "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#include "BlynkEdgent.h"
#include "WiFi.h"
#include <Stepper.h>
#include "ESP32Servo.h"

#define TIME_PERIOD_PIN V1
#define ACC_G V3
#define IR_PIN 4
#define SERVO_PIN 2

const int ENA = 14; // Enable pin for motor A
const int IN1 = 27; // Input 1 pin for motor A
const int IN2 = 26; // Input 2 pin for motor A
const int STEP_DELAY = 0;

int flag_dc = 0;
int blynk_flag = 0;
boolean flag = true;

const char* ssid = "Ulog";
const char* password = "cherryblossom";

int length = 15;
int on_off;
int angle;

int ind = 0;
int ignore_count;
int total_count;
unsigned long pretime;
unsigned long currtime;
double sum = 0;
double timePeriod = 0;
double acc_g = 0;
const int arr_size = 5;
double SensorValues[arr_size];
// int sum = 0;

Servo servo;

BLYNK_WRITE(V0)
{
  on_off = param.asInt();
  if(on_off == 1)
  {
    Serial.println("ON");
  }
  else
  {
    Serial.println("OFF");
    rotateAntiClockwiseServo();
  }
}

BLYNK_WRITE(V2)
{
  // length = param.asInt();
  if(on_off == 0)
  {
    Serial.println("Length of string: " + String(length) + "cm");
    // code for servo
    rotateClockwiseServo();
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Set initial motor state (stopped)
  digitalWrite(ENA, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  servo.attach(2);

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //show ip address when connected on serial monitor.
  
  pinMode(IR_PIN, INPUT);
  pretime = micros();
  BlynkEdgent.begin();
}

void rotateClockwise() {
  digitalWrite(ENA, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(0);
}

void stopDC() {
  digitalWrite(ENA, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(0);
}

void rotateAntiClockwise() {
  digitalWrite(ENA, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(0);
}

void rotateClockwiseServo() {
  angle = (length)*18;
  servo.write(angle);
  delay(1000);
}

void rotateAntiClockwiseServo() {
  angle = (length)*18;
  servo.write(-angle);
  delay(1000);
}

void loop()
{
  BlynkEdgent.run();
  delay(10);

  if(on_off == 1)
  {
    delay(100);
     if(flag_dc == 0)
    {
      rotateAntiClockwise();
      delay(60);
      flag_dc = 1;
      // Serial.println("Clockwise");
      // stopDC();
      // delay(1000);
      // rotateAntiClockwise();
      // Serial.println("Anti-Clockwise");
      // delay(40);
    }
    digitalWrite(ENA, LOW);
  }

  if(on_off == 0)
  {
    flag_dc = 0;
    ind = 0;
    ignore_count = 2;
    total_count = 5;
    sum = 0;
    blynk_flag = 0;
  }
  
  if(on_off == 1 && blynk_flag == 0) {
    while(total_count > 0) {
      if(digitalRead(IR_PIN) == 0) {
        // if(ignore_count > 0) {
        //   ignore_count--;
        //   // total_count--;
        //   // Serial.println(timePeriod/1000);
        // }
        // else {
          if(flag == true) {
            currtime = micros();
            timePeriod = currtime - pretime;
            pretime = currtime;
            flag = !flag;
            Serial.println("Time Period(in ms) count:");
            Serial.println(timePeriod/1000);
            total_count--;
            sum = sum + timePeriod/1000;
            SensorValues[ind] = timePeriod/1000;
            // Serial.println(sum, 2);
            // Serial.println(ind);
            ind++;
            delay(100);
          } else {
            flag = !flag;
            delay(100);
          }
        }
      // }
    }
    // Serial.println(SensorValues[0]);
    // Serial.println(Sum);
    sum = sum - SensorValues[0];
    ind = ind - 1;
    double time_period = sum/ind;
    Serial.println("Time Period");
    Serial.println(time_period);
    // // Serial.println(time_period);
    // if(on_off == 1) {
    //   if (digitalRead(IR_PIN) == 0) {
    //     if (flag == true) {
    //       currtime = micros();
    //       timePeriod = currtime - pretime;
    //       pretime = currtime;
    //       flag = !flag;
    //       Serial.println(timePeriod / 1000);
    //       delay(100);
    //     } else {
    //       flag = !flag;
    //       delay(100);
    //     }
    //   }
    // }
    // Serial.println(time_period);
    acc_g = (4*PI*PI*length*10000)/(time_period*time_period);
    
    Blynk.virtualWrite(TIME_PERIOD_PIN, time_period);
    Blynk.virtualWrite(ACC_G, acc_g);
    blynk_flag = 1;
  }
  // if(on_off == 1) {
  //   if (digitalRead(IR_PIN) == 0) {
  //     if (flag == true) {
  //       currtime = micros();
  //       timePeriod = currtime - pretime;
  //       pretime = currtime;
  //       flag = !flag;
  //       Serial.println(timePeriod/1000);
  //       timePeriod = timePeriod/1000;
  //       delay(100);
  //       sum = sum + timePeriod;
  //       acc_g = (4*PI*PI*length*10000)/(timePeriod*timePeriod);
  //       Blynk.virtualWrite(TIME_PERIOD_PIN, timePeriod);
  //       Blynk.virtualWrite(ACC_G, acc_g);
  //     } else {
  //       flag = !flag;
  //       delay(100);
  //     }
  //   }
  // }
}