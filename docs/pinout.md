# Final Pin Map

This is the pin map used by the current working version of the project.

## Arduino Uno

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

## Important

D0 and D1 are also the Arduino Uno serial pins. This project uses them for the keypad because that was part of the working hardware setup. When uploading the Arduino program, disconnect the keypad wires from D0 and D1 if the upload does not start.

The LCD is a parallel 16×2 LCD, so no I2C module is used.
