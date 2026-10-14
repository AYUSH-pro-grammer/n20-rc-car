
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Stanza_Car";
const char* password = "drive123";

WebServer server(80);

const int PWMA = 25;
const int AIN1 = 26;
const int AIN2 = 27;

const int PWMB = 14;
const int BIN1 = 16;
const int BIN2 = 17;

const int STBY = 33;

int motorSpeed = 200;
String currentMove = "stop";
unsigned long lastCommand = 0;

const char page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Stanza Car</title>
<style>
body {
  background: #fff;
  color: #111;
  font-family: Arial, sans-serif;
  text-align: center;
  max-width: 360px;
  margin: 30px auto;
  padding: 15px;
}
input { width: 100%; }
.controls {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 10px;
  margin-top: 25px;
}
button {
  background: white;
  color: black;
  border: 1px solid #999;
  border-radius: 5px;
  padding: 18px 4px;
  font-size: 16px;
  touch-action: none;
  user-select: none;
}
button:active { background: #ddd; }
#stop { background: #eee; font-weight: bold; }
.empty { visibility: hidden; }
</style>
</head>
<body>
<h2>Stanza Car</h2>
<p>Speed: <span id="speedValue">200</span></p>
<input id="speed" type="range" min="0" max="255" value="200">

<div class="controls">
  <button class="empty"></button>
  <button data-move="forward">Forward</button>
  <button class="empty"></button>

  <button data-move="left">Left</button>
  <button id="stop">STOP</button>
  <button data-move="right">Right</button>

  <button class="empty"></button>
  <button data-move="backward">Backward</button>
  <button class="empty"></button>
</div>

<p>Hold a direction to move. Release to stop.</p>

<script>
const slider = document.getElementById('speed');
const speedValue = document.getElementById('speedValue');

let currentMove = 'stop';
let timer = null;
let busy = false;

async function sendCommand(move) {
  if (busy) return;
  busy = true;

  try {
    await fetch('/cmd?move=' + move + '&speed=' + slider.value,
      { cache: 'no-store' });
  } catch (error) {
  } finally {
    busy = false;
  }
}

function startMove(move) {
  clearInterval(timer);
  currentMove = move;
  sendCommand(move);

  timer = setInterval(() => {
    if (currentMove !== 'stop') sendCommand(currentMove);
  }, 250);
}

function stopMove() {
  clearInterval(timer);
  timer = null;
  currentMove = 'stop';
  sendCommand('stop');
}

document.querySelectorAll('[data-move]').forEach(button => {
  button.addEventListener('pointerdown', event => {
    event.preventDefault();
    startMove(button.dataset.move);
  });

  button.addEventListener('pointerup', stopMove);
  button.addEventListener('pointercancel', stopMove);
  button.addEventListener('lostpointercapture', stopMove);
});

document.getElementById('stop').addEventListener('pointerdown', event => {
  event.preventDefault();
  stopMove();
});

slider.addEventListener('input', () => {
  speedValue.textContent = slider.value;
  if (currentMove !== 'stop') sendCommand(currentMove);
});

window.addEventListener('blur', stopMove);
window.addEventListener('pagehide', stopMove);

document.addEventListener('visibilitychange', () => {
  if (document.hidden) stopMove();
});
</script>
</body>
</html>
)rawliteral";

void setMotors(int left, int right) {
  left = constrain(left, -255, 255);
  right = constrain(right, -255, 255);

  digitalWrite(AIN1, left > 0 ? HIGH : LOW);
  digitalWrite(AIN2, left < 0 ? HIGH : LOW);
  digitalWrite(BIN1, right > 0 ? HIGH : LOW);
  digitalWrite(BIN2, right < 0 ? HIGH : LOW);

  ledcWrite(PWMA, abs(left));
  ledcWrite(PWMB, abs(right));
}

void stopMotors() {
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
  currentMove = "stop";
}

void moveCar(String move) {
  currentMove = move;
  lastCommand = millis();

  if (move == "forward") {
    setMotors(motorSpeed, motorSpeed);
  } else if (move == "backward") {
    setMotors(-motorSpeed, -motorSpeed);
  } else if (move == "left") {
    setMotors(-motorSpeed, motorSpeed);
  } else if (move == "right") {
    setMotors(motorSpeed, -motorSpeed);
  } else {
    stopMotors();
  }
}

void handleCommand() {
  String move = server.arg("move");

  if (server.hasArg("speed")) {
    motorSpeed = constrain(server.arg("speed").toInt(), 0, 255);
  }

  if (move == "forward" || move == "backward" ||
      move == "left" || move == "right") {
    moveCar(move);
  } else {
    stopMotors();
    lastCommand = millis();
  }

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  bool pwmA = ledcAttach(PWMA, 1000, 8);
  bool pwmB = ledcAttach(PWMB, 1000, 8);

  digitalWrite(STBY, LOW);
  stopMotors();

  if (!pwmA || !pwmB) {
    Serial.println("PWM setup failed");
    while (true) delay(1000);
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("Connect to Wi-Fi: ");
  Serial.println(ssid);
  Serial.print("Open: http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() {
    server.send_P(200, "text/html; charset=utf-8", page);
  });

  server.on("/cmd", handleCommand);
  server.begin();

  digitalWrite(STBY, HIGH);
  Serial.println("Car controller ready");
}

void loop() {
  server.handleClient();

  if (currentMove != "stop" &&
      millis() - lastCommand > 700) {
    stopMotors();
  }
}
