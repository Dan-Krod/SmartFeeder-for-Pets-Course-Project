# 📘 Schematic Overview

This folder contains the schematic diagram for the **SmartFeeder Course Project**, created in KiCad 7. It represents a modular IoT hardware architecture centered around the ESP32-S3 microcontroller, integrating sensing, actuation, and feedback components.

## 🔌 System Architecture

The circuit is structured around **functional modules** connected to the ESP32 as follows:

<table>
  <thead>
    <tr>
      <th style="text-align:left;">Module</th>
      <th style="text-align:left;">Interface</th>
      <th style="text-align:left;">ESP32 Pins Involved</th>
      <th style="text-align:left;">Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td rowspan="3"><strong>OV2640 Camera</strong></td>
      <td rowspan="3">Parallel / I²C</td>
      <td>D0–D9, XCLK, SIOD,</td>
      <td rowspan="3">Captures images from the environment</td>
    </tr>
    <tr><td>SIOC, VSYNC, HREF,</td></tr>
    <tr><td>PCLK, PWDN, RESET</td></tr>
    <tr>
      <td><strong>HC-SR04</strong></td>
      <td>Digital IO</td>
      <td>Trig, Echo</td>
      <td>Measures distance to detect food level</td>
    </tr>
    <tr>
      <td><strong>HX711 + Load Cell</strong></td>
      <td>Digital IO</td>
      <td>DT, SCK</td>
      <td>Measures food weight</td>
    </tr>
    <tr>
      <td><strong>Servo Motor</strong></td>
      <td>PWM</td>
      <td>PWM (e.g., IO13)</td>
      <td>Controls mechanical dispensing mechanism</td>
    </tr>
    <tr>
      <td><strong>OLED SSD1306</strong></td>
      <td>I²C</td>
      <td>SDA, SCL</td>
      <td>Displays real-time system feedback</td>
    </tr>
  </tbody>
</table>

Each module is powered via 3.3V or 5V rails depending on requirement, with appropriate ground references. The schematic includes pull-up resistors on I²C lines and decoupling capacitors for power stabilization.

## 🛠️ Notable Design Aspects

- **Labeling conventions**: Signal lines are clearly named (e.g., `CAM_D7`, `SERVO_PWM`, `WEIGHT_DT`) for traceability across the project.
- **Power domain separation**: Digital sensors operate at 5V, while ESP32 operates at 3.3V. Level shifting is handled implicitly via module specs or GPIO tolerance.
- **Modularity**: The schematic allows individual components to be prototyped or replaced independently.

## 📂 Files Included

- `schematic.png` – High-resolution render for quick reference

## 🧭 Usage Recommendations

- Open in **KiCad 7.x** or newer for best compatibility.
- Match module pinouts exactly as shown — especially for the OV2640, which has many parallel data lines.
- Before PCB layout, verify I²C pull-up values and servo power routing for peak current.

## ⚠️ Known Constraints

- The camera and HX711 modules may be sensitive to bus timing — firmware must handle them on separate tasks or queues.
- Ensure no GPIO overlap across modules unless multiplexing is intended.

---
