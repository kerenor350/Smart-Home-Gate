Smart Home Gate

Smart gate system developed with Arduino Uno.

## Features

**Automatic Gate Control**

Ultrasonic sensor detects objects within 10 cm and opens the gate automatically.

**Bluetooth Control**

O = Open gate

C = Close gate

A = Automatic mode

**Fire Detection**

Flame sensor activates a red LED and displays a fire alert on the LCD for at least 60 seconds.

**Night Mode**

LDR detects low light levels and activates the green LED.

**Temperature Monitoring**

LM35 measures temperature and displays it on the LCD.

**LED Indicators**

Green LED indicates night mode.

Yellow LED blinks while the gate is open.

Red LED blinks during closing and indicates fire detection.

## Hardware

| Component | Arduino Pin |
|---|---|
| Ultrasonic Trig | 2 |
| Ultrasonic Echo | 3 |
| Green LED | 6 |
| Red LED | 7 |
| Yellow LED | 8 |
| Servo Motor | 9 |
| LDR | A0 |
| LM35 | A1 |
| Flame Sensor | A2 |
| LCD I2C | A4 A5 |
| Bluetooth HC05 | RX TX |

## Settings

| Parameter | Value |
|---|---|
| Detection Distance | 10 cm |
| Gate Delay | 2 seconds |
| Night Threshold | 400 |
| Fire Threshold | 1000 |
| Fire Alert Duration | 60 seconds |
| Bluetooth Baud Rate | 9600 |

## Technologies

Arduino Uno

C++

Embedded Systems

Bluetooth Communication

Sensor Integration

## Source Code

`Smart_Gate.ino`
