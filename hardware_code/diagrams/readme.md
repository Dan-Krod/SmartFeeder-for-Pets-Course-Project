# 📊 System Diagrams for Smart Feeder

This directory contains visual documentation for the **Smart Feeder for Pets** course project. The diagrams provide both high-level and detailed views of the architecture, logic, and component interactions across hardware, backend, and frontend layers.

---

## 🧩 Diagram 1: System Interaction Overview

**Filename:** `basic_logic_diagram.jpg`  

### 🔍 Description:
This diagram presents a simplified yet complete overview of the **four key subsystems** involved in the smart feeder solution:

### 🧱 Core Blocks:
- **HARDWARE**  
  - Includes the ESP32-CAM module with sensors (HC-SR04, HX711), servo motor, camera, and OLED display.
  - Collects environmental data and executes physical actions (e.g., dispensing food).

- **BACK-END**  
  - Refers to the ESP32’s embedded logic, which:
    - Processes sensor data
    - Manages feeding schedules
    - Uploads images
    - Controls local display updates and logic

- **DATABASE (Firebase Realtime DB)**  
  - Acts as the central data exchange and persistence layer.
  - Logs feeding events, weight readings, photos, and obstacle detection info.
  - Enables remote synchronization between hardware and frontend.

- **WEB-INTERFACE**  
  - Built with HTML/CSS/JS + Firebase SDK.
  - Allows users to:
    - Trigger feedings
    - Schedule automatic actions
    - View charts and logs of pet activity
    - Monitor system status in real time

---

## 🔬 Diagram 2: Detailed Logic Architecture

**Filename:** `detailed_logic_diagram.png`  

### 🔍 Description:
This advanced logic diagram reveals how each module operates and interacts at runtime — from hardware routines to real-time database syncing and user-triggered events.

### 🧱 Components:

#### **1. ESP32-CAM Hardware**
- 📡 **Wi-Fi Module** – Enables connection to Firebase and Google Apps Script
- 📷 **Camera** – Captures pet photos on command or automatically
- ⚙️ **Servo Motor** – Dispenses food by rotating on schedule or manually
- 📏 **HC-SR04 Sensor** – Detects animal presence; triggers alerts and logs
- ⚖️ **Load Cell (HX711)** – Measures food intake with timed sampling
- 🖥️ **OLED Display** – Cycles time, feeding status, weight data

#### **2. Firebase Realtime Database**
- Synchronizes feeding times, user commands, weight logs, photos, and alerts
- Acts as a central state server for both the web UI and device logic

#### **3. Web UI (Frontend)**
- Real-time interface powered by Firebase SDK
- Supports:
  - Feeding scheduler
  - Real-time data visualization (Chart.js)
  - Manual control actions (buttons, modals)
  - Historical logs for weight and feeding events

#### **4. Google Drive via Apps Script**
- Captures and stores photos sent by the ESP32
- Each photo is base64-encoded, then POSTed via an HTTP request

---

## 🔄 Communication Flows:
<h3>Communication Flows:</h3>

<table>
  <thead>
    <tr>
      <th style="text-align:left;">From → To</th>
      <th style="text-align:left;">Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>HARDWARE → BACK-END</strong></td>
      <td>Captures input from sensors (distance, weight, button presses)</td>
    </tr>
    <tr>
      <td><strong>BACK-END → DATABASE</strong></td>
      <td>Uploads data, logs, feeding records</td>
    </tr>
    <tr>
      <td><strong>DATABASE → WEB-INTERFACE</strong></td>
      <td>Delivers real-time data to the UI (charts, state)</td>
    </tr>
    <tr>
      <td><strong>WEB-INTERFACE → DATABASE → BACK-END</strong></td>
      <td>Sends control commands (e.g., activate servo, schedule feeding)</td>
    </tr>
    <tr>
      <td><strong>BACK-END → Google Drive</strong></td>
      <td>Sends photo logs via Apps Script (base64-encoded)</td>
    </tr>
  </tbody>
</table>

---
> This overview helps orient contributors and reviewers to the architecture without diving into full implementation details.
