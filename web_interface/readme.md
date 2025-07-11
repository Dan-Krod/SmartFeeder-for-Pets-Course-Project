# 🌐 SmartFeeder Web Interface

This folder contains the web interface for the **SmartFeeder** system. The UI provides real-time interaction with ESP32-based hardware via **Firebase Realtime Database**.

## 📁 Folder Structure

```
web_interface/
├── index.html       # Main HTML structure of the UI
├── styles.css       # Custom styling and fonts
├── app.js           # JavaScript for logic and Firebase
├── images/          # Local assets and pet pictures
│   └── image_1.jpg
|   └── image_2.jpg
|
└── readme.md        # This documentation
```

## 🧩 Features & Functionality

- Real-time system status display (Wi-Fi, camera, feed level)
- Manual feeding control via servo motor
- Feeding schedule using time picker
- Weight tracking (line chart via HX711 logs)
- Feeding duration chart (bar graph via ultrasonic sensor)
- Detection-based modal popup when animal appears
- Manual feed event history

## 🔌 Firebase Interaction

<table>
  <thead>
    <tr>
      <th style="text-align:left;">Feature</th>
      <th style="text-align:left;">Firebase Path</th>
      <th style="text-align:left;">Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>Manual feeding</strong></td>
      <td><code>/servo/state</code></td>
      <td>Starts or stops feeding</td>
    </tr>
    <tr>
      <td><strong>Schedule feeding time</strong></td>
      <td><code>/servo/scheduled_time</code></td>
      <td>Stores scheduled time value</td>
    </tr>
    <tr>
      <td><strong>Trigger weigh sensor</strong></td>
      <td><code>/weight_sensor/weight/activate</code></td>
      <td>Activates load cell via HX711</td>
    </tr>
    <tr>
      <td><strong>Display weight history</strong></td>
      <td><code>/weight_sensor/weight_logs</code></td>
      <td>Logs for chart and table</td>
    </tr>
    <tr>
      <td><strong>Feeding durations</strong></td>
      <td><code>/obstacle_logs</code></td>
      <td>Ultrasonic sensor logs</td>
    </tr>
    <tr>
      <td><strong>Feed action history</strong></td>
      <td><code>/manual_feedings</code></td>
      <td>Logs user-triggered feed events</td>
    </tr>
    <tr>
      <td><strong>Animal detection popup</strong></td>
      <td><code>/notifications/animal_detected/active</code></td>
      <td>Controls UI modal display</td>
    </tr>
  </tbody>
</table>

## 🚀 Getting Started

1. Open `index.html` in any modern browser.
2. Confirm internet access for:
   - Firebase SDK
   - Chart.js
   - FontAwesome and Google Fonts
3. Ensure your ESP32 is connected and writing to Firebase Realtime Database.

## 🛡️ Notes

- Built with **Firebase JS SDK v8.10.0**
- Uses [Chart.js](https://www.chartjs.org/) for data visualization
- Image paths should be relative for deployment
- Designed for local or network-based usage without a server

