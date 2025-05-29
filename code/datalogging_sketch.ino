#include <Wire.h>
#include <MPU6050.h>
#include <SPI.h>
#include <SD.h>

MPU6050 mpu;

// time interval between measurements
const int TIME_INTERVAL = 100; // ms

// sound sensor variables
const int OUT_PIN = 8;

unsigned long millisCurrent;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;
int sampleBufferValue = 0;
float maxsound = 40000;

// ---- Ultrasonic Sensor Variables ----
const int trigPin = 2;
const int echoPin = 3;
float distance = 0;
float maxdistance = 170.00;

// ---- MPU6050 Variables ----
int16_t ax, ay, az;

// SD card chip select pin
const int chipSelect = 10;
// File for data logging
File dataFile;
String fileName = "mtest.CSV";



void setup() {
  Serial.begin(9600);

  // Sound + Ultrasonic Pins
  pinMode(OUT_PIN, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    // Don't proceed if SD card initialization fails
    while (1)
      ;
  }
  Serial.println("SD card initialized successfully.");

  // Create/open data file and write header
  dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    // If this is a new file, write the header
    if (dataFile.size() == 0) {
      dataFile.println("Time(ms),Distance(cm),Sound,AX,AY,AZ");
    }
    dataFile.close();
    Serial.println("Data file ready.");}
    else {
    Serial.println("Error opening data file!");
    }
  // MPU6050 Setup
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
  }
  Serial.println("starting");

}


void loop() {
  millisCurrent = millis();

  // ---- SOUND SENSOR ----
  millisElapsed = millisCurrent - millisLast;
  if (digitalRead(OUT_PIN) == LOW) {
    sampleBufferValue++;                  // count how many times sound is detected during the time interval
  }

  // ---- DATA OUTPUT (every SAMPLE_TIME) ----
  if (millisElapsed > TIME_INTERVAL) {   // end of sound interal
    mpu.getAcceleration(&ax, &ay, &az); // Read accel data
    distance = measureDistance();       //read distance


    Serial.print("Sound:");
    Serial.print(maxsound-sampleBufferValue);
    Serial.print("\tDistance:");
    Serial.print(distance);
    Serial.print("\tAX:");
    Serial.print(ax);
    Serial.print("\tAY:");
    Serial.print(ay);
    Serial.print("\tAZ:");
    Serial.println(az);
    // Open file and log data
    dataFile = SD.open(fileName, FILE_WRITE);

    if (dataFile) {
      dataFile.print(millisCurrent);
      dataFile.print(",");
      dataFile.print(distance);
      dataFile.print(",");
      dataFile.print(maxsound-sampleBufferValue);
      dataFile.print(",");
      dataFile.print(ax);
      dataFile.print(",");
      dataFile.print(ay);
      dataFile.print(",");
      dataFile.println(az);
      dataFile.close();
    }
    else {
      Serial.println("Error opening data file!");} 

    sampleBufferValue = 0;
    millisLast = millisCurrent;
  }


}

// ---- Distance Function ----
float measureDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2.0;
  if (distance > 175 || distance < 0) return maxdistance;
  return distance;
}