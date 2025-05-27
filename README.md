# Joystick-Controlled Servo Motor using ATmega32

This embedded systems project uses a joystick to control a servo motor’s position and display direction and PWM data on a 16x2 LCD. The joystick's X-axis is read using ADC and mapped to control the servo angle using PWM.

---

## 🔧 Features

- Analog joystick input (via ADC)
- PWM signal generation for servo motor control
- LCD display shows:
  - Direction (`L`, `C`, `R`)
  - ADC value
  - PWM pulse width (µs)

---

## 🧰 Hardware Components

- ATmega32 Microcontroller
- 16x2 LCD (connected in 4-bit mode)
- Analog Joystick (X-axis used)
- Servo Motor (connected to PD5 / OC1A)
- External Power Supply (if needed)
- Proteus simulation file included

---

## 🔌 Pin Connections

| Component       | Pin(s) Used      |
|----------------|------------------|
| Joystick (X)   | PA0 (ADC0)       |
| LCD (4-bit)    | PORTA, PORTB     |
| Servo Motor    | PD5 (OC1A)       |

---

## 📦 Files Included

- `src/proteusmain.c` — Main C program for proteus Simulation(PWM, ADC, LCD control)
- `src/lcd.h` — LCD Driver Header
- `src/lcd.c` - LCD Driver Source
- `simulation/Joystick-controlled-motor-Simulation.pdsprj` — Proteus project file
- `simulation/Eclipse-main.c` - Main logic for Eclipse/real-life simulation

---

## 🛠️ How It Works

1. ADC reads joystick’s X-axis.
2. Value is mapped to servo PWM pulse (520–2400µs).
3. Direction is determined (`L`, `C`, `R`) and shown on LCD.
4. Servo responds accordingly.

---



## 🧪 Simulation

Open the `.pdsprj` file in Proteus 8 or higher to simulate the project without hardware.

---
