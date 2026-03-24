# SOLARIS
A smart and sustainable agriculture system that integrates dual-axis solar tracking, IoT-based monitoring, and intelligent irrigation for hydroponics-based vertical farming. The project uses an ESP32-based framework to collect real-time environmental data and optimizes irrigation using fuzzy logic.


#  Dual-Axis Solar Irrigation System for Vertical Hydroponics

##  Team Members

* Krupali Furia
* Reeva Mishra
* Rutvi Gosalia

---

##  Problem Statement

Modern agriculture faces challenges such as **water scarcity, rising energy costs, and limited farming space**. While vertical farming offers an efficient solution, existing irrigation systems often rely on **manual control or fixed thresholds**, leading to inefficient water and energy usage. Additionally, static solar panels fail to utilize sunlight efficiently throughout the day.

---

##  Proposed Solution

This project presents a **smart irrigation system for hydroponics-based vertical farming** that integrates **dual-axis solar tracking, IoT, and intelligent decision-making**.

The system uses **dual-axis solar panels** to maximize energy generation and an **ESP32-based IoT setup** to collect real-time environmental data. An application layer processes this data and applies **machine learning and fuzzy logic techniques** to optimize irrigation decisions.

A simple interface enables **monitoring and control**, making the system efficient, automated, and sustainable.

---

##  Tech Stack / Components Used

###  Hardware

* ESP32 Devkit V1
* Dual-axis solar panel system
* Servo motors
* Temperature & humidity sensor
* Water level sensor
* Water temperature sensor
* Water pump

###  Software

* Python (`app.py`)
* Fuzzy Logic
* IoT Communication (ESP32 ↔ Application)

---

##  Project Structure

```
├── app.py              # Main application (ML + logic + interface)
├── esp_code/           # ESP32 code for sensor data + control
└── README.md
```

---

##  Demo Video

 *https://drive.google.com/drive/folders/1wV5MTv8M9S1t6kqCtVOa42IgHn8LHcLl?usp=sharing*

---

##  How It Works

1. ESP32 collects real-time sensor data (temperature, humidity, water parameters).
2. Data is sent to the application (`app.py`).
3. The application processes data using **ML + fuzzy logic**.
4. Irrigation decisions are made and executed.
5. System performance can be monitored by the user.

---

##  Goal

To build a **cost-effective, scalable, and energy-efficient irrigation system** that improves water usage and supports sustainable hydroponics-based vertical farming.

---


