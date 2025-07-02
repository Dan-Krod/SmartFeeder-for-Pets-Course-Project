// 🔥 Firebase Initialization
const firebaseConfig = {
    //Insert your Data
};

firebase.initializeApp(firebaseConfig);
const db = firebase.database();


// 🕒 Clock Update
function updateClock() {
  const now = new Date();
  const formatted = now.toLocaleTimeString('uk-UA', { hour12: false });
  document.getElementById("currentTime").innerText = formatted;
}
setInterval(updateClock, 1000);
updateClock();


// 🐾 Notification Popup
function showNotification(msg) {
  const el = document.getElementById("notification");
  el.innerText = msg;
  el.style.display = "block";
  setTimeout(() => el.style.display = "none", 3000);
}


// ⚙️ Device Control Handlers
function setServo(state) {
  db.ref("/servo/state").set(state);
}

function sendActivationTime() {
  const time = document.getElementById("activationTime").value;
  db.ref("/servo/scheduled_time").set(time);
}

function activateWeightSensor() {
  db.ref("/weight_sensor/weight/activate").set(1);
}


// 🖥️ Real-Time State Updates
db.ref("/servo/state").on("value", (snapshot) => {
  if (snapshot.exists()) {
    const state = snapshot.val() === 1 ? "Система активна" : "Система неактивна";
    document.getElementById("servoState").innerText = state;
  }
});

db.ref("/servo/scheduled_time").on("value", (snapshot) => {
  const el = document.getElementById("scheduleState");
  el.innerText = snapshot.exists() ? snapshot.val() : "Не заплановано";
});


// 📊 Processing the feed schedule
const obstacleChartCtx = document.getElementById('obstacleChart').getContext('2d');
const obstacleChart = new Chart(obstacleChartCtx, {
  type: 'bar',
  data: {
    labels: [],
    datasets: [{
      label: 'Тривалість прийому їжі (сек)',
      data: [],
      backgroundColor: 'rgba(59, 76, 93, 0.6)',
      borderColor: 'rgba(59, 76, 93, 1)',
      borderWidth: 1
    }]
  },
  options: {
    responsive: true,
    scales: { y: { beginAtZero: true } }
  }
});

db.ref("/obstacle_logs").on("value", (snapshot) => {
  if (snapshot.exists()) {
    const data = Object.values(snapshot.val()).reverse();
    obstacleChart.data.labels = data.map(log => log.time);
    obstacleChart.data.datasets[0].data = data.map(log => log.duration);
    obstacleChart.update();
  }
});


// 📈 Weight chart
const weightChartCtx = document.getElementById("weightChart").getContext("2d");
const weightChart = new Chart(weightChartCtx, {
  type: "line",
  data: {
    labels: [],
    datasets: [{
      label: "Тренд ваги",
      data: [],
      borderColor: "rgba(41, 128, 185, 1)",
      borderWidth: 3,
      backgroundColor: "rgba(41, 128, 185, 0.2)",
      fill: true,
      pointRadius: 4,
      pointBackgroundColor: "rgba(41, 128, 185, 1)"
    }]
  },
  options: {
    responsive: true,
    scales: { y: { beginAtZero: true } }
  }
});

db.ref("/weight_sensor/weight_logs").on("value", (snapshot) => {
  if (snapshot.exists()) {
    const data = Object.values(snapshot.val());
    weightChart.data.labels = data.map(log => log.time);
    weightChart.data.datasets[0].data = data.map(log => log.weight);
    weightChart.update();
  }
});


// 📅 Data tables
function updateTable(refPath, tableId, mapper) {
  db.ref(refPath).on("value", (snapshot) => {
    const table = document.getElementById(tableId);
    table.innerHTML = "";

    if (snapshot.exists()) {
      const data = Object.values(snapshot.val()).reverse();
      data.forEach(mapper);
    } else {
      const row = table.insertRow();
      const cell = row.insertCell(0);
      cell.colSpan = 2;
      cell.innerText = "Немає даних";
    }
  });
}

updateTable("/manual_feedings", "manualFeedTable", (log) => {
  const row = manualFeedTable.insertRow();
  row.insertCell(0).innerText = log.time;
  row.insertCell(1).innerText = log.message;
});

updateTable("/weight_sensor/weight_logs", "weightTable", (log) => {
  const row = weightTable.insertRow();
  row.insertCell(0).innerText = log.time;
  row.insertCell(1).innerText = log.weight;
});

updateTable("/obstacle_logs", "obstacleTable", (log) => {
  const row = obstacleTable.insertRow();
  row.insertCell(0).innerText = log.time;
  row.insertCell(1).innerText = log.duration;
});


// 🐾 Pop-up window: pet detected
db.ref("/notifications/animal_detected/active").on("value", (snapshot) => {
  if (snapshot.exists() && snapshot.val()) {
    db.ref("/notifications/animal_detected/message").once("value").then((msgSnap) => {
      document.getElementById("modalMessage").textContent = msgSnap.val();
      document.getElementById("animalModal").style.display = "block";
    });
  }
});

document.getElementById("openStreamBtn").addEventListener("click", () => {
  window.open("http://....../", "_blank");
  db.ref("/notifications/animal_detected/active").set(false);
  document.getElementById("animalModal").style.display = "none";
});

document.getElementById("takePhotoBtn").addEventListener("click", () => {
  db.ref("/cameraControl/action").set(true);
  db.ref("/notifications/animal_detected/active").set(false);
  document.getElementById("animalModal").style.display = "none";
});

document.getElementById("cancelBtn").addEventListener("click", () => {
  db.ref("/notifications/animal_detected/active").set(false);
  document.getElementById("animalModal").style.display = "none";
});