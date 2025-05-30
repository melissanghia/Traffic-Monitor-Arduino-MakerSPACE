# Monitoring Traffic Using Arduino

This project aims to monitor traffic in a given area using the Arduino platform. The area's density is assessed using Arduino sensors, which determine the frequency of people walking by, as well the area's sound intensity.

## Overview and Features
Main features:
- Measuring the frequency of passersby.
- Determining the relative sound intensity of the area.
- Measuring the area's vibrations (with an accelerometer), which helps determine if anynone has tampered with the experiment (when left in a public area).

For a complete example of monitoring traffic using Arduino (at Dawson College), see [data collected on May 9th](/code%20&%20data/data_may9.CSV). Results (graphs of frequency of passersby, sound intensity variations, and acceleration variations) are displayed in [Data analysis](/code%20&%20data/data_analysis_code.ipynb).

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

## Instructions


### Step 1: Install necessary libraries in the Arduino IDE
1. Click on Sketch > Include Library > Manage Libraries...;
2. In the search field, look for the libraries mentioned in <em>Software Requirements</em>;
3. Click on Install.

### Step 2: Install necessary libraries/extensions in VS Code
1. From the <em>Extensions</em> search field, install the librairies mentioned in <em>Software Requirements</em>.

### Step 3: Import code for data logging
1. Import or copy/paste [data logging code](/code/datalogging_sketch.ino) into the Arduino IDE.

### Step 4: Circuit diagram
- Replicate the circuit diagram:

![Circuit](/images/circuit_diagram.png?raw=true)
*Don't forget to insert the SD card.

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

### Step 5: Connect the Arduino to a power source and start data logging!

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
See [full Data Analysis code (with comments)](/code/data_analysis_code.ipynb).

To start data analysis, create a dataframe from the CSV document.
```python
filename = "< Insert file name here>.CSV"
df = pd.read_csv(filename)
```

To determine the frequency of passersby, we count how many times—in a given interval—the distance measured by the sensor is smaller than the distance to the wall.
```python
count_dist = []
time_list = []
start_index = 0
start_time = df.loc[start_index, "Time(ms)"]

# convert the "Time(ms)" column to a numpy array (for speed)
times = df["Time(ms)"].values
distances = df["Distance(cm)"].values

# count how many times the distance is smaller than the width of the hallway (170cm), in a given interval
for row in range(len(df)):
    end_time = times[row]
    
    if end_time - start_time > 300000:  # time interval (in ms): 5 seconds
        window_dist = distances[start_index:row+1]
        counter = (window_dist < 168).sum()
        count_dist.append(counter)
        time_list.append(end_time)
        
        start_index = row+2
        start_time = times[start_index]
```
To get a visual representation of the frequency of passersby:
```python
plt.figure(figsize=(10, 5))  # adjust size of graph
plt.plot(time_list,count_dist)

# add labels and title
plt.xlabel("Time (ms)")
plt.ylabel("Frequency of people walking by")
plt.title("Frequency of people walking by (in a 5s interval)")
plt.show()
```

To determine the sound intensity level, and its variations over the period of the experiment, we proceed in the same way: by counting how many times—in a given interval—the sound level surpasses a threshold value.
```python
count_sound = []
time_list = []
start_index = 0
start_time = df.loc[start_index, "Sound"]

# convert the "Time(ms)" column to a numpy array
sound_values = df["Sound"].values

# count how many tim
for row in range(len(df)):
    end_time = times[row]
    
    if end_time - start_time > 300000:    # time interval: 5 seconds   
        window_sound = sound_values[start_index:row+1]
        counter = (window_sound > 500).sum()    # threshold sound intensity value
        count_sound.append(counter)
        time_list.append(end_time)
        
        start_index = row+2
        start_time = times[start_index]
```
To visualize the sound intensity variations, use the same code for plotting the frequency of passersby.

Again, we proceed the same way for the 3 accelerations (x, y, z). The following code is for the analysis of the acceleration in x only:

First, we find the value that corresponds to no acceleration (which we will call the <em>default</em>) by searching through the dataframe and finding the value that appears most often. Note that the value measured by the sensor that corresponds to no acceleration is not zero (perhaps due to a calibration issue).
```python
default_AX = df['AX'].value_counts().idxmax()
```
We then count how many times—in a given interval—the acceleration measured differs greatly from the default one (i.e., how many times the acceleration detected is very large).
```python
count_AX = []
time_list = []
start_index = 0
start_time = df.loc[start_index, "AX"]

# convert the "Time(ms)" column to a numpy array for speed
AX_values = df["AX"].values

# count how many vibrations in a given interval (i.e., how many times the acceleration detected is very large)
for row in range(len(df)):
    end_time = times[row]
    
    if end_time - start_time > 30000:  # time interval: 5 seconds  
        window_AX = AX_values[start_index:row+1]
        counter = (window_AX < (default_AX-500)).sum()
        counter = counter + (window_AX > (default_AX+500)).sum()
        count_AX.append(counter)
        time_list.append(end_time)
        
        start_index = row+2
        start_time = times[start_index]
```
Use the plotting code mentioned previously to visualize the variations in accelerations throughout the experiment.

## Troubleshooting

- **No sound detected by the sound sensor**: adjust the sensitivity or threshold at which the sensor detects sound by turning a small screw on the sensor module itself.

![Potentiometer](/images/potentiometer.png?raw=true)

- **No change in distance was observed on the serial monitor when an object/person has moved in front the ultrasonic sensor**: in the Arduino IDE sketch, adjust the sample rate to a lower value (i.e., print the distance at a quicker rate).


## Future Improvements
- Changing the KY-038 Microphone Sound Sensor to a sensor with a wider range. *KY-038 only detects sound in a 50 cm range.
- Incorporating a temperature or humidity sensor, to observe their variations as traffic in the area varies.

## References and Resources

- [Dawson College Virtual Lab for Arduino Interns](https://englab.dawsoncollege.qc.ca/interns/Intro.html) 
- [KY-038 Sound Sensor Code and Wiring Example](https://sensorkit.joy-it.net/en/sensors/ky-038)
- [MPU6050 Accelerometer Guide](https://lastminuteengineers.com/mpu6050-accel-gyro-arduino-tutorial/)


## Acknowledgments
This experiment was carried out under the guidance of Joel Trudeau as part of the MakerSPACE Internship.