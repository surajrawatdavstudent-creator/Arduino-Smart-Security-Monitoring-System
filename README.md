# Smart Security & Environment Monitoring System

An Arduino project that combines a keypad security system with temperature, humidity and distance monitoring. The Arduino sends the readings to Python, Python saves them in a CSV file, and another Python script creates an Excel dashboard.

## Project Status

**Working system:** Arduino + keypad + LCD + DHT11 + HC-SR04 + LEDs + buzzer + Python serial logging + CSV + Excel dashboard.

The image section can be updated later with the latest project photos.

## What I Built

This project started as an Arduino security system. I then connected it with Python so that the data is not only shown on the LCD but also saved for later use.

The full flow is:

```text
Arduino hardware
      ↓
PIN authentication
      ↓
Temperature + humidity + distance
      ↓
Serial communication
      ↓
Python
      ↓
CSV data file
      ↓
Excel dashboard + charts
```

This makes the project more than a simple sensor display. It is a small system where hardware, code and data handling work together.

## Main Features

- PIN-based access control using a 4×4 keypad
- 16×2 parallel LCD display
- Green LED for correct access
- Red LED for wrong access
- Buzzer feedback for access results
- DHT11 temperature and humidity readings
- HC-SR04 distance readings
- Arduino serial communication
- Python data logging
- CSV file for saved readings
- Automatic Excel dashboard generation
- Access result chart
- Sensor reading chart
- System can be locked again using `*`

## Hardware Used

| Component | Quantity |
|---|---:|
| Arduino Uno | 1 |
| 16×2 parallel LCD | 1 |
| 4×4 matrix keypad | 1 |
| DHT11 | 1 |
| HC-SR04 | 1 |
| Green LED | 1 |
| Red LED | 1 |
| 5V buzzer | 1 |
| Breadboard | 1 |
| Jumper wires | As required |
| 220Ω resistors | 2 |
| LCD contrast potentiometer | 1 |

## Final Pin Map

| Part | Arduino pin |
|---|---|
| Red LED | D2 |
| Green LED | D3 |
| DHT11 data | D4 |
| Buzzer | D5 |
| HC-SR04 Echo | D6 |
| HC-SR04 Trig | D7 |
| LCD RS | D8 |
| LCD E | D9 |
| LCD D4 | D10 |
| LCD D5 | D11 |
| LCD D6 | D12 |
| LCD D7 | D13 |
| Keypad R1 | A0 |
| Keypad R2 | A1 |
| Keypad R3 | A2 |
| Keypad R4 | A3 |
| Keypad C1 | A4 |
| Keypad C2 | A5 |
| Keypad C3 | D0 |
| Keypad C4 | D1 |

See [`docs/pinout.md`](docs/pinout.md) for the same map with the important D0/D1 note.

## Security System

The current PIN is:

```text
111
```

The physical keypad key `4` is used as Enter/Submit.

### Correct PIN

When `111` is entered and `4` is pressed:

1. Green LED turns on.
2. Red LED stays off.
3. Buzzer sounds for about 2 seconds.
4. LCD shows `ACCESS GRANTED`.
5. The system starts the sensor display.
6. Python receives an `ACCESS_GRANTED` message.

### Wrong PIN

If another PIN is submitted:

1. Red LED turns on.
2. Green LED stays off.
3. Buzzer sounds for about 5 seconds.
4. LCD shows `ACCESS DENIED`.
5. Python receives an `ACCESS_DENIED` message.
6. The system returns to the PIN screen.

### Other keys

- `*` clears the PIN while the system is locked.
- `*` locks the system again after access is granted.
- Number keys are used to enter the PIN.

## Sensor Monitoring

After successful login, the LCD cycles through three screens:

### Temperature

The DHT11 reads temperature in °C.

### Humidity

The DHT11 reads humidity as a percentage.

### Distance

The HC-SR04 measures distance in cm. The code takes several readings and uses the valid readings to reduce random changes.

## Python Part

The Arduino sends simple text messages through serial communication.

Examples:

```text
SYSTEM_STARTED
ACCESS_GRANTED
TEMPERATURE:26.0
HUMIDITY:57.8
DISTANCE:10.3
```

Python reads these messages and waits until a complete granted-access record is available.

For a denied attempt, Python saves the access result immediately.

The main Python file is:

```text
python/arduino_python.py
```

## CSV Data

The saved data uses this format:

```text
Date,Time,Access,Temperature_C,Humidity_percent,Distance_cm
```

Example structure:

```text
2026-10-24,23:52:09,GRANTED,24.3,55.3,11.7
```

The real CSV file is generated while the program is running. It is kept separate from the Arduino code so the data can be used again for analysis.

## Excel Dashboard

The second Python file is:

```text
python/update_dashboard.py
```

It reads the CSV file and creates:

```text
dashboard/Smart_Security_Auto_Dashboard.xlsx
```

The dashboard includes:

- Total records
- Granted attempts
- Denied attempts
- Latest temperature
- Latest humidity
- Latest distance
- Access result chart
- Sensor reading chart

The Excel file is generated from the CSV, so it does not need to be edited by hand every time new data is added.

## How To Run It

### 1. Arduino

Open:

```text
Smart_Security_System.ino
```

Install these Arduino libraries if they are not already installed:

- LiquidCrystal
- Keypad
- DHT sensor library

Upload the code to the Arduino Uno.

Because the keypad uses D0 and D1, disconnect those two keypad wires during upload if the Arduino IDE cannot upload the program. Connect them again after the upload.

### 2. Python

Install the requirements:

```text
pip install -r requirements.txt
```

Check the COM port in `python/arduino_python.py` if your Arduino is not on `COM4`.

Before running the Python program, close the Arduino Serial Monitor because Python needs the serial connection.

Run:

```text
python python/arduino_python.py
```

### 3. Update the Excel dashboard manually

The logger already runs the dashboard updater after saving a record. It can also be run directly:

```text
python python/update_dashboard.py
```

## Project Structure

```text
Arduino-Smart-Security-Monitoring-System/
│
├── Smart_Security_System.ino
├── README.md
├── requirements.txt
├── .gitignore
│
├── python/
│   ├── arduino_python.py
│   └── update_dashboard.py
│
├── data/
│   └── arduino_data.csv
│
├── dashboard/
│   └── README.md
│
├── docs/
│   └── pinout.md
│
├── LICENSE
│
├── 1.Overview.jpg
├── 2.lcd_interface.jpg
├── 3.Temperature.jpg
├── 4.Distance.jpg
├── 5.humidity.jpg
└── 6-explanation.mp4
```

The existing images and video can stay in the repository. New photos can be added later when the final setup is ready.

## Testing And Debugging

This project needed a lot of testing because several components had to work together at the same time.

Some of the main problems were:

- LCD text becoming random
- Keypad input affecting the rest of the system
- HC-SR04 giving no echo at very short distances
- DHT11 readings not appearing when the full system was connected
- Arduino serial communication using the same D0/D1 pins as part of the keypad
- Python and Excel needing the correct file paths
- Old dashboard files causing confusion about which data was being opened

Testing the components separately and then joining them together helped find these problems.

## What I Learned

- How to connect several Arduino components in one project
- How to use a 4×4 keypad for a real input system
- How to control a parallel 16×2 LCD
- How to read DHT11 and HC-SR04 sensors
- How Arduino serial communication works
- How Python can read Arduino data
- How to save sensor data in CSV format
- How Python can create and update Excel files
- How charts can make sensor data easier to understand
- How to debug a project when several hardware parts are connected together

## Future Ideas

Possible next steps are:

- Add better access history analysis
- Add sensor limits and warnings
- Add more detailed Excel charts
- Add a web dashboard
- Add a database for long-term records
- Build a proper enclosure for the hardware
- Improve the wiring and make a cleaner final hardware setup

These are future improvements. The current Arduino + Python + CSV + Excel system is already working as one complete project.

## About This Project

I built this project as a hands-on electronics and programming project. The main goal was not only to make the sensors work, but to connect the hardware with Python and use the collected data in a useful way.
