# Monitoring Traffic Using Arduino

This project aims to monitor traffic in a given area using the Arduino platform. The area's density is assessed using Arduino sensors, which determine the frequency of people walking by, as well the area's sound intensity.

## Overview and Features
Main features:
- Measuring the frequency of passersby.
- Determining the relative sound intensity of the area.
- Measuring the area's vibrations.

## Getting Started

### Software Requirements
- Arduino IDE
- VS Code (or equivalent)
- Arduino IDE libraries:
   - Wire.h
   - MPU6050.h
   - SPI.h
   - SD.h
- VS Code libraries:
   - Pandas
   - Matplotib


### Hardware
- HC-SR04 Ultrasonic Sensor
- KY-038 Microphone Sound Sensor
- MPU6050 Accelerometer and Gyroscope
- SD Card
- SD Card Module
- Arduino Uno
- Breadboard
- Jumper wires

## Installation
```bash
git clone https://github.com/melissanghia/Traffic-Monitor-Arduino-MakerSPACE.git
cd melissanghia
```

### Step 1: Install necessary libraries in the Arduino IDE
1. Click on Sketch > Include Library > Manage Libraries...;
2. In the search field, look for the following libraries;
3. Click on Install.

### Step 2: Import code for data logging
1. Import or copy/paste [data logging code](/code/datalogging_sketch.ino) into the Arduino IDE.

### Step 3: Circuit diagram
- Replicate the circuit diagram:
![Circuit](/images/circuit_diagram.png?raw=true)

### Wiring instructions
| Arduino Pin | Connected To | 
|-------------|--------------|
| Pin 2       | Trig  (Ultrasonic sensor)| 
| Pin 3       | Echo  (Ultrasonic sensor)| 
| Pin 8      | DO (Sound sensor)          | 
| Pin 10       | CS  | 
| Pin 11       | MOSI  | 
| Pin 12       | MISO  | 
| Pin 13       | SCK  | 
| A4      | SDA (Accelerometer) | 
| A5     | SCL  (Accelerometer)| 


## Code Explanation Part 1: Data logging
Refer to [data logging sketch](/code/datalogging_sketch.ino).

1. First, initialize variables, pin modes, as well as the SD card and the MPU6050.
```arduino
// SETUP
void setup() {
  Serial.begin(9600);

  // Sound + Ultrasonic Pins
  pinMode(OUT_PIN, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set up the SD card
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
```

2. The main loop consists of getting the values for distance, sound intensity, and acceleration (in x, y and z) every 100 ms, and logs the data into an CSV document.
- First, as the sound sensor outputs digital values instead of analog, we use an unconventional method to measure the amplitude of sound; we count how many times the sensor detects sound above a threshold value (in a given interval). 
- Then, we read the acceleration (in x, y, z), as well as the distance. Note that <em>measureDistance()</em> is a previously defined function outside the loop that returns the distance.

```arduino
void loop() {
  millisCurrent = millis();

  // ---- SOUND SENSOR ----
  millisElapsed = millisCurrent - millisLast;
  if (digitalRead(OUT_PIN) == LOW) {
    sampleBufferValue++;                  // count how many times sound is detected during the time interval
  }

  if (millisElapsed > TIME_INTERVAL) {   // start of interval
    mpu.getAcceleration(&ax, &ay, &az); // read acceleration data
    distance = measureDistance();       //read distance

  // ---- DATA OUTPUT (data that gets sent into the CSV every 100 ms) ----
  // print data on the Serial monitor
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

    // open file and log data
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
```
## Code Explanation Part 2: Data Analysis






## Usage Instructions

1. [Step 1]
2. [Step 2]
3. [Step 3]
4. [Additional steps...]

## Troubleshooting

- **Issue 1**: [Solution]
- **Issue 2**: [Solution]
- **Issue 3**: [Solution]

## Future Improvements

- [Potential improvement 1]
- [Potential improvement 2]
- [Potential improvement 3]

## References

- [Reference 1]
- [Reference 2]
- [Additional references...] 


## Resources

- [Git Documentation](https://git-scm.com/doc)
- [GitHub Guides](https://guides.github.com/)
- [Arduino Reference](https://www.arduino.cc/reference/en/)
- [Dawson College Virtual Lab for Arduino Interns](https://englab.dawsoncollege.qc.ca/interns/Intro.html) 

## Acknowledgments

- Person or resource 1
- Person or resource 2
- Inspiration 