# 🐟 IoT-Based Aquaculture Monitoring & Organic Fertilization Analysis

An embedded IoT system for real-time monitoring and analysis of water quality in aquaculture environments using ESP32 and multiple sensors.

---

## 📌 Overview

This project is designed to analyze the effect of **organic fertilization** in fish farming ponds by continuously monitoring key water parameters. The system uses **ESP32** for data acquisition and sends real-time data to **ThingSpeak cloud** for visualization and analysis.

---

## 🎯 Objectives

- Monitor water quality in real-time
- Analyze the effect of organic fertilization
- Provide cloud-based visualization
- Improve fish farming efficiency and sustainability

---

## 🧰 Hardware Components

- ESP32 Microcontroller
- pH Sensor
- TDS Sensor
- Turbidity Sensor
- Temperature Sensor (DS18B20 recommended)
- Water Level Sensor
- 20x4 LCD Display (I2C)
- WiFi (built-in ESP32)

---

## 🧠 System Architecture

Sensors collect water data → ESP32 processes data →  
Displays on LCD → Sends data to ThingSpeak Cloud →  
User monitors via dashboard

---

## 🔄 Working Principle

1. Sensors collect real-time water parameters
2. ESP32 reads and processes sensor data
3. Data is displayed on a 20x4 LCD
4. Data is sent to ThingSpeak via WiFi
5. Graphs and analytics help evaluate fertilization impact

---

## 📊 Parameters Monitored

| Parameter   | Purpose                         |
|------------|--------------------------------|
| pH         | Water acidity/alkalinity       |
| TDS        | Nutrient concentration         |
| Turbidity  | Water clarity                  |
| Temperature| Fish growth condition          |
| Water Level| Pond safety monitoring         |

---

## ☁️ ThingSpeak Integration

- Create a channel on ThingSpeak
- Assign fields:
  - Field1: pH
  - Field2: TDS
  - Field3: Turbidity
  - Field4: Temperature
  - Field5: Water Level
- Use Write API Key in ESP32 code

---

## 💡 Data Analysis Insight

- Organic fertilizer increases nutrient levels (TDS)
- Turbidity may rise initially
- pH variation indicates biological activity

---
