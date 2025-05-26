# Smart Bike Light

## Introduction

Urban cycling has emerged as a sustainable and efficient mode of transportation, yet it is accompanied by persistent safety and security concerns. Inadequate visibility is a major contributor to collisions: Aalborg Universitet [^bike-safety-report] estimates that up to 2,200 bicycle accidents per year in Denmark could be averted through reliable lighting systems. Concurrently, bicycle theft imposes a substantial economic burden, with annual losses in Denmark reaching approximately 250 million DKK [^bike-theft-statistics]. These figures underscore the need for integrated technological solutions that both enhance rider visibility and deter theft.

### Product

This project introduces a smart bike light system powered by an ESP32 microcontroller, designed to enhance cyclist safety and system intelligence through sensor integration, wireless communication, and adaptive mode management. The system operates in three main modes—Active, Park, and Storage—each tailored to specific use cases and controlled through button input and ambient light sensing - a fourth mode, Stolen, also exists and can be triggered from the backend. Upon startup, the ESP32 initializes modules including WiFi for location approximation, LoRaWAN for long-range communication, a light sensor, a battery monitor and not currently on the device, but to be implemented in the future: BLE for proximity detection. 

In Active Mode, the device monitors light levels and user input. In Park mode the device wakes up every set interval, estimates its location by scanning for the three closest WiFi access points and analyzing their RSSI values. It then transmits the system data and location updates via LoRaWAN to The Things Network (TTN), which are then forwarded through an MQTT broker to a cross-platform Flutter mobile application. Likewise, in Storage mode it wakes up and sends the battery percentage. The app allows users to remotely monitor real-time location, battery level, and system status.

The light includes intuitive user interaction through button controls, and its modular hardware design supports future upgrades such as a custom PCB and 3D-printed housing.

### Problem statement and objectives

The project tackles:

- **Safety:** Riders frequently neglect to switch on or recharge their lights, especially under variable ambient conditions, which elevates the risk of nighttime and low light accidents.
- **Theft:** High incidence of bicycle theft persists, as traditional lights offer no means of tracking or recovering stolen vehicles.

The Smart Bike Light aims to:

- **Autonomous Activation:** Employ movement detection and ambient light sensing to engage lighting automatically, eliminating reliance on manual operation.
- **Geolocation Tracking:** Utilize geolocation positioning (Wi-Fi triangulation) in combination with wireless communication (LoRaWAN), and the Google API to monitor and report the bicycle’s location in real time, thereby aiding theft deterrence and recovery.
- **Remote Monitoring:** Provide a Flutter based mobile application and backend interface for users to view and interact with device status, battery level, and location data remotely.
- **Power Optimization:** Implement efficient power management strategies to maximize battery longevity in different modes.


[^bike-safety-report]: Aalborg Universitet, Bike Safety Report, 2020.<br>
[^bike-theft-statistics]: Fyens Stiftstidens, Bike Theft Statistics, 2012.

---
This repository has to main folders:
- **Smart-Bike-Light:** Which contains all the code of the [backend (BE) application/firmware](Smart-Bike-Light/README.md) of the IoT device written with Arduino IDE.
- **FE-Flutter/smart_bike_light:** Which contains all the Flutter/Dart code of the [Flutter frontend (FE) test app](FE-Flutter/smart_bike_light/README.md), while the Firebase sub folder contains code for the [Firebase solution used in FlutterFlow](FE-Flutter/smart_bike_light/Firebase).

The final report of the project from the group is located [here](34346_IoT_smart_bike_light___group_3.pdf) and the accompanying presented [slides](Group-3-slides.pdf).
