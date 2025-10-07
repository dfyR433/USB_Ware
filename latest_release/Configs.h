#ifndef CONFIGS_H
#define CONFIGS_H

// globals
String feature = "";

// Python script
const char* pymode = "e";
const char* target = "Desktop";

// DNS Port
const byte DNS_PORT = 53;

// Brute-force delay in milliseconds per attempt
int bruteDelay = 100;

// Wi-Fi AP settings
String  inputSSID   = "USB Ware";                                 // SSID
String  inputPASS   = "HID1000#";                                 // Password (leave empty for open network)
int     inputCH     = 6;                                          // Channel
bool    inputHidden = true;                                       // Hidden
int     maxClients  = 1;                                          // MAX devices allowed
uint8_t MAC[6]      = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02};       // Custom MAC
IPAddress local_IP(10, 0, 0, 1);                                  // IP
IPAddress gateway(10, 0, 0, 1);                                   // Gateway
IPAddress subnet(255, 255, 255, 0);                               // Subnet mask 

// Typing delay multiplier (affects HID typing speed)
float Delay = 50.0;

// YouTube URL for Rickroll payload
const char* linkURL = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";

//--------------------------------------------------


const char MainPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>USB Ware</title>
<style>
  body {
    font-family: "Courier New", monospace;
    background: #000;
    color: #0f0;
    margin: 0;
    padding: 20px;
    display: flex;
    flex-direction: column;
    align-items: center;
    min-height: 100vh;
  }

  h2 {
    color: #0f0;
    text-shadow: 0 0 8px #0f0, 0 0 18px #0f0;
    margin-bottom: 10px;
  }

  form {
    background: #111;
    padding: 20px;
    border-radius: 10px;
    box-shadow: 0 0 15px #0f044;
    max-width: 400px;
    width: 100%;
    display: flex;
    flex-direction: column;
    gap: 10px;
    align-items: center;
  }

  label {
    font-weight: bold;
    margin-bottom: 5px;
  }

  select {
    padding: 8px;
    border-radius: 6px;
    border: 1px solid #0f0;
    background: #111;
    color: #0f0;
    font-size: 14px;
    width: 100%;
  }

  select:focus {
    outline: none;
    box-shadow: 0 0 10px #0f0;
  }

  .small-btn {
    margin-top: 10px;
    padding: 8px 16px;
    background: #009933;
    border: none;
    border-radius: 6px;
    color: #000;
    font-size: 14px;
    font-weight: bold;
    cursor: pointer;
    text-transform: uppercase;
    transition: 0.25s;
  }

  .small-btn:hover {
    background: #00cc44;
    box-shadow: 0 0 10px #00cc44;
  }

  #status {
    margin-top: 15px;
    padding: 10px;
    border-radius: 8px;
    background: #111;
    color: #0f0;
    min-height: 80px;
    width: 100%;
    max-width: 400px;
    overflow-y: auto;
    white-space: pre-wrap;
    box-shadow: 0 0 10px #0f044;
  }
</style>
</head>
<body>

<h2>USB Ware</h2>

<form id="espForm">
  <label for="feature">Feature:</label>
  <select id="feature" name="feature">
    <option value="richroll">Ping Site</option>
    <option value="script">Python Script</option>
    <option value="script_richroll">Script + Ping</option>
    <option value="barrel_roll">Barrel Roll</option>
    <option value="note">Notepad</option>
    <option value="all">All</option>
  </select>

  <button type="button" class="small-btn" onclick="sendSettings()">Run</button>
</form>

<div id="status">Status output will appear here...</div>

<script>
async function sendSettings() {
  const form = document.getElementById('espForm');
  const params = new URLSearchParams(new FormData(form));
  const status = document.getElementById('status');
  status.innerHTML = "Processing...";

  try {
    const response = await fetch('/set', {
      method: 'POST',
      body: params
    });

    if (response.ok) {
      const text = await response.text();
      status.innerHTML = text;
    } else {
      status.innerHTML = `⚠ ESP32 responded with error: ${response.status}`;
    }
  } catch (err) {
    status.innerHTML = `❌ Request failed: ${err}`;
  }
}
</script>

</body>
</html>
)rawliteral";

#endif
